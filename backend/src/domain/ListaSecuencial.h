#pragma once
#include <stdexcept>
#include <functional>

// ============================================================
// ListaSecuencial<T> — Arreglo dinámico propio (sin std::vector)
// Uso: Catálogos de tipos de trámite y edificios
// ============================================================
template<typename T>
class ListaSecuencial {
private:
    T*  datos;
    int tamanio;
    int capacidad;

    void redimensionar() {
        int nuevaCap = capacidad * 2;
        T*  nuevoDatos = new T[nuevaCap];
        for (int i = 0; i < tamanio; ++i)
            nuevoDatos[i] = datos[i];
        delete[] datos;
        datos     = nuevoDatos;
        capacidad = nuevaCap;
    }

public:
    explicit ListaSecuencial(int capInicial = 16)
        : datos(new T[capInicial]), tamanio(0), capacidad(capInicial) {}

    ~ListaSecuencial() { delete[] datos; }

    ListaSecuencial(const ListaSecuencial&) = delete;
    ListaSecuencial& operator=(const ListaSecuencial&) = delete;

    void agregar(const T& elemento) {
        if (tamanio >= capacidad) redimensionar();
        datos[tamanio++] = elemento;
    }

    void insertarEn(int indice, const T& elemento) {
        if (indice < 0 || indice > tamanio)
            throw std::out_of_range("Índice fuera de rango");
        if (tamanio >= capacidad) redimensionar();
        for (int i = tamanio; i > indice; --i)
            datos[i] = datos[i - 1];
        datos[indice] = elemento;
        ++tamanio;
    }

    void eliminarEn(int indice) {
        if (indice < 0 || indice >= tamanio)
            throw std::out_of_range("Índice fuera de rango");
        for (int i = indice; i < tamanio - 1; ++i)
            datos[i] = datos[i + 1];
        --tamanio;
    }

    T& operator[](int indice) {
        if (indice < 0 || indice >= tamanio)
            throw std::out_of_range("Índice fuera de rango");
        return datos[indice];
    }

    const T& operator[](int indice) const {
        if (indice < 0 || indice >= tamanio)
            throw std::out_of_range("Índice fuera de rango");
        return datos[indice];
    }

    int buscar(const std::function<bool(const T&)>& pred) const {
        for (int i = 0; i < tamanio; ++i)
            if (pred(datos[i])) return i;
        return -1;
    }

    void recorrer(const std::function<void(const T&)>& fn) const {
        for (int i = 0; i < tamanio; ++i) fn(datos[i]);
    }

    void limpiar() { tamanio = 0; }

    bool estaVacia() const { return tamanio == 0; }
    int  size()      const { return tamanio; }
};
