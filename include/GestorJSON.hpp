#ifndef GESTOR_JSON_HPP
#define GESTOR_JSON_HPP

#include <string>
#include "Grafo.hpp"
#include "Clientes.hpp"
#include "Beneficios.hpp"

class GestorJSON {
public:
    static bool cargarRedElectrica(const std::string& rutaArchivo, GrafoRedElectrica& grafo);
    static bool guardarRedElectrica(const std::string& rutaArchivo, const GrafoRedElectrica& grafo);

    static bool cargarClientesYBeneficios(const std::string& rutaArchivo, ListaClientes& clientes, ListaBeneficios& beneficios);
    static bool guardarClientesYBeneficios(const std::string& rutaArchivo, const ListaClientes& clientes, const ListaBeneficios& beneficios);
};

#endif // GESTOR_JSON_HPP
