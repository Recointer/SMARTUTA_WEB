#pragma once
#include <mutex>
#include <nlohmann/json.hpp>
#include "../domain/Grafo.h"
#include "../domain/Arbol.h"
#include "../domain/ListaSecuencial.h"
#include "../persistence/CampusRepo.h"
#include "../persistence/DocumentoRepo.h"
#include "../persistence/TramiteRepo.h"

using json = nlohmann::json;

// ============================================================
// CampusService — Grafo cargado al inicio del servidor
// ============================================================
class CampusService {
    Grafo grafo;
    std::mutex mtx;

    CampusService() { recargar(); }

public:
    static CampusService& instance() { static CampusService s; return s; }

    void recargar() {
        std::lock_guard<std::mutex> lock(mtx);
        grafo.limpiar();
        auto puntos    = CampusRepo::getPuntos();
        auto conexiones = CampusRepo::getConexiones();
        for (auto& p : puntos)
            grafo.agregarNodo({p["id"].get<int>(), p["nombre"], p["x"], p["y"],
                               p["es_edificio"], p["descripcion"]});
        for (auto& c : conexiones)
            grafo.agregarArista(c["origen_id"], c["destino_id"], c["distancia"]);
    }

    json calcularRuta(int origen, int destino) {
        std::lock_guard<std::mutex> lock(mtx);
        auto camino = grafo.dijkstra(origen, destino);
        if (camino.empty())
            return {{"ok", false}, {"error", "No existe ruta"}};
        double dist = grafo.distanciaTotal(camino);
        json nodos_ruta = json::array();
        for (int id : camino) {
            auto& n = grafo.getNodos().at(id);
            nodos_ruta.push_back({{"id", n.id}, {"nombre", n.nombre},
                                  {"x", n.x}, {"y", n.y}});
        }
        return {{"ok", true}, {"ruta", nodos_ruta},
                {"distancia_total", dist}, {"pasos", (int)camino.size()}};
    }

    json getGrafoJson() {
        std::lock_guard<std::mutex> lock(mtx);
        json res;
        json nodos = json::array();
        for (auto& [id, n] : grafo.getNodos())
            nodos.push_back({{"id", n.id}, {"nombre", n.nombre},
                             {"x", n.x}, {"y", n.y},
                             {"es_edificio", n.esEdificio},
                             {"descripcion", n.descripcion}});
        json aristas = json::array();
        for (auto& [orig, lista] : grafo.getAdyacencia())
            for (auto& a : lista)
                if (orig < a.destino)
                    aristas.push_back({{"origen", orig}, {"destino", a.destino},
                                       {"peso", a.peso}});
        res["nodos"]   = nodos;
        res["aristas"] = aristas;
        return res;
    }
};

// ============================================================
// DocumentoService — Árbol N-ario construido on-demand
// ============================================================
class DocumentoService {
public:
    static json getCategoriaArbol() {
        auto cats = DocumentoRepo::getCategorias();
        // Construir Árbol N-ario desde los datos
        Arbol<std::string> arbol;
        for (auto& c : cats)
            arbol.insertar(c["id"], c["padre_id"], c["nombre"]);

        // Serializar recursivamente
        std::function<json(Arbol<std::string>::Nodo*)> serial;
        serial = [&](Arbol<std::string>::Nodo* n) -> json {
            if (!n) return nullptr;
            json hijos = json::array();
            for (auto* h : n->hijos) hijos.push_back(serial(h));
            return {{"id", n->id}, {"nombre", n->dato}, {"hijos", hijos}};
        };
        return serial(arbol.getRaiz());
    }

    static json getDependenciaArbol() {
        auto deps = DependenciaRepo::getAll();
        Arbol<std::string> arbol;
        for (auto& d : deps)
            arbol.insertar(d["id"], d["padre_id"], d["nombre"].get<std::string>()
                           + "|" + d["descripcion"].get<std::string>());

        std::function<json(Arbol<std::string>::Nodo*)> serial;
        serial = [&](Arbol<std::string>::Nodo* n) -> json {
            if (!n) return nullptr;
            // Split nombre|descripcion
            auto sep = n->dato.find('|');
            std::string nom = n->dato.substr(0, sep);
            std::string desc = sep != std::string::npos ? n->dato.substr(sep+1) : "";
            json hijos = json::array();
            for (auto* h : n->hijos) hijos.push_back(serial(h));
            return {{"id", n->id}, {"nombre", nom}, {"descripcion", desc}, {"hijos", hijos}};
        };
        return serial(arbol.getRaiz());
    }
};

// ============================================================
// CatalogoService — ListaSecuencial para tipos de trámite
// ============================================================
class CatalogoService {
public:
    static json getTiposTramite() {
        auto tipos = TramiteRepo::getTipos();
        ListaSecuencial<json> lista;
        for (auto& t : tipos) lista.agregar(t);
        json arr = json::array();
        lista.recorrer([&](const json& t) { arr.push_back(t); });
        return arr;
    }
};
