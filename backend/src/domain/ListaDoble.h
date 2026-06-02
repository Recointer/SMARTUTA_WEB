#pragma once
#include <stdexcept>
#include <functional>

// ============================================================
// ListaDoble<T> — Lista doblemente enlazada propia
// Uso: Navegación adelante/atrás en expedientes de trámites
// ============================================================
template<typename T>
class ListaDoble {
public:
    struct Nodo {
        T     dato;
        Nodo* siguiente;
        Nodo* anterior;
        explicit Nodo(const T& d)
            : dato(d), siguiente(nullptr), anterior(nullptr) {}
    };

private:
    Nodo* cabeza;
    Nodo* cola_;
    Nodo* cursor;   // Posición actual del navegador
    int   tamanio;

public:
    ListaDoble() : cabeza(nullptr), cola_(nullptr), cursor(nullptr), tamanio(0) {}

    ~ListaDoble() { limpiar(); }

    void insertarAlFinal(const T& elemento) {
        Nodo* nuevo = new Nodo(elemento);
        if (!cola_) { cabeza = cola_ = nuevo; cursor = nuevo; }
        else {
            nuevo->anterior = cola_;
            cola_->siguiente = nuevo;
            cola_ = nuevo;
        }
        ++tamanio;
    }

    // Navegación bidireccional
    T* siguiente() {
        if (!cursor || !cursor->siguiente) return nullptr;
        cursor = cursor->siguiente;
        return &cursor->dato;
    }

    T* anterior() {
        if (!cursor || !cursor->anterior) return nullptr;
        cursor = cursor->anterior;
        return &cursor->dato;
    }

    T* actual() { return cursor ? &cursor->dato : nullptr; }

    void irAlInicio() { cursor = cabeza; }
    void irAlFinal()  { cursor = cola_; }

    bool eliminar(const T& elemento) {
        Nodo* actual = cabeza;
        while (actual) {
            if (actual->dato == elemento) {
                if (actual->anterior) actual->anterior->siguiente = actual->siguiente;
                else cabeza = actual->siguiente;
                if (actual->siguiente) actual->siguiente->anterior = actual->anterior;
                else cola_ = actual->anterior;
                if (cursor == actual) cursor = actual->siguiente ? actual->siguiente : actual->anterior;
                delete actual;
                --tamanio;
                return true;
            }
            actual = actual->siguiente;
        }
        return false;
    }

    void recorrer(const std::function<void(const T&)>& fn) const {
        Nodo* a = cabeza;
        while (a) { fn(a->dato); a = a->siguiente; }
    }

    void recorrerInverso(const std::function<void(const T&)>& fn) const {
        Nodo* a = cola_;
        while (a) { fn(a->dato); a = a->anterior; }
    }

    void limpiar() {
        Nodo* a = cabeza;
        while (a) { Nodo* tmp = a->siguiente; delete a; a = tmp; }
        cabeza = cola_ = cursor = nullptr;
        tamanio = 0;
    }

    bool  estaVacia() const { return tamanio == 0; }
    int   size()      const { return tamanio; }
    Nodo* getCabeza() const { return cabeza; }
};
