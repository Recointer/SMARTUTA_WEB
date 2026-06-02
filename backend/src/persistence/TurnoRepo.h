#pragma once
#include <nlohmann/json.hpp>
#include "Database.h"

using json = nlohmann::json;

class TurnoRepo {
public:
    static json findPendientes() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT t.id, t.numero_turno, t.tipo_tramite, t.estado, "
            "t.created_at, u.nombre AS usuario "
            "FROM turnos t LEFT JOIN usuarios u ON t.usuario_id = u.id "
            "WHERE t.estado='pendiente' ORDER BY t.numero_turno");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",           row["id"].as<int>()},
                           {"numero_turno", row["numero_turno"].as<int>()},
                           {"tipo_tramite", row["tipo_tramite"].c_str()},
                           {"estado",       row["estado"].c_str()},
                           {"usuario",      row["usuario"].is_null() ? "" : row["usuario"].c_str()},
                           {"created_at",   row["created_at"].c_str()}});
        return arr;
    }

    static json findAll(int limit = 50) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT t.id, t.numero_turno, t.tipo_tramite, t.estado, "
            "t.created_at, t.atendido_at, u.nombre AS usuario "
            "FROM turnos t LEFT JOIN usuarios u ON t.usuario_id = u.id "
            "ORDER BY t.id DESC LIMIT $1", limit);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",           row["id"].as<int>()},
                           {"numero_turno", row["numero_turno"].as<int>()},
                           {"tipo_tramite", row["tipo_tramite"].c_str()},
                           {"estado",       row["estado"].c_str()},
                           {"usuario",      row["usuario"].is_null() ? "" : row["usuario"].c_str()},
                           {"created_at",   row["created_at"].c_str()},
                           {"atendido_at",  row["atendido_at"].is_null() ? "" : row["atendido_at"].c_str()}});
        return arr;
    }

    static int siguienteNumero() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec("SELECT COALESCE(MAX(numero_turno),0)+1 AS n FROM turnos "
                          "WHERE DATE(created_at)=CURRENT_DATE");
        txn.commit();
        return r[0]["n"].as<int>();
    }

    static int crear(int usuario_id, const std::string& tipo_tramite) {
        int num = siguienteNumero();
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO turnos(numero_turno,usuario_id,tipo_tramite,estado) "
            "VALUES($1,$2,$3,'pendiente') RETURNING id",
            num, usuario_id, tipo_tramite);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static bool atender(int id, int ventanilla_id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params(
            "UPDATE turnos SET estado='atendido', ventanilla_id=$1, "
            "atendido_at=NOW() WHERE id=$2", ventanilla_id, id);
        txn.commit();
        return true;
    }

    static json stats() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT "
            "COUNT(*) FILTER(WHERE estado='pendiente') AS pendientes,"
            "COUNT(*) FILTER(WHERE estado='atendido') AS atendidos,"
            "COUNT(*) AS total "
            "FROM turnos WHERE DATE(created_at)=CURRENT_DATE");
        txn.commit();
        return {{"pendientes", r[0]["pendientes"].as<int>()},
                {"atendidos",  r[0]["atendidos"].as<int>()},
                {"total",      r[0]["total"].as<int>()}};
    }
    static json findByUsuario(int usuario_id, int limit = 50) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT t.id, t.numero_turno, t.tipo_tramite, t.estado, "
            "t.created_at, t.atendido_at, u.nombre AS usuario "
            "FROM turnos t LEFT JOIN usuarios u ON t.usuario_id = u.id "
            "WHERE t.usuario_id=$1 ORDER BY t.id DESC LIMIT $2",
            usuario_id, limit);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",           row["id"].as<int>()},
                           {"numero_turno", row["numero_turno"].as<int>()},
                           {"tipo_tramite", row["tipo_tramite"].c_str()},
                           {"estado",       row["estado"].c_str()},
                           {"usuario",      row["usuario"].is_null() ? "" : row["usuario"].c_str()},
                           {"created_at",   row["created_at"].c_str()},
                           {"atendido_at",  row["atendido_at"].is_null() ? "" : row["atendido_at"].c_str()}});
        return arr;
    }
};
