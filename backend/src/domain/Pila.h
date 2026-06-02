#pragma once
#include <stdexcept>

// ============================================================
// Pila<T> — Implementación propia LIFO (sin std::stack)
// Uso: Historial de acciones y funcionalidad de deshacer
// ============================================================
template<typename T>
class Pila {
private:
    struct Nodo {
        T     dato;
        Nodo* anterior;
        Nodo(const T& d, Nodo* ant) : dato(d), anterior(ant) {}
    };
    Nodo* cima;
    int   tamanio;
    int   capacidadMax;

public:
    explicit Pila(int maxCap = 100)
        : cima(nullptr), tamanio(0), capacidadMax(maxCap) {}

    ~Pila() { while (!estaVacia()) desapilar(); }

    Pila(const Pila&) = delete;
    Pila& operator=(const Pila&) = delete;

    void apilar(const T& elemento) {
        if (tamanio >= capacidadMax) {
            // Elimina la base para hacer espacio (ventana deslizante)
            // Recorre hasta el penúltimo
            if (!cima || !cima->anterior) { desapilar(); }
            else {
                Nodo* cur = cima;
                while (cur->anterior && cur->anterior->anterior)
                    cur = cur->anterior;
                delete cur->anterior;
                cur->anterior = nullptr;
                --tamanio;
            }
        }
        cima = new Nodo(elemento, cima);
        ++tamanio;
    }

    T desapilar() {
        if (estaVacia()) throw std::runtime_error("Pila vacía");
        T dato = cima->dato;
        Nodo* tmp = cima;
        cima = cima->anterior;
        delete tmp;
        --tamanio;
        return dato;
    }

    T& verCima() {
        if (estaVacia()) throw std::runtime_error("Pila vacía");
        return cima->dato;
    }

    bool estaVacia() const { return tamanio == 0; }
    int  size()      const { return tamanio; }
};
