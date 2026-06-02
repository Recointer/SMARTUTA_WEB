#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "Database.h"

using json = nlohmann::json;

struct Usuario {
    int         id;
    std::string nombre;
    std::string email;
    std::string password_hash;
    int         rol_id;
    std::string rol_nombre;
    bool        activo;
};

class UsuarioRepo {
public:
    static json findByEmail(const std::string& email) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT u.id, u.nombre, u.email, u.password_hash, u.rol_id, r.nombre AS rol, u.activo "
            "FROM usuarios u JOIN roles r ON u.rol_id = r.id "
            "WHERE u.email = $1 AND u.activo = TRUE", email);
        txn.commit();
        if (r.empty()) return nullptr;
        auto row = r[0];
        return {{"id",            row["id"].as<int>()},
                {"nombre",        row["nombre"].c_str()},
                {"email",         row["email"].c_str()},
                {"password_hash", row["password_hash"].c_str()},
                {"rol_id",        row["rol_id"].as<int>()},
                {"rol",           row["rol"].c_str()},
                {"activo",        row["activo"].as<bool>()}};
    }

    static json findById(int id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "SELECT u.id, u.nombre, u.email, u.rol_id, r.nombre AS rol, u.activo, u.created_at "
            "FROM usuarios u JOIN roles r ON u.rol_id = r.id WHERE u.id = $1", id);
        txn.commit();
        if (r.empty()) return nullptr;
        auto row = r[0];
        return {{"id",         row["id"].as<int>()},
                {"nombre",     row["nombre"].c_str()},
                {"email",      row["email"].c_str()},
                {"rol_id",     row["rol_id"].as<int>()},
                {"rol",        row["rol"].c_str()},
                {"activo",     row["activo"].as<bool>()},
                {"created_at", row["created_at"].c_str()}};
    }

    static json findAll() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT u.id, u.nombre, u.email, u.rol_id, r.nombre AS rol, u.activo, u.created_at "
            "FROM usuarios u JOIN roles r ON u.rol_id = r.id ORDER BY u.id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r) {
            arr.push_back({{"id",         row["id"].as<int>()},
                           {"nombre",     row["nombre"].c_str()},
                           {"email",      row["email"].c_str()},
                           {"rol_id",     row["rol_id"].as<int>()},
                           {"rol",        row["rol"].c_str()},
                           {"activo",     row["activo"].as<bool>()},
                           {"created_at", row["created_at"].c_str()}});
        }
        return arr;
    }

    static int crear(const std::string& nombre, const std::string& email,
                     const std::string& password_hash, int rol_id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO usuarios(nombre,email,password_hash,rol_id) "
            "VALUES($1,$2,$3,$4) RETURNING id",
            nombre, email, password_hash, rol_id);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static bool actualizarActivo(int id, bool activo) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params("UPDATE usuarios SET activo=$1 WHERE id=$2", activo, id);
        txn.commit();
        return true;
    }

    static json getRoles() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec("SELECT id, nombre, descripcion FROM roles ORDER BY id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"nombre",      row["nombre"].c_str()},
                           {"descripcion", row["descripcion"].c_str()}});
        return arr;
    }
    static bool existeEmail(const std::string& email) {
        auto u = findByEmail(email);
        return !u.is_null();
    }
};



