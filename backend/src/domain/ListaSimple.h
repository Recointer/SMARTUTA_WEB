#pragma once
#include <stdexcept>
#include <functional>

// ============================================================
// ListaSimple<T> — Lista simplemente enlazada propia
// Uso: Historial dinámico de solicitudes/trámites de un usuario
// ============================================================
template<typename T>
class ListaSimple {
public:
    struct Nodo {
        T     dato;
        Nodo* siguiente;
        explicit Nodo(const T& d) : dato(d), siguiente(nullptr) {}
    };

private:
    Nodo* cabeza;
    Nodo* cola_;
    int   tamanio;

public:
    ListaSimple() : cabeza(nullptr), cola_(nullptr), tamanio(0) {}

    ~ListaSimple() { limpiar(); }

    void insertarAlFinal(const T& elemento) {
        Nodo* nuevo = new Nodo(elemento);
        if (!cola_) { cabeza = cola_ = nuevo; }
        else        { cola_->siguiente = nuevo; cola_ = nuevo; }
        ++tamanio;
    }

    void insertarAlInicio(const T& elemento) {
        Nodo* nuevo = new Nodo(elemento);
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        if (!cola_) cola_ = cabeza;
        ++tamanio;
    }

    bool eliminar(const T& elemento) {
        Nodo* actual = cabeza;
        Nodo* prev   = nullptr;
        while (actual) {
            if (actual->dato == elemento) {
                if (prev) prev->siguiente = actual->siguiente;
                else      cabeza = actual->siguiente;
                if (actual == cola_) cola_ = prev;
                delete actual;
                --tamanio;
                return true;
            }
            prev   = actual;
            actual = actual->siguiente;
        }
        return false;
    }

    Nodo* buscar(const T& elemento) const {
        Nodo* actual = cabeza;
        while (actual) {
            if (actual->dato == elemento) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void recorrer(const std::function<void(const T&)>& fn) const {
        Nodo* actual = cabeza;
        while (actual) { fn(actual->dato); actual = actual->siguiente; }
    }

    void limpiar() {
        Nodo* actual = cabeza;
        while (actual) {
            Nodo* tmp = actual->siguiente;
            delete actual;
            actual = tmp;
        }
        cabeza = cola_ = nullptr;
        tamanio = 0;
    }

    Nodo* getCabeza()  const { return cabeza; }
    bool  estaVacia()  const { return tamanio == 0; }
    int   size()       const { return tamanio; }
};
