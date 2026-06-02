#pragma once
#include <pqxx/pqxx>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <mutex>
#include <memory>

// ============================================================
// Database — Factory de conexiones independientes a PostgreSQL
// Cada llamada a newConn() retorna una NUEVA conexión para
// evitar "transaction while transaction still active" en pqxx 6.x
// ============================================================
class Database {
    std::string connstr_;

    Database() {
        auto e = [](const char* k, const char* def) -> std::string {
            const char* v = std::getenv(k);
            return v ? v : def;
        };
        connstr_ =
            "host="      + e("DB_HOST", "localhost") +
            " port="     + e("DB_PORT", "5432") +
            " dbname="   + e("DB_NAME", "smartcampus") +
            " user="     + e("DB_USER", "smartuser") +
            " password=" + e("DB_PASS", "smartpass123");
    }

public:
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    static Database& instance() {
        static Database inst;
        return inst;
    }

    // Crea una nueva conexión independiente (thread-safe; pqxx::connection es lightweight)
    std::unique_ptr<pqxx::connection> newConn() const {
        return std::make_unique<pqxx::connection>(connstr_);
    }

    // Conexión singleton para servicios de larga vida (TurnoService, CampusService)
    pqxx::connection& conn() {
        static pqxx::connection c(connstr_);
        return c;
    }
};

// ── Macro helper: nueva conexión por scope ──────────────────
// Uso: auto [c, txn] = DB_TXN;  txn.exec(...)  txn.commit();
#define DB_CONN() Database::instance().newConn()
// Alias para backward-compat de los servicios de larga vida
#define DB Database::instance().conn()
