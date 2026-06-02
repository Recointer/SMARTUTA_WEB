#pragma once
#include <stdexcept>

// ============================================================
// Cola<T> — Implementación propia FIFO (sin std::queue)
// Uso: Cola de turnos de atención estudiantil
// ============================================================
template<typename T>
class Cola {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        explicit Nodo(const T& d) : dato(d), siguiente(nullptr) {}
    };
    Nodo* frente;
    Nodo* fondo;
    int   tamanio;

public:
    Cola() : frente(nullptr), fondo(nullptr), tamanio(0) {}
    ~Cola() { while (!estaVacia()) desencolar(); }

    Cola(const Cola&) = delete;
    Cola& operator=(const Cola&) = delete;

    void encolar(const T& elemento) {
        Nodo* nuevo = new Nodo(elemento);
        if (!fondo) { frente = fondo = nuevo; }
        else        { fondo->siguiente = nuevo; fondo = nuevo; }
        ++tamanio;
    }

    T desencolar() {
        if (estaVacia()) throw std::runtime_error("Cola vacía");
        T dato = frente->dato;
        Nodo* tmp = frente;
        frente = frente->siguiente;
        if (!frente) fondo = nullptr;
        delete tmp;
        --tamanio;
        return dato;
    }

    T& verFrente() {
        if (estaVacia()) throw std::runtime_error("Cola vacía");
        return frente->dato;
    }

    bool estaVacia()  const { return tamanio == 0; }
    int  size()       const { return tamanio; }
};
