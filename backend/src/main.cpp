#define CROW_MAIN
#include <crow_all.h>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdlib>

#include "utils/JwtHelper.h"
#include "persistence/Database.h"
#include "persistence/UsuarioRepo.h"
#include "persistence/TurnoRepo.h"
#include "persistence/TramiteRepo.h"
#include "persistence/DocumentoRepo.h"
#include "persistence/CampusRepo.h"
#include "application/AuthService.h"
#include "application/TurnoService.h"
#include "application/HistorialService.h"
#include "application/CampusService.h"

using json = nlohmann::json;

// ── Helpers ──────────────────────────────────────────────────
crow::response jsonResp(const json& j, int status = 200) {
    crow::response res(status, j.dump());
    res.set_header("Content-Type", "application/json");
    res.set_header("Access-Control-Allow-Origin",  "*");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    return res;
}

bool verifyJwt(const crow::request& req, int& uid, std::string& rol, std::string& nombre) {
    auto auth = req.get_header_value("Authorization");
    std::string token = JwtHelper::extractToken(auth);
    return JwtHelper::verifyToken(token, uid, rol, nombre);
}

json parseBody(const crow::request& req) {
    try { return json::parse(req.body); }
    catch (...) { return json::object(); }
}

// ── CORS preflight ───────────────────────────────────────────
void addCors(crow::App<>& app) {
    CROW_ROUTE(app, "/api/<path>").methods("OPTIONS"_method)
    ([](const crow::request&, std::string) {
        crow::response res(204);
        res.set_header("Access-Control-Allow-Origin",  "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        return res;
    });
}

int main() {
    crow::App<> app;

    // Inicializar singletons
    Database::instance();
    TurnoService::instance();
    CampusService::instance();
    HistorialService::instance();

    // ── CORS ──────────────────────────────────────────────────
    addCors(app);

    // ═══════════════════════════════════════════════════════════
    // AUTH
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/auth/login").methods("POST"_method)
    ([](const crow::request& req) {
        auto body  = parseBody(req);
        auto email = body.value("email", "");
        auto pass  = body.value("password", "");
        if (email.empty() || pass.empty())
            return jsonResp({{"error", "Email y contraseña requeridos"}}, 400);
        auto res = AuthService::login(email, pass);
        if (!res["ok"].get<bool>()) return jsonResp(res, 401);
        return jsonResp(res);
    });

    CROW_ROUTE(app, "/api/auth/me").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto u = UsuarioRepo::findById(uid);
        if (u.is_null()) return jsonResp({{"error","Usuario no encontrado"}}, 404);
        return jsonResp(u);
    });

    CROW_ROUTE(app, "/api/auth/register").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","Solo admin puede registrar usuarios"}}, 403);
        auto body = parseBody(req);
        auto res  = AuthService::registrar(
            body.value("nombre",""), body.value("email",""),
            body.value("password",""), body.value("rol_id",3));
        return jsonResp(res, res["ok"] ? 201 : 400);
    });

    CROW_ROUTE(app, "/api/auth/roles").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(UsuarioRepo::getRoles());
    });

    // ═══════════════════════════════════════════════════════════
    // USUARIOS (admin)
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/usuarios").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        return jsonResp(UsuarioRepo::findAll());
    });

    CROW_ROUTE(app, "/api/usuarios/<int>/activo").methods("PUT"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        auto body = parseBody(req);
        UsuarioRepo::actualizarActivo(id, body.value("activo", true));
        return jsonResp({{"ok", true}});
    });

    // ═══════════════════════════════════════════════════════════
    // TURNOS
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/turnos").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(TurnoRepo::findAll());
    });

    CROW_ROUTE(app, "/api/turnos").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto body = parseBody(req);
        auto tipo = body.value("tipo_tramite","General");
        auto res  = TurnoService::instance().registrarTurno(uid, tipo);
        if (res["ok"].get<bool>())
            HistorialService::instance().registrar(uid, "NUEVO_TURNO",
                "Turno registrado: " + tipo);
        return jsonResp(res, res["ok"] ? 201 : 400);
    });

    CROW_ROUTE(app, "/api/turnos/estado").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(TurnoService::instance().estadoCola());
    });

    CROW_ROUTE(app, "/api/turnos/siguiente").methods("PUT"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || (rol != "admin" && rol != "secretaria"))
            return jsonResp({{"error","No autorizado"}}, 403);
        auto res = TurnoService::instance().atenderSiguiente();
        if (res["ok"].get<bool>())
            HistorialService::instance().registrar(uid, "TURNO_ATENDIDO",
                "Turno #" + std::to_string(res["numero"].get<int>()) + " atendido");
        return jsonResp(res);
    });

    CROW_ROUTE(app, "/api/turnos/pendientes").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(TurnoRepo::findPendientes());
    });

    // ═══════════════════════════════════════════════════════════
    // TRÁMITES
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/tramites").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(TramiteRepo::findAll());
    });

    CROW_ROUTE(app, "/api/tramites").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto body = parseBody(req);
        int id = TramiteRepo::crear(
            body.value("estudiante_id", 0),
            body.value("tipo_tramite_id", 1),
            body.value("descripcion", ""));
        HistorialService::instance().registrar(uid, "NUEVO_TRAMITE",
            "Trámite creado ID: " + std::to_string(id));
        return jsonResp({{"ok", true}, {"id", id}}, 201);
    });

    CROW_ROUTE(app, "/api/tramites/<int>").methods("GET"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto t = TramiteRepo::findById(id);
        if (t.is_null()) return jsonResp({{"error","No encontrado"}}, 404);
        return jsonResp(t);
    });

    CROW_ROUTE(app, "/api/tramites/<int>/estado").methods("PUT"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto body   = parseBody(req);
        auto estado = body.value("estado", "pendiente");
        TramiteRepo::actualizarEstado(id, estado);
        HistorialService::instance().registrar(uid, "ACTUALIZAR_TRAMITE",
            "Trámite " + std::to_string(id) + " → " + estado);
        return jsonResp({{"ok", true}});
    });

    CROW_ROUTE(app, "/api/tramites/tipos").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(TramiteRepo::getTipos());
    });

    CROW_ROUTE(app, "/api/tramites/estudiantes").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(TramiteRepo::getEstudiantes());
    });

    // ═══════════════════════════════════════════════════════════
    // HISTORIAL (Pila LIFO)
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/historial").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(HistorialRepo::findByUsuario(uid, 20));
    });

    CROW_ROUTE(app, "/api/historial/todos").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        return jsonResp(HistorialRepo::findAll(50));
    });

    CROW_ROUTE(app, "/api/historial/cima").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(HistorialService::instance().getCima(uid));
    });

    CROW_ROUTE(app, "/api/historial/deshacer").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(HistorialService::instance().deshacer(uid));
    });

    // ═══════════════════════════════════════════════════════════
    // DOCUMENTOS (Árbol N-ario)
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/documentos/arbol").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(DocumentoService::getCategoriaArbol());
    });

    CROW_ROUTE(app, "/api/documentos/categorias").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(DocumentoRepo::getCategorias());
    });

    CROW_ROUTE(app, "/api/documentos/categorias").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        auto body = parseBody(req);
        int id = DocumentoRepo::crearCategoria(
            body.value("nombre","Nueva categoría"),
            body.value("padre_id",-1));
        return jsonResp({{"ok",true},{"id",id}}, 201);
    });

    CROW_ROUTE(app, "/api/documentos").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(DocumentoRepo::getDocumentos());
    });

    CROW_ROUTE(app, "/api/documentos").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto body = parseBody(req);
        int id = DocumentoRepo::crearDocumento(
            body.value("nombre",""), body.value("descripcion",""),
            body.value("categoria_id",1), body.value("archivo_url",""), uid);
        HistorialService::instance().registrar(uid,"SUBIR_DOCUMENTO",
            "Documento subido: " + body.value("nombre",""));
        return jsonResp({{"ok",true},{"id",id}}, 201);
    });

    CROW_ROUTE(app, "/api/documentos/<int>").methods("DELETE"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        DocumentoRepo::eliminarDocumento(id);
        return jsonResp({{"ok",true}});
    });

    // ═══════════════════════════════════════════════════════════
    // DEPENDENCIAS (Árbol N-ario)
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/dependencias/arbol").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(DocumentoService::getDependenciaArbol());
    });

    CROW_ROUTE(app, "/api/dependencias").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(DependenciaRepo::getAll());
    });

    CROW_ROUTE(app, "/api/dependencias").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        auto body = parseBody(req);
        int id = DependenciaRepo::crear(
            body.value("nombre",""), body.value("descripcion",""),
            body.value("padre_id",-1), body.value("nivel",0));
        return jsonResp({{"ok",true},{"id",id}}, 201);
    });

    // ═══════════════════════════════════════════════════════════
    // CAMPUS / RUTAS (Grafo + Dijkstra)
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/campus/grafo").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(CampusService::instance().getGrafoJson());
    });

    CROW_ROUTE(app, "/api/campus/puntos").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(CampusRepo::getPuntos());
    });

    CROW_ROUTE(app, "/api/campus/puntos").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        auto body = parseBody(req);
        int id = CampusRepo::crearPunto(
            body.value("nombre",""), body.value("tipo","waypoint"),
            body.value("x",0.0), body.value("y",0.0),
            body.value("es_edificio",false), body.value("descripcion",""));
        CampusService::instance().recargar();
        return jsonResp({{"ok",true},{"id",id}}, 201);
    });

    CROW_ROUTE(app, "/api/campus/conexiones").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        auto body = parseBody(req);
        int id = CampusRepo::crearConexion(
            body.value("origen_id",0), body.value("destino_id",0),
            body.value("distancia",0.0), body.value("tipo_camino","peatonal"));
        CampusService::instance().recargar();
        return jsonResp({{"ok",true},{"id",id}}, 201);
    });

    CROW_ROUTE(app, "/api/campus/puntos/<int>").methods("PUT"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        auto body = parseBody(req);
        CampusRepo::updatePunto(id,
            body.value("nombre",""), body.value("tipo","waypoint"),
            body.value("x",0.0),    body.value("y",0.0),
            body.value("es_edificio",false), body.value("descripcion",""));
        CampusService::instance().recargar();
        return jsonResp({{"ok",true}});
    });

    // Primero consultar conexiones del nodo
    CROW_ROUTE(app, "/api/campus/puntos/<int>/conexiones").methods("GET"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        int n = CampusRepo::contarConexiones(id);
        return jsonResp({{"count", n}});
    });

    CROW_ROUTE(app, "/api/campus/puntos/<int>").methods("DELETE"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        CampusRepo::deletePunto(id);
        CampusService::instance().recargar();
        HistorialService::instance().registrar(uid, "BORRAR_PUNTO_CAMPUS",
            "Punto campus ID " + std::to_string(id) + " eliminado");
        return jsonResp({{"ok",true}});
    });

    CROW_ROUTE(app, "/api/campus/conexiones").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(CampusRepo::getConexiones());
    });

    CROW_ROUTE(app, "/api/campus/conexiones/<int>").methods("DELETE"_method)
    ([](const crow::request& req, int id) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre) || rol != "admin")
            return jsonResp({{"error","No autorizado"}}, 403);
        CampusRepo::deleteConexion(id);
        CampusService::instance().recargar();
        return jsonResp({{"ok",true}});
    });

    CROW_ROUTE(app, "/api/campus/ruta").methods("POST"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        auto body = parseBody(req);
        int origen  = body.value("origen_id",  0);
        int destino = body.value("destino_id", 0);
        if (!origen || !destino) return jsonResp({{"error","origen_id y destino_id requeridos"}}, 400);
        return jsonResp(CampusService::instance().calcularRuta(origen, destino));
    });

    // ═══════════════════════════════════════════════════════════
    // REPORTES
    // ═══════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/reportes/generales").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(ReporteRepo::statsGenerales());
    });

    CROW_ROUTE(app, "/api/reportes/turnos").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(ReporteRepo::statsTurnos());
    });

    CROW_ROUTE(app, "/api/reportes/tramites").methods("GET"_method)
    ([](const crow::request& req) {
        int uid; std::string rol, nombre;
        if (!verifyJwt(req, uid, rol, nombre)) return jsonResp({{"error","No autorizado"}}, 401);
        return jsonResp(ReporteRepo::statsTramites());
    });

    // ── Start server ─────────────────────────────────────────
    int port = 8080;
    if (const char* p = std::getenv("PORT")) port = std::stoi(p);
    CROW_LOG_INFO << "SmartCampus UTA API iniciando en puerto " << port;
    app.port(port).multithreaded().run();
    return 0;
}
