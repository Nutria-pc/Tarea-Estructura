#ifndef CLIENTES_HPP
#define CLIENTES_HPP

#include <iostream>
#include <string>

// Nodo para lista enlazada simple de strings (subestaciones conectadas y beneficios canjeados)
struct NodoTexto {
    std::string dato;
    NodoTexto* siguiente;

    explicit NodoTexto(const std::string& d) : dato(d), siguiente(nullptr) {}
};

// Nodo de cliente en lista doblemente enlazada dinamica
struct NodoCliente {
    std::string nombre;
    std::string tipoCliente;
    double consumoKwh;
    int tokens;
    NodoTexto* subestacionesConectadas;
    NodoTexto* beneficiosCanjeados;
    NodoCliente* anterior;
    NodoCliente* siguiente;

    NodoCliente(const std::string& _nombre, const std::string& _tipo, double _consumo, int _tokens)
        : nombre(_nombre), tipoCliente(_tipo), consumoKwh(_consumo), tokens(_tokens),
          subestacionesConectadas(nullptr), beneficiosCanjeados(nullptr),
          anterior(nullptr), siguiente(nullptr) {}
};

class ListaClientes {
private:
    NodoCliente* cabeza;
    NodoCliente* cola;

    void limpiarListaTexto(NodoTexto*& head);
    bool existeEnListaTexto(NodoTexto* head, const std::string& val) const;
    void agregarAListaTexto(NodoTexto*& head, const std::string& val);

public:
    ListaClientes();
    ~ListaClientes();

    NodoCliente* getCabeza() const { return cabeza; }

    // CRUD Clientes
    bool agregarCliente(const std::string& nombre, const std::string& tipoCliente, double consumoKwh, int tokens);
    NodoCliente* buscarCliente(const std::string& nombre) const;
    bool modificarCliente(const std::string& nombre, const std::string& nuevoTipo, double nuevoConsumo, int nuevosTokens);
    bool eliminarCliente(const std::string& nombre);
    bool asignarCreditos(const std::string& nombre, int cantidadTokens);

    // Relaciones con subestaciones y beneficios
    void registrarSubestacionCliente(NodoCliente* cliente, const std::string& nombreSubestacion);
    void registrarBeneficioCliente(NodoCliente* cliente, const std::string& nombreBeneficio);
    bool clienteTieneSubestacion(const std::string& nombreSubestacion) const;

    // Reporte B.4: Listar clientes activos (omitiendo clientes sin historial)
    void imprimirClientesActivos() const;
    void imprimirTodosLosClientes() const;
    void limpiar();
};

#endif // CLIENTES_HPP
