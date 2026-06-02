#pragma once
#include <nlohmann/json.hpp>
#include "Database.h"

using json = nlohmann::json;

class DocumentoRepo {
public:
    static json getCategorias() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT id, nombre, COALESCE(padre_id, -1) AS padre_id "
            "FROM categorias_documento ORDER BY id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",       row["id"].as<int>()},
                           {"nombre",   row["nombre"].c_str()},
                           {"padre_id", row["padre_id"].as<int>()}});
        return arr;
    }

    static int crearCategoria(const std::string& nombre, int padre_id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        std::string sql = padre_id == -1
            ? "INSERT INTO categorias_documento(nombre) VALUES($1) RETURNING id"
            : "INSERT INTO categorias_documento(nombre,padre_id) VALUES($1,$2) RETURNING id";
        pqxx::result r;
        if (padre_id == -1) r = txn.exec_params(sql, nombre);
        else                r = txn.exec_params(sql, nombre, padre_id);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static json getDocumentos(int categoria_id = -1) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        pqxx::result r;
        if (categoria_id == -1)
            r = txn.exec(
                "SELECT d.id, d.nombre, d.descripcion, d.categoria_id, d.created_at, "
                "c.nombre AS categoria, d.archivo_url "
                "FROM documentos d LEFT JOIN categorias_documento c ON d.categoria_id=c.id "
                "ORDER BY d.id DESC");
        else
            r = txn.exec_params(
                "SELECT d.id, d.nombre, d.descripcion, d.categoria_id, d.created_at, "
                "c.nombre AS categoria, d.archivo_url "
                "FROM documentos d LEFT JOIN categorias_documento c ON d.categoria_id=c.id "
                "WHERE d.categoria_id=$1 ORDER BY d.id DESC", categoria_id);
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"nombre",      row["nombre"].c_str()},
                           {"descripcion", row["descripcion"].is_null() ? "" : row["descripcion"].c_str()},
                           {"categoria_id",row["categoria_id"].as<int>()},
                           {"categoria",   row["categoria"].is_null() ? "" : row["categoria"].c_str()},
                           {"archivo_url", row["archivo_url"].is_null() ? "" : row["archivo_url"].c_str()},
                           {"created_at",  row["created_at"].c_str()}});
        return arr;
    }

    static int crearDocumento(const std::string& nombre, const std::string& descripcion,
                               int categoria_id, const std::string& archivo_url, int subido_por) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec_params(
            "INSERT INTO documentos(nombre,descripcion,categoria_id,archivo_url,subido_por) "
            "VALUES($1,$2,$3,$4,$5) RETURNING id",
            nombre, descripcion, categoria_id, archivo_url, subido_por);
        txn.commit();
        return r[0]["id"].as<int>();
    }

    static bool eliminarDocumento(int id) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        txn.exec_params("DELETE FROM documentos WHERE id=$1", id);
        txn.commit();
        return true;
    }
};

class DependenciaRepo {
public:
    static json getAll() {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        auto r = txn.exec(
            "SELECT id, nombre, descripcion, COALESCE(padre_id,-1) AS padre_id, nivel "
            "FROM dependencias ORDER BY nivel, id");
        txn.commit();
        json arr = json::array();
        for (const auto row : r)
            arr.push_back({{"id",          row["id"].as<int>()},
                           {"nombre",      row["nombre"].c_str()},
                           {"descripcion", row["descripcion"].is_null() ? "" : row["descripcion"].c_str()},
                           {"padre_id",    row["padre_id"].as<int>()},
                           {"nivel",       row["nivel"].as<int>()}});
        return arr;
    }

    static int crear(const std::string& nombre, const std::string& descripcion,
                     int padre_id, int nivel) {
        auto _conn_ = DB_CONN(); pqxx::work txn(*_conn_);
        std::string sql = padre_id == -1
            ? "INSERT INTO dependencias(nombre,descripcion,nivel) VALUES($1,$2,$3) RETURNING id"
            : "INSERT INTO dependencias(nombre,descripcion,padre_id,nivel) VALUES($1,$2,$3,$4) RETURNING id";
        pqxx::result r;
        if (padre_id == -1) r = txn.exec_params(sql, nombre, descripcion, nivel);
        else                r = txn.exec_params(sql, nombre, descripcion, padre_id, nivel);
        txn.commit();
        return r[0]["id"].as<int>();
    }
};


