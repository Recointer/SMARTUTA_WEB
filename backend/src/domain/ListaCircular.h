#pragma once
#include <stdexcept>
#include <functional>

// ============================================================
// ListaCircular<T> — Lista circular doblemente enlazada propia
// Uso: Rotación Round-Robin de ventanillas de atención
// ============================================================
template<typename T>
class ListaCircular {
public:
    struct Nodo {
        T     dato;
        Nodo* siguiente;
        Nodo* anterior;
        explicit Nodo(const T& d)
            : dato(d), siguiente(nullptr), anterior(nullptr) {}
    };

private:
    Nodo* actual_;   // Puntero al nodo activo en la rotación
    int   tamanio;

public:
    ListaCircular() : actual_(nullptr), tamanio(0) {}

    ~ListaCircular() { limpiar(); }

    void insertar(const T& elemento) {
        Nodo* nuevo = new Nodo(elemento);
        if (!actual_) {
            actual_ = nuevo;
            nuevo->siguiente = nuevo;
            nuevo->anterior  = nuevo;
        } else {
            Nodo* ultimo = actual_->anterior;
            ultimo->siguiente  = nuevo;
            nuevo->anterior    = ultimo;
            nuevo->siguiente   = actual_;
            actual_->anterior  = nuevo;
        }
        ++tamanio;
    }

    // Avanza al siguiente — Round-Robin
    T& siguiente() {
        if (!actual_) throw std::runtime_error("Lista circular vacía");
        actual_ = actual_->siguiente;
        return actual_->dato;
    }

    T& actual() {
        if (!actual_) throw std::runtime_error("Lista circular vacía");
        return actual_->dato;
    }

    bool eliminar(const T& elemento) {
        if (!actual_) return false;
        Nodo* inicio = actual_;
        Nodo* cur    = actual_;
        do {
            if (cur->dato == elemento) {
                if (tamanio == 1) { delete cur; actual_ = nullptr; tamanio = 0; return true; }
                cur->anterior->siguiente = cur->siguiente;
                cur->siguiente->anterior = cur->anterior;
                if (cur == actual_) actual_ = cur->siguiente;
                delete cur;
                --tamanio;
                return true;
            }
            cur = cur->siguiente;
        } while (cur != inicio);
        return false;
    }

    void recorrer(const std::function<void(const T&)>& fn) const {
        if (!actual_) return;
        Nodo* inicio = actual_;
        Nodo* cur    = actual_;
        do { fn(cur->dato); cur = cur->siguiente; } while (cur != inicio);
    }

    void limpiar() {
        if (!actual_) return;
        Nodo* inicio = actual_;
        Nodo* cur    = actual_->siguiente;
        delete actual_;
        while (cur != inicio) {
            Nodo* tmp = cur->siguiente;
            delete cur;
            cur = tmp;
        }
        actual_ = nullptr;
        tamanio = 0;
    }

    bool estaVacia() const { return tamanio == 0; }
    int  size()      const { return tamanio; }
};
