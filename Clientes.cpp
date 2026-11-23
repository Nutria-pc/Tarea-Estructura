#include "../include/Clientes.hpp"
#include <iomanip>

ListaClientes::ListaClientes() : cabeza(nullptr), cola(nullptr) {}

ListaClientes::~ListaClientes() {
    limpiar();
}

void ListaClientes::limpiarListaTexto(NodoTexto*& head) {
    while (head != nullptr) {
        NodoTexto* sig = head->siguiente;
        delete head;
        head = sig;
    }
}

bool ListaClientes::existeEnListaTexto(NodoTexto* head, const std::string& val) const {
    NodoTexto* aux = head;
    while (aux != nullptr) {
        if (aux->dato == val) return true;
        aux = aux->siguiente;
    }
    return false;
}

void ListaClientes::agregarAListaTexto(NodoTexto*& head, const std::string& val) {
    if (val.empty() || existeEnListaTexto(head, val)) return;
    NodoTexto* nuevo = new NodoTexto(val);
    if (head == nullptr) {
        head = nuevo;
    } else {
        NodoTexto* aux = head;
        while (aux->siguiente != nullptr) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
}

void ListaClientes::limpiar() {
    NodoCliente* actual = cabeza;
    while (actual != nullptr) {
        NodoCliente* sig = actual->siguiente;
        limpiarListaTexto(actual->subestacionesConectadas);
        limpiarListaTexto(actual->beneficiosCanjeados);
        delete actual;
        actual = sig;
    }
    cabeza = nullptr;
    cola = nullptr;
}

bool ListaClientes::agregarCliente(const std::string& nombre, const std::string& tipoCliente, double consumoKwh, int tokens) {
    if (nombre.empty() || consumoKwh < 0.0 || tokens < 0) return false;
    if (buscarCliente(nombre) != nullptr) return false;

    NodoCliente* nuevo = new NodoCliente(nombre, tipoCliente.empty() ? "General" : tipoCliente, consumoKwh, tokens);
    if (cabeza == nullptr) {
        cabeza = cola = nuevo;
    } else {
        cola->siguiente = nuevo;
        nuevo->anterior = cola;
        cola = nuevo;
    }
    return true;
}

NodoCliente* ListaClientes::buscarCliente(const std::string& nombre) const {
    NodoCliente* aux = cabeza;
    while (aux != nullptr) {
        if (aux->nombre == nombre) return aux;
        aux = aux->siguiente;
    }
    return nullptr;
}

bool ListaClientes::modificarCliente(const std::string& nombre, const std::string& nuevoTipo, double nuevoConsumo, int nuevosTokens) {
    NodoCliente* c = buscarCliente(nombre);
    if (c == nullptr || nuevoConsumo < 0.0 || nuevosTokens < 0) return false;
    if (!nuevoTipo.empty()) c->tipoCliente = nuevoTipo;
    c->consumoKwh = nuevoConsumo;
    c->tokens = nuevosTokens;
    return true;
}

bool ListaClientes::eliminarCliente(const std::string& nombre) {
    NodoCliente* c = buscarCliente(nombre);
    if (c == nullptr) return false;

    if (c->anterior != nullptr) {
        c->anterior->siguiente = c->siguiente;
    } else {
        cabeza = c->siguiente;
    }

    if (c->siguiente != nullptr) {
        c->siguiente->anterior = c->anterior;
    } else {
        cola = c->anterior;
    }

    limpiarListaTexto(c->subestacionesConectadas);
    limpiarListaTexto(c->beneficiosCanjeados);
    delete c;
    return true;
}

bool ListaClientes::asignarCreditos(const std::string& nombre, int cantidadTokens) {
    NodoCliente* c = buscarCliente(nombre);
    if (c == nullptr) return false;
    if (c->tokens + cantidadTokens < 0) return false;
    c->tokens += cantidadTokens;
    return true;
}

void ListaClientes::registrarSubestacionCliente(NodoCliente* cliente, const std::string& nombreSubestacion) {
    if (cliente == nullptr) return;
    agregarAListaTexto(cliente->subestacionesConectadas, nombreSubestacion);
}

void ListaClientes::registrarBeneficioCliente(NodoCliente* cliente, const std::string& nombreBeneficio) {
    if (cliente == nullptr) return;
    // Un cliente puede canjear multiples veces el mismo beneficio o acumularlos
    if (nombreBeneficio.empty()) return;
    NodoTexto* nuevo = new NodoTexto(nombreBeneficio);
    if (cliente->beneficiosCanjeados == nullptr) {
        cliente->beneficiosCanjeados = nuevo;
    } else {
        NodoTexto* aux = cliente->beneficiosCanjeados;
        while (aux->siguiente != nullptr) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
}

bool ListaClientes::clienteTieneSubestacion(const std::string& nombreSubestacion) const {
    NodoCliente* c = cabeza;
    while (c != nullptr) {
        if (existeEnListaTexto(c->subestacionesConectadas, nombreSubestacion)) {
            return true;
        }
        c = c->siguiente;
    }
    return false;
}

void ListaClientes::imprimirClientesActivos() const {
    std::cout << "\n=== REPORTE DE CLIENTES ACTIVOS (CON HISTORIAL ENERGÉTICO) ===\n";
    NodoCliente* aux = cabeza;
    int activos = 0;
    while (aux != nullptr) {
        bool tieneHistorial = (aux->tokens > 0 || aux->consumoKwh > 0.0 ||
                               aux->subestacionesConectadas != nullptr ||
                               aux->beneficiosCanjeados != nullptr);
        if (tieneHistorial) {
            ++activos;
            std::cout << activos << ". " << aux->nombre << " [" << aux->tipoCliente << "]\n"
                      << "   - Consumo acumulado: " << std::fixed << std::setprecision(2) << aux->consumoKwh << " kWh\n"
                      << "   - Eco-Tokens disponibles: " << aux->tokens << "\n"
                      << "   - Subestaciones conectadas: ";
            if (aux->subestacionesConectadas == nullptr) {
                std::cout << "Ninguna";
            } else {
                NodoTexto* sub = aux->subestacionesConectadas;
                while (sub != nullptr) {
                    std::cout << sub->dato << (sub->siguiente ? ", " : "");
                    sub = sub->siguiente;
                }
            }
            std::cout << "\n   - Beneficios canjeados: ";
            if (aux->beneficiosCanjeados == nullptr) {
                std::cout << "Ninguno";
            } else {
                NodoTexto* ben = aux->beneficiosCanjeados;
                while (ben != nullptr) {
                    std::cout << ben->dato << (ben->siguiente ? ", " : "");
                    ben = ben->siguiente;
                }
            }
            std::cout << "\n--------------------------------------------------------------\n";
        }
        aux = aux->siguiente;
    }
    if (activos == 0) {
        std::cout << "  No hay clientes con historial activo.\n";
    } else {
        std::cout << "Total de clientes activos reportados: " << activos << "\n";
    }
}

void ListaClientes::imprimirTodosLosClientes() const {
    std::cout << "\n=== LISTADO COMPLETO DE CLIENTES REGISTRADOS ===\n";
    NodoCliente* aux = cabeza;
    int idx = 0;
    while (aux != nullptr) {
        ++idx;
        std::cout << idx << ". " << aux->nombre << " (" << aux->tipoCliente
                  << ") | Consumo: " << aux->consumoKwh << " kWh | Tokens: " << aux->tokens << "\n";
        aux = aux->siguiente;
    }
    if (idx == 0) std::cout << "  (Lista vacia)\n";
}
