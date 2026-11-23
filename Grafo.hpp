#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <iostream>
#include <string>
#include "Clientes.hpp"

// Declaracion adelantada
struct Vertice;

// Arco (Linea de Transmision) en la multilista dinamica
struct Arco {
    Vertice* origen;
    Vertice* destino;
    std::string medio;          // "alta_tension", "subterraneo", "red_local"
    double distanciaKm;
    double capacidadMw;
    double perdidaPorcentaje;
    int usoConteo;              // Cantidad de veces utilizada en la red
    Arco* siguienteArco;        // Siguiente arco saliente del mismo vertice origen

    Arco(Vertice* orig, Vertice* dest, const std::string& _medio,
         double _dist, double _cap, double _perdida, int _uso = 1)
        : origen(orig), destino(dest), medio(_medio),
          distanciaKm(_dist), capacidadMw(_cap), perdidaPorcentaje(_perdida),
          usoConteo(_uso), siguienteArco(nullptr) {}
};

// Vertice (Nodo de Generacion/Distribucion) en la multilista dinamica
struct Vertice {
    std::string region;
    std::string nombre;
    std::string tipo;           // "generador", "subestacion", "subestacion_local"
    Arco* listaArcos;           // Puntero al primer arco saliente (Multilista)
    Vertice* siguienteVertice;  // Puntero al siguiente vertice en la lista principal

    Vertice(const std::string& _region, const std::string& _nombre, const std::string& _tipo)
        : region(_region), nombre(_nombre), tipo(_tipo),
          listaArcos(nullptr), siguienteVertice(nullptr) {}
};

class GrafoRedElectrica {
private:
    Vertice* primerVertice;
    int totalVertices;
    int totalArcos;

    // Auxiliares recursivos
    void dfsRecursivo(Vertice* actual, bool* visitados, Vertice** mapaVertices, int n) const;
    void buscarTodasRutasAux(Vertice* actual, Vertice* destino,
                             const std::string& medio1, const std::string& medio2,
                             bool* visitados, Vertice** mapaVertices, int n,
                             Arco** caminoActual, int profundidad, int& rutasEncontradas) const;
    void buscarMaxTokensAux(Vertice* actual, Vertice* destino, bool horaNoPico,
                            bool* visitados, Vertice** mapaVertices, int n,
                            Arco** caminoActual, int profundidad,
                            int tokensAcumulados, int& mejorPuntaje,
                            Arco** mejorCamino, int& mejorLongitud) const;

    int obtenerIndiceVertice(Vertice* v, Vertice** mapaVertices, int n) const;
    Vertice** construirArregloVertices(int& n) const;
    int calcularTokensArco(const Arco* arco, bool horaNoPico) const;

public:
    GrafoRedElectrica();
    ~GrafoRedElectrica();

    Vertice* getPrimerVertice() const { return primerVertice; }
    int getTotalVertices() const { return totalVertices; }
    int getTotalArcos() const { return totalArcos; }

    // A.1 CRUD Vertices y Arcos
    Vertice* buscarVertice(const std::string& nombre) const;
    bool agregarVertice(const std::string& region, const std::string& nombre, const std::string& tipo);
    bool modificarVertice(const std::string& nombre, const std::string& nuevaRegion, const std::string& nuevoTipo);
    bool eliminarVertice(const std::string& nombre);

    Arco* buscarArco(const std::string& origen, const std::string& destino, const std::string& medio = "") const;
    bool agregarArco(const std::string& origen, const std::string& destino,
                     const std::string& medio, double distanciaKm,
                     double capacidadMw, double perdidaPorcentaje, int usoConteo = 1);
    bool modificarArco(const std::string& origen, const std::string& destino, const std::string& medioActual,
                       const std::string& nuevoMedio, double nuevaDist, double nuevaCap, double nuevaPerdida);
    bool eliminarArco(const std::string& origen, const std::string& destino, const std::string& medio = "");

    void limpiar();
    void mostrarMultilistaCompleta() const;

    // B.1 & B.2 Recorridos
    void recorridoBFS(const std::string& nombreInicio) const;
    void recorridoDFS(const std::string& nombreInicio) const;

    // B.5 Nodos Inactivos
    void reportarNodosInactivos(const ListaClientes& listaClientes) const;

    // C.1 Todas las rutas posibles filtradas por 1 o 2 medios
    void consultarTodasLasRutas(const std::string& origen, const std::string& destino,
                                const std::string& medio1, const std::string& medio2 = "") const;

    // C.2 Ruta con menor perdida de energia (Dijkstra)
    void consultarRutaMenorPerdida(const std::string& origen, const std::string& destino) const;

    // C.3 Ruta de mayor capacidad / Eco-Tokens con actualizacion de cliente
    void consultarRutaMayorTokens(const std::string& nombreCliente, const std::string& origen,
                                  const std::string& destino, bool horaNoPico,
                                  ListaClientes& listaClientes);

    // C.5 Ruta frecuente y ruta unica
    void reportarRutaFrecuenteYUnica() const;
};

#endif // GRAFO_HPP
