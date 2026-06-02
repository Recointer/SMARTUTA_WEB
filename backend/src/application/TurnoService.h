#pragma once
#include <mutex>
#include <nlohmann/json.hpp>
#include "../domain/Cola.h"
#include "../domain/ListaCircular.h"
#include "../persistence/TurnoRepo.h"
#include "../persistence/CampusRepo.h"

using json = nlohmann::json;

struct TurnoInfo { int id; int numero; std::string tipo; std::string usuario; };
struct VentanillaInfo { int id; std::string nombre; };

// ============================================================
// TurnoService — Cola FIFO en memoria + ListaCircular ventanillas
// ============================================================
class TurnoService {
    Cola<TurnoInfo>            colaTurnos;
    ListaCircular<VentanillaInfo> ventanillas;
    std::mutex                 mtx;

    TurnoService() {
        // Cargar turnos pendientes de hoy desde la BD
        auto pendientes = TurnoRepo::findPendientes();
        for (auto& t : pendientes)
            colaTurnos.encolar({t["id"].get<int>(),
                                t["numero_turno"].get<int>(),
                                t["tipo_tramite"].get<std::string>(),
                                t["usuario"].get<std::string>()});
        // Cargar ventanillas
        try {
            auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
            auto r = txn.exec("SELECT id, nombre FROM ventanillas WHERE activo=TRUE");
            txn.commit();
            for (const auto row : r)
                ventanillas.insertar({row["id"].as<int>(), row["nombre"].c_str()});
        } catch (...) {}
    }

public:
    static TurnoService& instance() { static TurnoService s; return s; }

    json registrarTurno(int usuario_id, const std::string& tipo) {
        std::lock_guard<std::mutex> lock(mtx);
        try {
            int id = TurnoRepo::crear(usuario_id, tipo);
            // Consultar el número asignado
            auto pendientes = TurnoRepo::findPendientes();
            for (auto& t : pendientes)
                if (t["id"].get<int>() == id) {
                    TurnoInfo ti{id, t["numero_turno"].get<int>(), tipo, ""};
                    colaTurnos.encolar(ti);
                    return {{"ok", true}, {"id", id}, {"numero_turno", ti.numero}};
                }
            return {{"ok", false}, {"error", "Error al registrar"}};
        } catch (const std::exception& e) {
            return {{"ok", false}, {"error", e.what()}};
        }
    }

    json atenderSiguiente() {
        std::lock_guard<std::mutex> lock(mtx);
        if (colaTurnos.estaVacia())
            return {{"ok", false}, {"error", "Cola vacía"}};
        TurnoInfo ti = colaTurnos.desencolar();
        // Asignar ventanilla Round-Robin
        int vent_id = 1;
        if (!ventanillas.estaVacia()) {
            auto& v = ventanillas.siguiente();
            vent_id = v.id;
        }
        TurnoRepo::atender(ti.id, vent_id);
        return {{"ok", true}, {"turno_atendido", ti.id},
                {"numero", ti.numero}, {"restantes", colaTurnos.size()}};
    }

    json estadoCola() {
        std::lock_guard<std::mutex> lock(mtx);
        auto stats = TurnoRepo::stats();
        stats["en_memoria"] = colaTurnos.size();
        if (!colaTurnos.estaVacia())
            stats["siguiente_numero"] = colaTurnos.verFrente().numero;
        return stats;
    }

    json getVentanillaActual() {
        if (ventanillas.estaVacia()) return {{"nombre", "Sin ventanilla"}};
        return {{"nombre", ventanillas.actual().nombre},
                {"id",     ventanillas.actual().id}};
    }
};


