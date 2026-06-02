#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>
#include <stdexcept>

// ============================================================
// Grafo — Lista de adyacencia + Dijkstra propio
// Uso: Mapa del campus UTA y búsqueda de rutas peatonales
// ============================================================

struct NodoCampus {
    int         id;
    std::string nombre;
    double      x, y;
    bool        esEdificio;
    std::string descripcion;
};

struct Arista {
    int    destino;
    double peso;
};

// MinHeap propio para Dijkstra (sin std::priority_queue)
struct EntradaHeap { double dist; int nodo; };

class MinHeap {
    std::vector<EntradaHeap> datos;
    void subir(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (datos[p].dist <= datos[i].dist) break;
            std::swap(datos[p], datos[i]);
            i = p;
        }
    }
    void bajar(int i) {
        int n = datos.size();
        while (true) {
            int menor = i, l = 2*i+1, r = 2*i+2;
            if (l < n && datos[l].dist < datos[menor].dist) menor = l;
            if (r < n && datos[r].dist < datos[menor].dist) menor = r;
            if (menor == i) break;
            std::swap(datos[menor], datos[i]);
            i = menor;
        }
    }
public:
    void insertar(EntradaHeap e) { datos.push_back(e); subir(datos.size()-1); }
    EntradaHeap extraerMin() {
        EntradaHeap min = datos[0];
        datos[0] = datos.back(); datos.pop_back();
        if (!datos.empty()) bajar(0);
        return min;
    }
    bool estaVacio() const { return datos.empty(); }
};

// ────────────────────────────────────────────
class Grafo {
private:
    std::unordered_map<int, NodoCampus>          nodos;
    std::unordered_map<int, std::vector<Arista>> adyacencia;

public:
    void agregarNodo(const NodoCampus& nc) {
        nodos[nc.id]    = nc;
        if (!adyacencia.count(nc.id)) adyacencia[nc.id] = {};
    }

    void agregarArista(int origen, int destino, double peso) {
        adyacencia[origen].push_back({destino, peso});
        adyacencia[destino].push_back({origen,  peso}); // No dirigido
    }

    // Dijkstra — retorna lista de IDs del camino más corto
    std::vector<int> dijkstra(int origen, int destino) const {
        const double INF = std::numeric_limits<double>::infinity();
        std::unordered_map<int, double> dist;
        std::unordered_map<int, int>    prev;

        for (auto& [id, _] : nodos) { dist[id] = INF; prev[id] = -1; }
        dist[origen] = 0.0;

        MinHeap heap;
        heap.insertar({0.0, origen});

        while (!heap.estaVacio()) {
            auto [d, u] = heap.extraerMin();
            if (d > dist[u]) continue;
            if (u == destino)  break;
            for (const auto& a : adyacencia.at(u)) {
                double alt = dist[u] + a.peso;
                if (alt < dist[a.destino]) {
                    dist[a.destino] = alt;
                    prev[a.destino] = u;
                    heap.insertar({alt, a.destino});
                }
            }
        }

        std::vector<int> camino;
        if (dist[destino] == INF) return camino; // sin ruta
        for (int v = destino; v != -1; v = prev[v]) camino.push_back(v);
        std::reverse(camino.begin(), camino.end());
        return camino;
    }

    double distanciaTotal(const std::vector<int>& camino) const {
        double total = 0.0;
        for (size_t i = 1; i < camino.size(); ++i) {
            for (const auto& a : adyacencia.at(camino[i-1])) {
                if (a.destino == camino[i]) { total += a.peso; break; }
            }
        }
        return total;
    }

    const std::unordered_map<int, NodoCampus>&          getNodos()      const { return nodos; }
    const std::unordered_map<int, std::vector<Arista>>& getAdyacencia() const { return adyacencia; }
    bool tieneNodo(int id) const { return nodos.count(id) > 0; }
    void limpiar() { nodos.clear(); adyacencia.clear(); }
};
