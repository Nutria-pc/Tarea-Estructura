#include "../include/Grafo.hpp"
#include <iomanip>
#include <limits>
#include <cmath>

GrafoRedElectrica::GrafoRedElectrica()
    : primerVertice(nullptr), totalVertices(0), totalArcos(0) {}

GrafoRedElectrica::~GrafoRedElectrica() {
    limpiar();
}

void GrafoRedElectrica::limpiar() {
    Vertice* vActual = primerVertice;
    while (vActual != nullptr) {
        Arco* aActual = vActual->listaArcos;
        while (aActual != nullptr) {
            Arco* aSig = aActual->siguienteArco;
            delete aActual;
            aActual = aSig;
        }
        Vertice* vSig = vActual->siguienteVertice;
        delete vActual;
        vActual = vSig;
    }
    primerVertice = nullptr;
    totalVertices = 0;
    totalArcos = 0;
}

Vertice* GrafoRedElectrica::buscarVertice(const std::string& nombre) const {
    Vertice* aux = primerVertice;
    while (aux != nullptr) {
        if (aux->nombre == nombre) return aux;
        aux = aux->siguienteVertice;
    }
    return nullptr;
}

bool GrafoRedElectrica::agregarVertice(const std::string& region, const std::string& nombre, const std::string& tipo) {
    if (region.empty() || nombre.empty() || tipo.empty()) return false;
    if (buscarVertice(nombre) != nullptr) return false;

    Vertice* nuevo = new Vertice(region, nombre, tipo);
    if (primerVertice == nullptr) {
        primerVertice = nuevo;
    } else {
        Vertice* aux = primerVertice;
        while (aux->siguienteVertice != nullptr) {
            aux = aux->siguienteVertice;
        }
        aux->siguienteVertice = nuevo;
    }
    ++totalVertices;
    return true;
}

bool GrafoRedElectrica::modificarVertice(const std::string& nombre, const std::string& nuevaRegion, const std::string& nuevoTipo) {
    Vertice* v = buscarVertice(nombre);
    if (v == nullptr) return false;
    if (!nuevaRegion.empty()) v->region = nuevaRegion;
    if (!nuevoTipo.empty()) v->tipo = nuevoTipo;
    return true;
}

bool GrafoRedElectrica::eliminarVertice(const std::string& nombre) {
    Vertice* objetivo = buscarVertice(nombre);
    if (objetivo == nullptr) return false;

    // 1. Eliminar todos los arcos entrantes desde otros vertices hacia 'objetivo'
    Vertice* v = primerVertice;
    while (v != nullptr) {
        if (v != objetivo) {
            Arco* ant = nullptr;
            Arco* act = v->listaArcos;
            while (act != nullptr) {
                if (act->destino == objetivo) {
                    Arco* borrar = act;
                    if (ant == nullptr) {
                        v->listaArcos = act->siguienteArco;
                        act = v->listaArcos;
                    } else {
                        ant->siguienteArco = act->siguienteArco;
                        act = ant->siguienteArco;
                    }
                    delete borrar;
                    --totalArcos;
                } else {
                    ant = act;
                    act = act->siguienteArco;
                }
            }
        }
        v = v->siguienteVertice;
    }

    // 2. Eliminar todos los arcos salientes del vertice objetivo
    Arco* aOut = objetivo->listaArcos;
    while (aOut != nullptr) {
        Arco* sig = aOut->siguienteArco;
        delete aOut;
        --totalArcos;
        aOut = sig;
    }
    objetivo->listaArcos = nullptr;

    // 3. Desenlazar y liberar el vertice objetivo
    if (primerVertice == objetivo) {
        primerVertice = objetivo->siguienteVertice;
    } else {
        Vertice* prev = primerVertice;
        while (prev != nullptr && prev->siguienteVertice != objetivo) {
            prev = prev->siguienteVertice;
        }
        if (prev != nullptr) {
            prev->siguienteVertice = objetivo->siguienteVertice;
        }
    }
    delete objetivo;
    --totalVertices;
    return true;
}

Arco* GrafoRedElectrica::buscarArco(const std::string& origen, const std::string& destino, const std::string& medio) const {
    Vertice* vOrig = buscarVertice(origen);
    if (vOrig == nullptr) return nullptr;
    Arco* a = vOrig->listaArcos;
    while (a != nullptr) {
        if (a->destino->nombre == destino && (medio.empty() || a->medio == medio)) {
            return a;
        }
        a = a->siguienteArco;
    }
    return nullptr;
}

bool GrafoRedElectrica::agregarArco(const std::string& origen, const std::string& destino,
                                  const std::string& medio, double distanciaKm,
                                  double capacidadMw, double perdidaPorcentaje, int usoConteo) {
    if (origen == destino) return false;
    if (distanciaKm <= 0.0 || capacidadMw <= 0.0 || perdidaPorcentaje < 0.0 || perdidaPorcentaje >= 100.0) {
        return false;
    }
    if (medio != "alta_tension" && medio != "subterraneo" && medio != "red_local") {
        return false;
    }

    Vertice* vOrig = buscarVertice(origen);
    Vertice* vDest = buscarVertice(destino);
    if (vOrig == nullptr || vDest == nullptr) return false;

    // Evitar arcos duplicados con mismo origen, destino y medio
    if (buscarArco(origen, destino, medio) != nullptr) return false;

    Arco* nuevo = new Arco(vOrig, vDest, medio, distanciaKm, capacidadMw, perdidaPorcentaje, usoConteo < 0 ? 0 : usoConteo);
    if (vOrig->listaArcos == nullptr) {
        vOrig->listaArcos = nuevo;
    } else {
        Arco* aux = vOrig->listaArcos;
        while (aux->siguienteArco != nullptr) {
            aux = aux->siguienteArco;
        }
        aux->siguienteArco = nuevo;
    }
    ++totalArcos;
    return true;
}

bool GrafoRedElectrica::modificarArco(const std::string& origen, const std::string& destino, const std::string& medioActual,
                                    const std::string& nuevoMedio, double nuevaDist, double nuevaCap, double nuevaPerdida) {
    Arco* a = buscarArco(origen, destino, medioActual);
    if (a == nullptr) return false;
    if (nuevaDist <= 0.0 || nuevaCap <= 0.0 || nuevaPerdida < 0.0 || nuevaPerdida >= 100.0) return false;
    if (nuevoMedio != "alta_tension" && nuevoMedio != "subterraneo" && nuevoMedio != "red_local") return false;

    a->medio = nuevoMedio;
    a->distanciaKm = nuevaDist;
    a->capacidadMw = nuevaCap;
    a->perdidaPorcentaje = nuevaPerdida;
    return true;
}

bool GrafoRedElectrica::eliminarArco(const std::string& origen, const std::string& destino, const std::string& medio) {
    Vertice* vOrig = buscarVertice(origen);
    if (vOrig == nullptr || vOrig->listaArcos == nullptr) return false;

    Arco* ant = nullptr;
    Arco* act = vOrig->listaArcos;
    while (act != nullptr) {
        if (act->destino->nombre == destino && (medio.empty() || act->medio == medio)) {
            if (ant == nullptr) {
                vOrig->listaArcos = act->siguienteArco;
            } else {
                ant->siguienteArco = act->siguienteArco;
            }
            delete act;
            --totalArcos;
            return true;
        }
        ant = act;
        act = act->siguienteArco;
    }
    return false;
}

void GrafoRedElectrica::mostrarMultilistaCompleta() const {
    std::cout << "\n=== ESTRUCTURA MULTILISTA DE LA RED ELECTRICA (" << totalVertices
              << " Vertices, " << totalArcos << " Lineas) ===\n";
    Vertice* v = primerVertice;
    while (v != nullptr) {
        std::cout << "[" << v->region << "] " << v->nombre << " (" << v->tipo << ")\n";
        Arco* a = v->listaArcos;
        if (a == nullptr) {
            std::cout << "    -> (Sin lineas de transmision salientes)\n";
        }
        while (a != nullptr) {
            std::cout << "    ---> " << a->destino->nombre << " [" << a->destino->region << "]"
                      << " | Medio: " << a->medio
                      << " | Dist: " << a->distanciaKm << " km"
                      << " | Cap: " << a->capacidadMw << " MW"
                      << " | Perdida: " << a->perdidaPorcentaje << "%"
                      << " | Usos: " << a->usoConteo << "\n";
            a = a->siguienteArco;
        }
        v = v->siguienteVertice;
    }
    std::cout << "===========================================================================\n";
}

Vertice** GrafoRedElectrica::construirArregloVertices(int& n) const {
    n = totalVertices;
    if (n == 0) return nullptr;
    Vertice** arr = new Vertice*[n];
    Vertice* aux = primerVertice;
    int i = 0;
    while (aux != nullptr && i < n) {
        arr[i++] = aux;
        aux = aux->siguienteVertice;
    }
    return arr;
}

int GrafoRedElectrica::obtenerIndiceVertice(Vertice* v, Vertice** mapaVertices, int n) const {
    for (int i = 0; i < n; ++i) {
        if (mapaVertices[i] == v) return i;
    }
    return -1;
}

// B.1 Recorrido en Amplitud (BFS) con cola dinamica propia
void GrafoRedElectrica::recorridoBFS(const std::string& nombreInicio) const {
    Vertice* inicio = buscarVertice(nombreInicio);
    if (inicio == nullptr) {
        std::cout << "[Error] El nodo inicial '" << nombreInicio << "' no existe en la red.\n";
        return;
    }

    int n = 0;
    Vertice** mapa = construirArregloVertices(n);
    bool* visitados = new bool[n];
    for (int i = 0; i < n; ++i) visitados[i] = false;

    // Cola enlazada simple dinamica
    struct NodoCola {
        Vertice* v;
        int nivel;
        NodoCola* sig;
        NodoCola(Vertice* _v, int _n) : v(_v), nivel(_n), sig(nullptr) {}
    };

    NodoCola* frente = new NodoCola(inicio, 0);
    NodoCola* finalCola = frente;
    visitados[obtenerIndiceVertice(inicio, mapa, n)] = true;

    std::cout << "\n=== RECORRIDO EN AMPLITUD (BFS) DESDE: " << inicio->nombre << " ===\n";
    int orden = 1;
    while (frente != nullptr) {
        NodoCola* actual = frente;
        frente = frente->sig;
        if (frente == nullptr) finalCola = nullptr;

        Vertice* u = actual->v;
        std::cout << "  " << orden++ << ". [Nivel " << actual->nivel << "] "
                  << u->nombre << " (" << u->tipo << " - Region: " << u->region << ")\n";

        Arco* a = u->listaArcos;
        while (a != nullptr) {
            int idxDest = obtenerIndiceVertice(a->destino, mapa, n);
            if (idxDest != -1 && !visitados[idxDest]) {
                visitados[idxDest] = true;
                NodoCola* nuevo = new NodoCola(a->destino, actual->nivel + 1);
                if (finalCola == nullptr) {
                    frente = finalCola = nuevo;
                } else {
                    finalCola->sig = nuevo;
                    finalCola = nuevo;
                }
            }
            a = a->siguienteArco;
        }
        delete actual;
    }
    std::cout << "=================================================================\n";

    delete[] visitados;
    delete[] mapa;
}

// B.2 Recorrido en Profundidad (DFS)
void GrafoRedElectrica::dfsRecursivo(Vertice* actual, bool* visitados, Vertice** mapaVertices, int n) const {
    int idx = obtenerIndiceVertice(actual, mapaVertices, n);
    if (idx == -1 || visitados[idx]) return;

    visitados[idx] = true;
    std::cout << "  -> " << actual->nombre << " [" << actual->tipo << " | Region: " << actual->region << "]\n";

    Arco* a = actual->listaArcos;
    while (a != nullptr) {
        int idxDest = obtenerIndiceVertice(a->destino, mapaVertices, n);
        if (idxDest != -1 && !visitados[idxDest]) {
            std::cout << "     (via " << a->medio << ", " << a->distanciaKm << " km, perdida " << a->perdidaPorcentaje << "%)\n";
            dfsRecursivo(a->destino, visitados, mapaVertices, n);
        }
        a = a->siguienteArco;
    }
}

void GrafoRedElectrica::recorridoDFS(const std::string& nombreInicio) const {
    Vertice* inicio = buscarVertice(nombreInicio);
    if (inicio == nullptr) {
        std::cout << "[Error] El nodo inicial '" << nombreInicio << "' no existe en la red.\n";
        return;
    }

    int n = 0;
    Vertice** mapa = construirArregloVertices(n);
    bool* visitados = new bool[n];
    for (int i = 0; i < n; ++i) visitados[i] = false;

    std::cout << "\n=== RECORRIDO EN PROFUNDIDAD (DFS) DESDE: " << inicio->nombre << " ===\n";
    dfsRecursivo(inicio, visitados, mapa, n);
    std::cout << "===================================================================\n";

    delete[] visitados;
    delete[] mapa;
}

// B.5 Nodos Inactivos (no registrados en la lista de ningun cliente)
void GrafoRedElectrica::reportarNodosInactivos(const ListaClientes& listaClientes) const {
    std::cout << "\n=== REPORTE DE NODOS INACTIVOS (SIN CLIENTES CONECTADOS) ===\n";
    Vertice* v = primerVertice;
    int inactivos = 0;
    while (v != nullptr) {
        if (!listaClientes.clienteTieneSubestacion(v->nombre)) {
            ++inactivos;
            std::cout << "  " << inactivos << ". " << v->nombre
                      << " | Tipo: " << v->tipo
                      << " | Region: " << v->region << "\n";
        }
        v = v->siguienteVertice;
    }
    if (inactivos == 0) {
        std::cout << "  Todos los nodos de la red estan asociados al menos a un cliente.\n";
    } else {
        std::cout << "Total de nodos inactivos detectados: " << inactivos << "\n";
    }
    std::cout << "============================================================\n";
}

// C.1 Todas las Rutas Posibles filtradas por 1 o 2 medios de transmision
void GrafoRedElectrica::buscarTodasRutasAux(Vertice* actual, Vertice* destino,
                                            const std::string& medio1, const std::string& medio2,
                                            bool* visitados, Vertice** mapaVertices, int n,
                                            Arco** caminoActual, int profundidad, int& rutasEncontradas) const {
    if (actual == destino && profundidad > 0) {
        ++rutasEncontradas;
        double distTotal = 0.0;
        double perdidaTotal = 0.0;
        std::cout << "\n  Ruta #" << rutasEncontradas << ":\n";
        for (int i = 0; i < profundidad; ++i) {
            Arco* a = caminoActual[i];
            distTotal += a->distanciaKm;
            perdidaTotal += a->perdidaPorcentaje;
            std::cout << "    Tramo " << (i + 1) << ": Origen: " << a->origen->nombre
                      << " -> Destino: " << a->destino->nombre
                      << " | Peso (Perdida: " << a->perdidaPorcentaje << "%, Dist: " << a->distanciaKm
                      << " km, Medio: " << a->medio << ")\n";
        }
        std::cout << "    >> Resumen Ruta #" << rutasEncontradas << ": Distancia total = "
                  << distTotal << " km | Perdida acumulada = " << perdidaTotal << "%\n";
        return;
    }

    int idxActual = obtenerIndiceVertice(actual, mapaVertices, n);
    visitados[idxActual] = true;

    Arco* a = actual->listaArcos;
    while (a != nullptr) {
        bool medioValido = (a->medio == medio1) || (!medio2.empty() && a->medio == medio2);
        int idxDest = obtenerIndiceVertice(a->destino, mapaVertices, n);
        if (medioValido && idxDest != -1 && !visitados[idxDest]) {
            caminoActual[profundidad] = a;
            buscarTodasRutasAux(a->destino, destino, medio1, medio2,
                                visitados, mapaVertices, n,
                                caminoActual, profundidad + 1, rutasEncontradas);
        }
        a = a->siguienteArco;
    }

    visitados[idxActual] = false;
}

void GrafoRedElectrica::consultarTodasLasRutas(const std::string& origen, const std::string& destino,
                                              const std::string& medio1, const std::string& medio2) const {
    Vertice* vOrig = buscarVertice(origen);
    Vertice* vDest = buscarVertice(destino);
    if (vOrig == nullptr || vDest == nullptr) {
        std::cout << "[Error] Alguno de los nodos especificados no existe en la red.\n";
        return;
    }
    if (vOrig == vDest) {
        std::cout << "[Aviso] El nodo origen y destino son iguales.\n";
        return;
    }

    int n = 0;
    Vertice** mapa = construirArregloVertices(n);
    bool* visitados = new bool[n];
    for (int i = 0; i < n; ++i) visitados[i] = false;
    Arco** caminoActual = new Arco*[n];
    int rutasEncontradas = 0;

    std::cout << "\n=== TODAS LAS RUTAS POSIBLES: " << origen << " -> " << destino << " ===\n";
    std::cout << "Filtro de medio(s): " << medio1 << (medio2.empty() ? "" : " y " + medio2) << "\n";

    buscarTodasRutasAux(vOrig, vDest, medio1, medio2, visitados, mapa, n, caminoActual, 0, rutasEncontradas);

    if (rutasEncontradas == 0) {
        std::cout << "  No se encontraron rutas disponibles con los medios seleccionados.\n";
    } else {
        std::cout << "\nTotal de rutas encontradas: " << rutasEncontradas << "\n";
    }
    std::cout << "===================================================================\n";

    delete[] caminoActual;
    delete[] visitados;
    delete[] mapa;
}

// C.2 Ruta con Menor Perdida de Energia (Algoritmo de Dijkstra)
void GrafoRedElectrica::consultarRutaMenorPerdida(const std::string& origen, const std::string& destino) const {
    Vertice* vOrig = buscarVertice(origen);
    Vertice* vDest = buscarVertice(destino);
    if (vOrig == nullptr || vDest == nullptr) {
        std::cout << "[Error] Nodo origen o destino no encontrado.\n";
        return;
    }

    int n = 0;
    Vertice** mapa = construirArregloVertices(n);
    int idxOrig = obtenerIndiceVertice(vOrig, mapa, n);
    int idxDest = obtenerIndiceVertice(vDest, mapa, n);

    const double INF = std::numeric_limits<double>::infinity();
    double* dist = new double[n];
    bool* visitado = new bool[n];
    int* padre = new int[n];
    Arco** arcoPadre = new Arco*[n];

    for (int i = 0; i < n; ++i) {
        dist[i] = INF;
        visitado[i] = false;
        padre[i] = -1;
        arcoPadre[i] = nullptr;
    }

    dist[idxOrig] = 0.0;

    for (int count = 0; count < n; ++count) {
        int u = -1;
        double minDist = INF;
        for (int i = 0; i < n; ++i) {
            if (!visitado[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        if (u == -1 || u == idxDest) break;
        visitado[u] = true;

        Arco* a = mapa[u]->listaArcos;
        while (a != nullptr) {
            int v = obtenerIndiceVertice(a->destino, mapa, n);
            if (v != -1 && !visitado[v]) {
                // Peso combinado de perdida energetica + factor de distancia
                double peso = a->perdidaPorcentaje + (a->distanciaKm * 0.01);
                if (dist[u] + peso < dist[v]) {
                    dist[v] = dist[u] + peso;
                    padre[v] = u;
                    arcoPadre[v] = a;
                }
            }
            a = a->siguienteArco;
        }
    }

    std::cout << "\n=== RUTA OPTIMA DE MENOR PERDIDA ENERGETICA (DIJKSTRA) ===\n";
    if (dist[idxDest] == INF) {
        std::cout << "  No existe camino dirigido desde '" << origen << "' hasta '" << destino << "'.\n";
    } else {
        // Reconstruir ruta
        Arco** ruta = new Arco*[n];
        int len = 0;
        int curr = idxDest;
        while (curr != idxOrig && curr != -1) {
            ruta[len++] = arcoPadre[curr];
            curr = padre[curr];
        }

        double perdidaReal = 0.0;
        double distanciaReal = 0.0;
        for (int i = len - 1; i >= 0; --i) {
            Arco* a = ruta[i];
            double pesoTramo = a->perdidaPorcentaje + (a->distanciaKm * 0.01);
            perdidaReal += a->perdidaPorcentaje;
            distanciaReal += a->distanciaKm;
            std::cout << "  Tramo " << (len - i) << ": Origen: " << a->origen->nombre
                      << " -> Destino: " << a->destino->nombre
                      << " | Peso: " << std::fixed << std::setprecision(2) << pesoTramo
                      << " (Perdida: " << a->perdidaPorcentaje << "%, Dist: " << a->distanciaKm
                      << " km, Medio: " << a->medio << ")\n";
        }
        std::cout << "  >> Perdida energetica acumulada: " << perdidaReal << "% | Distancia total: "
                  << distanciaReal << " km | Costo Dijkstra: " << dist[idxDest] << "\n";
        delete[] ruta;
    }
    std::cout << "==========================================================\n";

    delete[] dist;
    delete[] visitado;
    delete[] padre;
    delete[] arcoPadre;
    delete[] mapa;
}

// Formula documentada de asignacion de Eco-Tokens por arco:
// - Base por tipo de medio: subterraneo (+30 pts por estabilidad/cero impacto), alta_tension (+20 pts), red_local (+10 pts)
// - Factor de capacidad: +1 punto por cada 25 MW de capacidad de carga
// - Penalizacion por perdida: -2 puntos por cada 1% de perdida energetica
// - Bonificacion por horario no pico: +25% adicional sobre el puntaje del tramo
int GrafoRedElectrica::calcularTokensArco(const Arco* arco, bool horaNoPico) const {
    int baseMedio = 10;
    if (arco->medio == "subterraneo") baseMedio = 30;
    else if (arco->medio == "alta_tension") baseMedio = 20;

    int puntosCapacidad = static_cast<int>(arco->capacidadMw / 25.0);
    int penalizacionPerdida = static_cast<int>(std::round(arco->perdidaPorcentaje * 2.0));
    int subtotal = baseMedio + puntosCapacidad - penalizacionPerdida;
    if (subtotal < 5) subtotal = 5; // Minimo garantizado por tramo operativo

    if (horaNoPico) {
        subtotal = static_cast<int>(std::round(subtotal * 1.25));
    }
    return subtotal;
}

void GrafoRedElectrica::buscarMaxTokensAux(Vertice* actual, Vertice* destino, bool horaNoPico,
                                           bool* visitados, Vertice** mapaVertices, int n,
                                           Arco** caminoActual, int profundidad,
                                           int tokensAcumulados, int& mejorPuntaje,
                                           Arco** mejorCamino, int& mejorLongitud) const {
    if (actual == destino && profundidad > 0) {
        if (tokensAcumulados > mejorPuntaje) {
            mejorPuntaje = tokensAcumulados;
            mejorLongitud = profundidad;
            for (int i = 0; i < profundidad; ++i) {
                mejorCamino[i] = caminoActual[i];
            }
        }
        return;
    }

    int idxActual = obtenerIndiceVertice(actual, mapaVertices, n);
    visitados[idxActual] = true;

    Arco* a = actual->listaArcos;
    while (a != nullptr) {
        int idxDest = obtenerIndiceVertice(a->destino, mapaVertices, n);
        if (idxDest != -1 && !visitados[idxDest]) {
            caminoActual[profundidad] = a;
            int pts = calcularTokensArco(a, horaNoPico);
            buscarMaxTokensAux(a->destino, destino, horaNoPico,
                               visitados, mapaVertices, n,
                               caminoActual, profundidad + 1,
                               tokensAcumulados + pts, mejorPuntaje,
                               mejorCamino, mejorLongitud);
        }
        a = a->siguienteArco;
    }

    visitados[idxActual] = false;
}

// C.3 Ruta de Mayor Capacidad / Eco-Tokens con actualizacion de cliente
void GrafoRedElectrica::consultarRutaMayorTokens(const std::string& nombreCliente, const std::string& origen,
                                                 const std::string& destino, bool horaNoPico,
                                                 ListaClientes& listaClientes) {
    NodoCliente* cliente = listaClientes.buscarCliente(nombreCliente);
    if (cliente == nullptr) {
        std::cout << "[Error] El cliente '" << nombreCliente << "' no existe.\n";
        return;
    }
    Vertice* vOrig = buscarVertice(origen);
    Vertice* vDest = buscarVertice(destino);
    if (vOrig == nullptr || vDest == nullptr || vOrig == vDest) {
        std::cout << "[Error] Nodos de origen o destino invalidos.\n";
        return;
    }

    int n = 0;
    Vertice** mapa = construirArregloVertices(n);
    bool* visitados = new bool[n];
    for (int i = 0; i < n; ++i) visitados[i] = false;
    Arco** caminoActual = new Arco*[n];
    Arco** mejorCamino = new Arco*[n];
    int mejorPuntaje = -1;
    int mejorLongitud = 0;

    buscarMaxTokensAux(vOrig, vDest, horaNoPico, visitados, mapa, n,
                       caminoActual, 0, 0, mejorPuntaje, mejorCamino, mejorLongitud);

    std::cout << "\n=== RUTA DE MAYOR CAPACIDAD Y ECO-TOKENS ===\n";
    if (mejorPuntaje <= 0 || mejorLongitud == 0) {
        std::cout << "  No existe una ruta disponible entre '" << origen << "' y '" << destino << "'.\n";
    } else {
        std::cout << "Cliente: " << cliente->nombre << " | Horario: " << (horaNoPico ? "Hora No Pico (+25%)" : "Hora Pico") << "\n";
        for (int i = 0; i < mejorLongitud; ++i) {
            Arco* a = mejorCamino[i];
            int ptsTramo = calcularTokensArco(a, horaNoPico);
            std::cout << "  Tramo " << (i + 1) << ": Origen: " << a->origen->nombre
                      << " -> Destino: " << a->destino->nombre
                      << " | Cap: " << a->capacidadMw << " MW | Medio: " << a->medio
                      << " | Eco-Tokens: +" << ptsTramo << "\n";
        }
        std::cout << "  >> TOTAL ECO-TOKENS A OTORGAR: " << mejorPuntaje << " tokens\n";

        char confirmar;
        std::cout << "Desea que el cliente tome esta ruta y actualizar su perfil? (s/n): ";
        std::cin >> confirmar;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirmar == 's' || confirmar == 'S') {
            cliente->tokens += mejorPuntaje;
            listaClientes.registrarSubestacionCliente(cliente, mejorCamino[0]->origen->nombre);
            for (int i = 0; i < mejorLongitud; ++i) {
                mejorCamino[i]->usoConteo += 1;
                listaClientes.registrarSubestacionCliente(cliente, mejorCamino[i]->destino->nombre);
            }
            std::cout << "[Exito] Ruta asignada. Nuevo saldo de Eco-Tokens de " << cliente->nombre
                      << ": " << cliente->tokens << " tokens. Subestaciones actualizadas.\n";
        } else {
            std::cout << "[Info] Simulacion finalizada sin modificar los datos del cliente.\n";
        }
    }
    std::cout << "============================================\n";

    delete[] mejorCamino;
    delete[] caminoActual;
    delete[] visitados;
    delete[] mapa;
}

// C.5 Reportar la ruta de transmision mas utilizada y una ruta utilizada solo una vez
void GrafoRedElectrica::reportarRutaFrecuenteYUnica() const {
    std::cout << "\n=== REPORTE DE FRECUENCIA DE LINEAS DE TRANSMISION ===\n";
    Arco* masFrecuente = nullptr;
    Arco* unicaVez = nullptr;

    Vertice* v = primerVertice;
    while (v != nullptr) {
        Arco* a = v->listaArcos;
        while (a != nullptr) {
            if (masFrecuente == nullptr || a->usoConteo > masFrecuente->usoConteo) {
                masFrecuente = a;
            }
            if (a->usoConteo == 1 && unicaVez == nullptr) {
                unicaVez = a;
            }
            a = a->siguienteArco;
        }
        v = v->siguienteVertice;
    }

    if (masFrecuente != nullptr) {
        std::cout << "  * Ruta Mas Utilizada en la Red:\n"
                  << "      Origen: " << masFrecuente->origen->nombre << " (" << masFrecuente->origen->region << ")\n"
                  << "      Destino: " << masFrecuente->destino->nombre << " (" << masFrecuente->destino->region << ")\n"
                  << "      Medio: " << masFrecuente->medio << " | Usos registrados: " << masFrecuente->usoConteo << " veces\n";
    } else {
        std::cout << "  * No hay lineas de transmision registradas.\n";
    }

    if (unicaVez != nullptr) {
        std::cout << "\n  * Ruta Utilizada Una Sola Vez (uso = 1):\n"
                  << "      Origen: " << unicaVez->origen->nombre << " (" << unicaVez->origen->region << ")\n"
                  << "      Destino: " << unicaVez->destino->nombre << " (" << unicaVez->destino->region << ")\n"
                  << "      Medio: " << unicaVez->medio << " | Distancia: " << unicaVez->distanciaKm << " km\n";
    } else {
        std::cout << "\n  * No existe actualmente ninguna linea con exactamente 1 uso registrado.\n";
    }
    std::cout << "======================================================\n";
}
