#pragma once
#include <nlohmann/json.hpp>
#include "Database.h"

using json = nlohmann::json;

class CampusRepo {
public:
    static json getPuntos() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT id, nombre, tipo, x, y, es_edificio, "
            "COALESCE(descripcion,'') AS descripcion "
            "FROM puntos_campus ORDER BY id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"nombre",      row["nombre"].c_str()},
                           {"tipo",        row["tipo"].c_str()},
                           {"x",           row["x"].as<double>()},
                           {"y",           row["y"].as<double>()},
                           {"es_edificio", row["es_edificio"].as<bool>()},
                           {"descripcion", row["descripcion"].c_str()}});
        return arr;
    }

    static json getConexiones() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT id, origen_id, destino_id, distancia, tipo_camino "
            "FROM conexiones_campus ORDER BY id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"origen_id",   row["origen_id"].as<int>()},
                           {"destino_id",  row["destino_id"].as<int>()},
                           {"distancia",   row["distancia"].as<double>()},
                           {"tipo_camino", row["tipo_camino"].c_str()}});
        return arr;
    }

    static int crearPunto(const std::string& nombre, const std::string& tipo,
                          double x, double y, bool es_edificio,
                          const std::string& descripcion) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO puntos_campus(nombre,tipo,x,y,es_edificio,descripcion) "
            "VALUES($1,$2,$3,$4,$5,$6) RETURNING id",
            nombre, tipo, x, y, es_edificio, descripcion);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static int crearConexion(int origen, int destino, double distancia,
                              const std::string& tipo_camino) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO conexiones_campus(origen_id,destino_id,distancia,tipo_camino) "
            "VALUES($1,$2,$3,$4) RETURNING id",
            origen, destino, distancia, tipo_camino);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static bool updatePunto(int id, const std::string& nombre, const std::string& tipo,
                            double x, double y, bool es_edificio,
                            const std::string& descripcion) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params(
            "UPDATE puntos_campus SET nombre=$1,tipo=$2,x=$3,y=$4,"
            "es_edificio=$5,descripcion=$6 WHERE id=$7",
            nombre, tipo, x, y, es_edificio, descripcion, id);
        txn.commit();
        return true;
    }

    static int contarConexiones(int id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT COUNT(*) AS n FROM conexiones_campus "
            "WHERE origen_id=$1 OR destino_id=$1", id);
        txn.commit();
        return r[0]["n"].as<int>();
    }

    static bool deletePunto(int id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params(
            "DELETE FROM conexiones_campus WHERE origen_id=$1 OR destino_id=$1", id);
        txn.exec_params("DELETE FROM puntos_campus WHERE id=$1", id);
        txn.commit();
        return true;
    }

    static bool deleteConexion(int id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params("DELETE FROM conexiones_campus WHERE id=$1", id);
        txn.commit();
        return true;
    }
};



class HistorialRepo {
public:
    static int registrar(int usuario_id, const std::string& accion,
                         const std::string& descripcion, const std::string& datos_json = "") {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO historial_acciones(usuario_id,accion,descripcion,datos_json) "
            "VALUES($1,$2,$3,$4) RETURNING id",
            usuario_id, accion, descripcion, datos_json);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static json findByUsuario(int usuario_id, int limit = 20) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT id, accion, descripcion, datos_json, timestamp "
            "FROM historial_acciones WHERE usuario_id=$1 "
            "ORDER BY id DESC LIMIT $2", usuario_id, limit);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"accion",      row["accion"].c_str()},
                           {"descripcion", row["descripcion"].c_str()},
                           {"datos_json",  row["datos_json"].is_null() ? "" : row["datos_json"].c_str()},
                           {"timestamp",   row["timestamp"].c_str()}});
        return arr;
    }

    static json findAll(int limit = 50) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT h.id, h.accion, h.descripcion, h.timestamp, u.nombre AS usuario "
            "FROM historial_acciones h LEFT JOIN usuarios u ON h.usuario_id=u.id "
            "ORDER BY h.id DESC LIMIT $1", limit);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"accion",      row["accion"].c_str()},
                           {"descripcion", row["descripcion"].c_str()},
                           {"usuario",     row["usuario"].is_null() ? "" : row["usuario"].c_str()},
                           {"timestamp",   row["timestamp"].c_str()}});
        return arr;
    }
};

class ReporteRepo {
public:
    static json statsTurnos() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT tipo_tramite, COUNT(*) AS total "
            "FROM turnos GROUP BY tipo_tramite ORDER BY total DESC");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"tipo",  row["tipo_tramite"].c_str()},
                           {"total", row["total"].as<int>()}});
        return arr;
    }

    static json statsTramites() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT estado, COUNT(*) AS total FROM tramites GROUP BY estado ORDER BY total DESC");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"estado", row["estado"].c_str()},
                           {"total",  row["total"].as<int>()}});
        return arr;
    }

    static json statsGenerales() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT "
            "(SELECT COUNT(*) FROM usuarios WHERE activo=TRUE) AS usuarios,"
            "(SELECT COUNT(*) FROM estudiantes) AS estudiantes,"
            "(SELECT COUNT(*) FROM tramites) AS tramites,"
            "(SELECT COUNT(*) FROM turnos WHERE DATE(created_at)=CURRENT_DATE) AS turnos_hoy,"
            "(SELECT COUNT(*) FROM documentos) AS documentos");
        txn.commit();
        return {{"usuarios",    r[0]["usuarios"].as<int>()},
                {"estudiantes", r[0]["estudiantes"].as<int>()},
                {"tramites",    r[0]["tramites"].as<int>()},
                {"turnos_hoy",  r[0]["turnos_hoy"].as<int>()},
                {"documentos",  r[0]["documentos"].as<int>()}};
    }
};


