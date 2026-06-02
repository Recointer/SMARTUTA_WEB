#pragma once
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

// ============================================================
// Arbol<T> — Árbol N-ario general propio
// Uso: Jerarquía documental y dependencias institucionales
// ============================================================
template<typename T>
class Arbol {
public:
    struct Nodo {
        T                  dato;
        int                id;
        int                padre_id;
        std::vector<Nodo*> hijos;   // N-ario: cada nodo tiene N hijos

        Nodo(int id_, int padre_id_, const T& d)
            : dato(d), id(id_), padre_id(padre_id_) {}

        ~Nodo() {
            for (auto* h : hijos) delete h;
        }
    };

private:
    Nodo* raiz;

    Nodo* buscarNodo(Nodo* nodo, int id) const {
        if (!nodo) return nullptr;
        if (nodo->id == id) return nodo;
        for (auto* h : nodo->hijos) {
            Nodo* r = buscarNodo(h, id);
            if (r) return r;
        }
        return nullptr;
    }

    void preorden(Nodo* nodo, const std::function<void(Nodo*)>& fn) const {
        if (!nodo) return;
        fn(nodo);
        for (auto* h : nodo->hijos) preorden(h, fn);
    }

public:
    Arbol() : raiz(nullptr) {}
    ~Arbol() { delete raiz; }

    // Inserta un nodo. padre_id = -1 → es raíz
    bool insertar(int id, int padre_id, const T& dato) {
        Nodo* nuevo = new Nodo(id, padre_id, dato);
        if (padre_id == -1) {
            if (raiz) { delete nuevo; return false; } // ya hay raíz
            raiz = nuevo;
            return true;
        }
        Nodo* padre = buscarNodo(raiz, padre_id);
        if (!padre) { delete nuevo; return false; }
        padre->hijos.push_back(nuevo);
        return true;
    }

    Nodo* buscar(int id) const { return buscarNodo(raiz, id); }

    void recorrerPreorden(const std::function<void(Nodo*)>& fn) const {
        preorden(raiz, fn);
    }

    bool eliminar(int id) {
        if (!raiz) return false;
        if (raiz->id == id) { delete raiz; raiz = nullptr; return true; }
        // Busca el padre del nodo a eliminar
        bool eliminado = false;
        preorden(raiz, [&](Nodo* nodo) {
            if (eliminado) return;
            auto& hijos = nodo->hijos;
            for (auto it = hijos.begin(); it != hijos.end(); ++it) {
                if ((*it)->id == id) {
                    delete *it;
                    hijos.erase(it);
                    eliminado = true;
                    return;
                }
            }
        });
        return eliminado;
    }

    Nodo* getRaiz()   const { return raiz; }
    bool  estaVacio() const { return raiz == nullptr; }
};
