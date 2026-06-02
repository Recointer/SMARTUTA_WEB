#pragma once
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "../domain/Pila.h"
#include "../domain/ListaSimple.h"
#include "../domain/ListaDoble.h"
#include "../persistence/CampusRepo.h"

using json = nlohmann::json;

struct AccionInfo {
    int         id;
    std::string accion;
    std::string descripcion;
    std::string timestamp;
};

// ============================================================
// HistorialService — Pila LIFO por usuario en memoria
// ============================================================
class HistorialService {
    std::unordered_map<int, Pila<AccionInfo>*> pilas; // por usuario_id
    std::mutex mtx;

    HistorialService() {}
public:
    static HistorialService& instance() { static HistorialService s; return s; }

    ~HistorialService() { for (auto& [k, v] : pilas) delete v; }

    void registrar(int usuario_id, const std::string& accion,
                   const std::string& desc, const std::string& datos = "") {
        std::lock_guard<std::mutex> lock(mtx);
        int id = HistorialRepo::registrar(usuario_id, accion, desc, datos);
        if (!pilas.count(usuario_id)) pilas[usuario_id] = new Pila<AccionInfo>(50);
        pilas[usuario_id]->apilar({id, accion, desc, ""});
    }

    json getCima(int usuario_id) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!pilas.count(usuario_id) || pilas[usuario_id]->estaVacia())
            return {{"ok", false}, {"error", "Sin historial"}};
        auto& a = pilas[usuario_id]->verCima();
        return {{"ok", true}, {"id", a.id}, {"accion", a.accion}, {"descripcion", a.descripcion}};
    }

    json deshacer(int usuario_id) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!pilas.count(usuario_id) || pilas[usuario_id]->estaVacia())
            return {{"ok", false}, {"error", "Nada que deshacer"}};
        auto a = pilas[usuario_id]->desapilar();
        return {{"ok", true}, {"deshecho", a.accion}, {"descripcion", a.descripcion}};
    }

    int tamanioPila(int usuario_id) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!pilas.count(usuario_id)) return 0;
        return pilas[usuario_id]->size();
    }
};

// ============================================================
// TramiteService — ListaSimple + ListaDoble para expedientes
// ============================================================
class TramiteService {
public:
    // Construye ListaSimple de trámites de un estudiante (on-demand)
    static json getExpediente(int estudiante_id) {
        auto tramites = TramiteRepo::findByEstudiante(estudiante_id);
        // Cargar en ListaSimple para demostrar la estructura
        ListaSimple<json> lista;
        for (auto& t : tramites) lista.insertarAlFinal(t);
        // Cargar en ListaDoble para navegación bidireccional
        ListaDoble<json> doble;
        for (auto& t : tramites) doble.insertarAlFinal(t);
        // Retornar como JSON con metadatos de navegación
        json result = json::array();
        lista.recorrer([&](const json& t) { result.push_back(t); });
        return {{"tramites", result},
                {"total",    lista.size()},
                {"tiene_anterior", doble.actual() && doble.anterior() != nullptr}};
    }
};
