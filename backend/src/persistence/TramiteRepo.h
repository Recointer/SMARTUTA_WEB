#pragma once
#include <nlohmann/json.hpp>
#include "Database.h"

using json = nlohmann::json;

class TramiteRepo {
public:
    static json findAll(int limit = 100) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT tr.id, tr.descripcion, tr.estado, tr.created_at, tr.updated_at, "
            "e.nombre||' '||e.apellido AS estudiante, "
            "tt.nombre AS tipo_tramite "
            "FROM tramites tr "
            "LEFT JOIN estudiantes e ON tr.estudiante_id=e.id "
            "LEFT JOIN tipos_tramite tt ON tr.tipo_tramite_id=tt.id "
            "ORDER BY tr.id DESC LIMIT $1", limit);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",           row["id"].as<int>()},
                           {"descripcion",  row["descripcion"].c_str()},
                           {"estado",       row["estado"].c_str()},
                           {"estudiante",   row["estudiante"].is_null() ? "" : row["estudiante"].c_str()},
                           {"tipo_tramite", row["tipo_tramite"].is_null() ? "" : row["tipo_tramite"].c_str()},
                           {"created_at",   row["created_at"].c_str()},
                           {"updated_at",   row["updated_at"].c_str()}});
        return arr;
    }

    static json findByEstudiante(int estudiante_id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT tr.id, tr.descripcion, tr.estado, tr.created_at, tr.updated_at, "
            "tt.nombre AS tipo_tramite "
            "FROM tramites tr "
            "LEFT JOIN tipos_tramite tt ON tr.tipo_tramite_id=tt.id "
            "WHERE tr.estudiante_id=$1 ORDER BY tr.id DESC", estudiante_id);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",           row["id"].as<int>()},
                           {"descripcion",  row["descripcion"].c_str()},
                           {"estado",       row["estado"].c_str()},
                           {"tipo_tramite", row["tipo_tramite"].is_null() ? "" : row["tipo_tramite"].c_str()},
                           {"created_at",   row["created_at"].c_str()},
                           {"updated_at",   row["updated_at"].c_str()}});
        return arr;
    }

    static json findById(int id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT tr.id, tr.descripcion, tr.estado, tr.created_at, tr.updated_at, "
            "tr.estudiante_id, e.nombre||' '||e.apellido AS estudiante, "
            "tr.tipo_tramite_id, tt.nombre AS tipo_tramite "
            "FROM tramites tr "
            "LEFT JOIN estudiantes e ON tr.estudiante_id=e.id "
            "LEFT JOIN tipos_tramite tt ON tr.tipo_tramite_id=tt.id "
            "WHERE tr.id=$1", id);
        txn.commit();
        if (r.empty()) return nullptr;
        auto& row = r[0];
        return {{"id",              row["id"].as<int>()},
                {"descripcion",     row["descripcion"].c_str()},
                {"estado",          row["estado"].c_str()},
                {"estudiante_id",   row["estudiante_id"].is_null() ? 0 : row["estudiante_id"].as<int>()},
                {"estudiante",      row["estudiante"].is_null() ? "" : row["estudiante"].c_str()},
                {"tipo_tramite_id", row["tipo_tramite_id"].is_null() ? 0 : row["tipo_tramite_id"].as<int>()},
                {"tipo_tramite",    row["tipo_tramite"].is_null() ? "" : row["tipo_tramite"].c_str()},
                {"created_at",      row["created_at"].c_str()},
                {"updated_at",      row["updated_at"].c_str()}};
    }

    static int crear(int estudiante_id, int tipo_tramite_id, const std::string& descripcion) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO tramites(estudiante_id,tipo_tramite_id,descripcion,estado) "
            "VALUES($1,$2,$3,'pendiente') RETURNING id",
            estudiante_id, tipo_tramite_id, descripcion);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static bool actualizarEstado(int id, const std::string& estado) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params(
            "UPDATE tramites SET estado=$1, updated_at=NOW() WHERE id=$2", estado, id);
        txn.commit();
        return true;
    }

    static json getTipos() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec("SELECT id, nombre, descripcion FROM tipos_tramite WHERE activo=TRUE ORDER BY id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"nombre",      row["nombre"].c_str()},
                           {"descripcion", row["descripcion"].c_str()}});
        return arr;
    }

    static json getEstudiantes() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec("SELECT id, cedula, nombre||' '||apellido AS nombre_completo, carrera "
                          "FROM estudiantes ORDER BY nombre");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",             row["id"].as<int>()},
                           {"cedula",         row["cedula"].c_str()},
                           {"nombre_completo",row["nombre_completo"].c_str()},
                           {"carrera",        row["carrera"].is_null() ? "" : row["carrera"].c_str()}});
        return arr;
    }
};


