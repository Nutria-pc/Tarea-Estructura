#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include "../include/Grafo.hpp"
#include "../include/Clientes.hpp"
#include "../include/Beneficios.hpp"
#include "../include/GestorJSON.hpp"

// Utilidades de lectura segura con validacion de entrada
int leerEntero(const std::string& mensaje) {
    int valor;
    while (true) {
        std::cout << mensaje;
        if (std::cin >> valor) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return valor;
        }
        std::cout << "[Error] Entrada invalida. Debe ingresar un numero entero.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double leerDouble(const std::string& mensaje) {
    double valor;
    while (true) {
        std::cout << mensaje;
        if (std::cin >> valor) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return valor;
        }
        std::cout << "[Error] Entrada invalida. Debe ingresar un valor numerico.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string leerTexto(const std::string& mensaje) {
    std::string linea;
    while (true) {
        std::cout << mensaje;
        std::getline(std::cin, linea);
        if (!linea.empty()) return linea;
        std::cout << "[Error] El campo no puede estar vacio.\n";
    }
}

std::string leerTextoOpcional(const std::string& mensaje) {
    std::string linea;
    std::cout << mensaje;
    std::getline(std::cin, linea);
    return linea;
}

std::string seleccionarMedio(const std::string& etiqueta) {
    while (true) {
        std::cout << etiqueta << "\n"
                  << "  1. alta_tension\n"
                  << "  2. subterraneo\n"
                  << "  3. red_local\n";
        int op = leerEntero("Seleccione una opcion (1-3): ");
        if (op == 1) return "alta_tension";
        if (op == 2) return "subterraneo";
        if (op == 3) return "red_local";
        std::cout << "[Error] Opcion fuera de rango.\n";
    }
}

std::string seleccionarTipoNodo() {
    while (true) {
        std::cout << "Tipo de nodo:\n"
                  << "  1. generador\n"
                  << "  2. subestacion\n"
                  << "  3. subestacion_local\n";
        int op = leerEntero("Seleccione una opcion (1-3): ");
        if (op == 1) return "generador";
        if (op == 2) return "subestacion";
        if (op == 3) return "subestacion_local";
        std::cout << "[Error] Opcion fuera de rango.\n";
    }
}

std::string resolverRutaDatos(const std::string& nombreArchivo) {
    std::string ruta1 = "data/" + nombreArchivo;
    std::ifstream f1(ruta1);
    if (f1.good()) return ruta1;

    std::string ruta2 = "../data/" + nombreArchivo;
    std::ifstream f2(ruta2);
    if (f2.good()) return ruta2;

    return nombreArchivo;
}

// C.4 Canje de Tokens
void realizarCanjeTokens(ListaClientes& clientes, ListaBeneficios& beneficios) {
    std::cout << "\n=== CANJE DE ECO-TOKENS POR BENEFICIOS ===\n";
    clientes.imprimirTodosLosClientes();
    std::string nombreCliente = leerTexto("\nIngrese el nombre exacto del cliente: ");
    NodoCliente* c = clientes.buscarCliente(nombreCliente);
    if (c == nullptr) {
        std::cout << "[Error] Cliente no encontrado.\n";
        return;
    }

    std::cout << "Saldo actual de " << c->nombre << ": " << c->tokens << " Eco-Tokens.\n";
    beneficios.imprimirRecursivo();

    int idBen = leerEntero("Ingrese el ID del beneficio que desea canjear (0 para cancelar): ");
    if (idBen == 0) return;

    NodoBeneficio* b = beneficios.buscarPorId(idBen);
    if (b == nullptr) {
        std::cout << "[Error] No existe un beneficio con el ID " << idBen << ".\n";
        return;
    }

    if (c->tokens < b->costoTokens) {
        std::cout << "[Error] Saldo insuficiente. Requiere " << b->costoTokens
                  << " Eco-Tokens y el cliente dispone de " << c->tokens << ".\n";
        return;
    }

    c->tokens -= b->costoTokens;
    clientes.registrarBeneficioCliente(c, b->nombre);
    std::cout << "[Exito] Canje realizado: '" << b->nombre << "'.\n"
              << "Nuevo saldo de Eco-Tokens de " << c->nombre << ": " << c->tokens << " tokens.\n";
}

// Submenus
void menuGestionGrafo(GrafoRedElectrica& grafo) {
    int op;
    do {
        std::cout << "\n--- GESTION DE RED ELECTRICA (MULTILISTA DE GRAFOS) ---\n"
                  << "1. Mostrar estructura completa de la multilista\n"
                  << "2. Agregar vertice (nodo)\n"
                  << "3. Modificar vertice (nodo)\n"
                  << "4. Eliminar vertice (nodo)\n"
                  << "5. Agregar arco (linea de transmision)\n"
                  << "6. Modificar arco (linea de transmision)\n"
                  << "7. Eliminar arco (linea de transmision)\n"
                  << "0. Volver al menu principal\n";
        op = leerEntero("Seleccione una opcion: ");

        if (op == 1) {
            grafo.mostrarMultilistaCompleta();
        } else if (op == 2) {
            std::string region = leerTexto("Region energetica: ");
            std::string nombre = leerTexto("Nombre del punto de interconexion: ");
            std::string tipo = seleccionarTipoNodo();
            if (grafo.agregarVertice(region, nombre, tipo)) {
                std::cout << "[Exito] Vertice agregado correctamente.\n";
            } else {
                std::cout << "[Error] No se pudo agregar (nombre duplicado o datos invalidos).\n";
            }
        } else if (op == 3) {
            std::string nombre = leerTexto("Nombre del vertice a modificar: ");
            std::string nuevaReg = leerTexto("Nueva region energetica: ");
            std::string nuevoTipo = seleccionarTipoNodo();
            if (grafo.modificarVertice(nombre, nuevaReg, nuevoTipo)) {
                std::cout << "[Exito] Vertice modificado.\n";
            } else {
                std::cout << "[Error] Vertice no encontrado.\n";
            }
        } else if (op == 4) {
            std::string nombre = leerTexto("Nombre del vertice a eliminar: ");
            if (grafo.eliminarVertice(nombre)) {
                std::cout << "[Exito] Vertice y sus conexiones asociadas eliminados.\n";
            } else {
                std::cout << "[Error] Vertice no encontrado.\n";
            }
        } else if (op == 5) {
            std::string orig = leerTexto("Nodo origen: ");
            std::string dest = leerTexto("Nodo destino: ");
            std::string medio = seleccionarMedio("Tipo de medio de transmision:");
            double dist = leerDouble("Distancia (km): ");
            double cap = leerDouble("Capacidad maxima de carga (MW): ");
            double perd = leerDouble("Porcentaje de perdida energetica (%): ");
            if (grafo.agregarArco(orig, dest, medio, dist, cap, perd, 1)) {
                std::cout << "[Exito] Linea de transmision agregada.\n";
            } else {
                std::cout << "[Error] No se pudo agregar el arco (verifique existencia de nodos, medio o duplicados).\n";
            }
        } else if (op == 6) {
            std::string orig = leerTexto("Nodo origen: ");
            std::string dest = leerTexto("Nodo destino: ");
            std::string medioAct = seleccionarMedio("Medio actual del arco:");
            std::string nuevoMedio = seleccionarMedio("Nuevo medio de transmision:");
            double dist = leerDouble("Nueva distancia (km): ");
            double cap = leerDouble("Nueva capacidad (MW): ");
            double perd = leerDouble("Nuevo porcentaje de perdida (%): ");
            if (grafo.modificarArco(orig, dest, medioAct, nuevoMedio, dist, cap, perd)) {
                std::cout << "[Exito] Linea de transmision actualizada.\n";
            } else {
                std::cout << "[Error] No se encontro el arco o los valores son invalidos.\n";
            }
        } else if (op == 7) {
            std::string orig = leerTexto("Nodo origen: ");
            std::string dest = leerTexto("Nodo destino: ");
            std::string medio = seleccionarMedio("Medio de la linea a eliminar:");
            if (grafo.eliminarArco(orig, dest, medio)) {
                std::cout << "[Exito] Linea de transmision eliminada.\n";
            } else {
                std::cout << "[Error] No se encontro la linea especificada.\n";
            }
        }
    } while (op != 0);
}

void menuGestionClientesYBeneficios(ListaClientes& clientes, ListaBeneficios& beneficios) {
    int op;
    do {
        std::cout << "\n--- GESTION DE CLIENTES Y CATALOGO DE BENEFICIOS ---\n"
                  << "1. Listar todos los clientes\n"
                  << "2. Agregar nuevo cliente\n"
                  << "3. Buscar cliente\n"
                  << "4. Modificar cliente\n"
                  << "5. Eliminar cliente\n"
                  << "6. Asignar Eco-Tokens a cliente\n"
                  << "7. Agregar beneficio al catalogo\n"
                  << "8. Modificar beneficio del catalogo\n"
                  << "9. Eliminar beneficio del catalogo\n"
                  << "0. Volver al menu principal\n";
        op = leerEntero("Seleccione una opcion: ");

        if (op == 1) {
            clientes.imprimirTodosLosClientes();
        } else if (op == 2) {
            std::string nombre = leerTexto("Nombre del cliente: ");
            std::string tipo = leerTexto("Tipo de cliente (Hospital/Industrial/Residencial/Comercial): ");
            double consumo = leerDouble("Consumo inicial (kWh): ");
            int tokens = leerEntero("Eco-Tokens iniciales: ");
            if (clientes.agregarCliente(nombre, tipo, consumo, tokens)) {
                std::string sub = leerTextoOpcional("Subestacion conectada inicial (Enter para omitir): ");
                if (!sub.empty()) {
                    clientes.registrarSubestacionCliente(clientes.buscarCliente(nombre), sub);
                }
                std::cout << "[Exito] Cliente registrado correctamente.\n";
            } else {
                std::cout << "[Error] No se pudo agregar el cliente (posible nombre duplicado o valores negativos).\n";
            }
        } else if (op == 3) {
            std::string nombre = leerTexto("Nombre del cliente a buscar: ");
            NodoCliente* c = clientes.buscarCliente(nombre);
            if (c != nullptr) {
                std::cout << "Cliente encontrado: " << c->nombre << " | Tipo: " << c->tipoCliente
                          << " | Consumo: " << c->consumoKwh << " kWh | Tokens: " << c->tokens << "\n";
            } else {
                std::cout << "[Info] Cliente no encontrado.\n";
            }
        } else if (op == 4) {
            std::string nombre = leerTexto("Nombre del cliente a modificar: ");
            std::string tipo = leerTexto("Nuevo tipo de cliente: ");
            double consumo = leerDouble("Nuevo consumo acumulado (kWh): ");
            int tokens = leerEntero("Nuevo saldo de Eco-Tokens: ");
            if (clientes.modificarCliente(nombre, tipo, consumo, tokens)) {
                std::cout << "[Exito] Cliente actualizado.\n";
            } else {
                std::cout << "[Error] No se encontro el cliente o los valores son invalidos.\n";
            }
        } else if (op == 5) {
            std::string nombre = leerTexto("Nombre del cliente a eliminar: ");
            if (clientes.eliminarCliente(nombre)) {
                std::cout << "[Exito] Cliente eliminado.\n";
            } else {
                std::cout << "[Error] Cliente no encontrado.\n";
            }
        } else if (op == 6) {
            std::string nombre = leerTexto("Nombre del cliente: ");
            int cant = leerEntero("Cantidad de Eco-Tokens a asignar: ");
            if (clientes.asignarCreditos(nombre, cant)) {
                std::cout << "[Exito] Creditos asignados correctamente.\n";
            } else {
                std::cout << "[Error] No se pudo asignar creditos (cliente inexistente o saldo negativo).\n";
            }
        } else if (op == 7) {
            int id = beneficios.obtenerSiguienteId();
            std::string nombre = leerTexto("Nombre del beneficio: ");
            int costo = leerEntero("Costo en Eco-Tokens: ");
            std::string desc = leerTexto("Descripcion del beneficio: ");
            if (beneficios.agregarBeneficio(id, nombre, costo, desc)) {
                std::cout << "[Exito] Beneficio agregado con ID " << id << ".\n";
            } else {
                std::cout << "[Error] No se pudo agregar el beneficio.\n";
            }
        } else if (op == 8) {
            beneficios.imprimirRecursivo();
            int id = leerEntero("ID del beneficio a modificar: ");
            std::string nombre = leerTexto("Nuevo nombre: ");
            int costo = leerEntero("Nuevo costo en Eco-Tokens: ");
            std::string desc = leerTexto("Nueva descripcion: ");
            if (beneficios.modificarBeneficio(id, nombre, costo, desc)) {
                std::cout << "[Exito] Beneficio modificado.\n";
            } else {
                std::cout << "[Error] ID no encontrado o datos invalidos.\n";
            }
        } else if (op == 9) {
            beneficios.imprimirRecursivo();
            int id = leerEntero("ID del beneficio a eliminar: ");
            if (beneficios.eliminarBeneficio(id)) {
                std::cout << "[Exito] Beneficio eliminado.\n";
            } else {
                std::cout << "[Error] Beneficio no encontrado.\n";
            }
        }
    } while (op != 0);
}

void menuReportes(const GrafoRedElectrica& grafo, const ListaClientes& clientes, const ListaBeneficios& beneficios) {
    int op;
    do {
        std::cout << "\n--- REPORTES DEL SISTEMA SMART GRID ---\n"
                  << "1. Recorrido en Amplitud (BFS) desde un nodo inicial\n"
                  << "2. Recorrido en Profundidad (DFS) desde un nodo inicial\n"
                  << "3. Lista de Beneficios (Impresion Recursiva)\n"
                  << "4. Reporte de Clientes Activos (con historial)\n"
                  << "5. Reporte de Nodos Inactivos (sin clientes asociados)\n"
                  << "0. Volver al menu principal\n";
        op = leerEntero("Seleccione una opcion: ");

        if (op == 1) {
            std::string inicio = leerTexto("Nombre del nodo inicial para BFS: ");
            grafo.recorridoBFS(inicio);
        } else if (op == 2) {
            std::string inicio = leerTexto("Nombre del nodo inicial para DFS: ");
            grafo.recorridoDFS(inicio);
        } else if (op == 3) {
            beneficios.imprimirRecursivo();
        } else if (op == 4) {
            clientes.imprimirClientesActivos();
        } else if (op == 5) {
            grafo.reportarNodosInactivos(clientes);
        }
    } while (op != 0);
}

void menuConsultasRutas(GrafoRedElectrica& grafo, ListaClientes& clientes, ListaBeneficios& beneficios) {
    int op;
    do {
        std::cout << "\n--- CONSULTAS Y ALGORITMOS DE OPTIMIZACION DE RUTAS ---\n"
                  << "1. Todas las rutas posibles (filtradas por 1 o 2 medios de transmision)\n"
                  << "2. Ruta con menor perdida de energia (Algoritmo de Dijkstra)\n"
                  << "3. Ruta de mayor capacidad y Eco-Tokens (con actualizacion de cliente)\n"
                  << "4. Canje de Eco-Tokens por beneficios\n"
                  << "5. Reportar ruta mas frecuente y ruta utilizada una sola vez\n"
                  << "0. Volver al menu principal\n";
        op = leerEntero("Seleccione una opcion: ");

        if (op == 1) {
            std::string orig = leerTexto("Nodo origen (X): ");
            std::string dest = leerTexto("Nodo destino (Y): ");
            int cantMedios = leerEntero("Desea filtrar por 1 o 2 tipos de medio? (1/2): ");
            std::string m1 = seleccionarMedio("Seleccione el primer medio de transmision:");
            std::string m2 = "";
            if (cantMedios == 2) {
                m2 = seleccionarMedio("Seleccione el segundo medio de transmision:");
            }
            grafo.consultarTodasLasRutas(orig, dest, m1, m2);
        } else if (op == 2) {
            std::string orig = leerTexto("Nodo origen (X): ");
            std::string dest = leerTexto("Nodo destino (Y): ");
            grafo.consultarRutaMenorPerdida(orig, dest);
        } else if (op == 3) {
            std::string cli = leerTexto("Nombre del cliente: ");
            std::string orig = leerTexto("Nodo origen: ");
            std::string dest = leerTexto("Nodo destino: ");
            int horario = leerEntero("El suministro se realiza en Hora No Pico? (1 = Si, 0 = No): ");
            grafo.consultarRutaMayorTokens(cli, orig, dest, (horario == 1), clientes);
        } else if (op == 4) {
            realizarCanjeTokens(clientes, beneficios);
        } else if (op == 5) {
            grafo.reportarRutaFrecuenteYUnica();
        }
    } while (op != 0);
}

int main() {
    GrafoRedElectrica grafo;
    ListaClientes clientes;
    ListaBeneficios beneficios;

    std::string rutaRed = resolverRutaDatos("red_electrica.json");
    std::string rutaClientes = resolverRutaDatos("clientes.json");

    std::cout << "=================================================================\n";
    std::cout << "  INSTITUTO TECNOLOGICO DE COSTA RICA - IC2001 ESTRUCTURA DE DATOS\n";
    std::cout << "  SISTEMA DE OPTIMIZACION Y DISTRIBUCION SMART GRID (C++ POO)\n";
    std::cout << "=================================================================\n";

    if (GestorJSON::cargarRedElectrica(rutaRed, grafo)) {
        std::cout << "[OK] Red electrica cargada desde '" << rutaRed << "' ("
                  << grafo.getTotalVertices() << " vertices, "
                  << grafo.getTotalArcos() << " arcos).\n";
    } else {
        std::cout << "[Advertencia] No se pudo cargar '" << rutaRed << "'. Iniciando grafo vacio.\n";
    }

    if (GestorJSON::cargarClientesYBeneficios(rutaClientes, clientes, beneficios)) {
        std::cout << "[OK] Clientes y beneficios cargados desde '" << rutaClientes << "'.\n";
    } else {
        std::cout << "[Advertencia] No se pudo cargar '" << rutaClientes << "'.\n";
    }

    int opcion;
    do {
        std::cout << "\n==================== MENU PRINCIPAL SMART GRID ====================\n"
                  << "1. Gestion de Grafo (CRUD Vertices y Lineas de Transmision)\n"
                  << "2. Gestion de Clientes y Beneficios (CRUD)\n"
                  << "3. Reportes (BFS, DFS, Beneficios Recursivo, Activos, Inactivos)\n"
                  << "4. Consultas y Algoritmos de Rutas / Canje de Tokens\n"
                  << "5. Guardar cambios en archivos JSON ahora\n"
                  << "0. Guardar y salir del sistema\n"
                  << "===================================================================\n";
        opcion = leerEntero("Seleccione una opcion: ");

        switch (opcion) {
            case 1:
                menuGestionGrafo(grafo);
                break;
            case 2:
                menuGestionClientesYBeneficios(clientes, beneficios);
                break;
            case 3:
                menuReportes(grafo, clientes, beneficios);
                break;
            case 4:
                menuConsultasRutas(grafo, clientes, beneficios);
                break;
            case 5:
                if (GestorJSON::guardarRedElectrica(rutaRed, grafo) &&
                    GestorJSON::guardarClientesYBeneficios(rutaClientes, clientes, beneficios)) {
                    std::cout << "[OK] Cambios persistidos exitosamente en los archivos JSON.\n";
                } else {
                    std::cout << "[Error] Ocurrio un problema al guardar los archivos JSON.\n";
                }
                break;
            case 0:
                std::cout << "Guardando estado final en JSON...\n";
                GestorJSON::guardarRedElectrica(rutaRed, grafo);
                GestorJSON::guardarClientesYBeneficios(rutaClientes, clientes, beneficios);
                std::cout << "Sistema cerrado correctamente. Hasta pronto.\n";
                break;
            default:
                std::cout << "[Error] Opcion no valida.\n";
                break;
        }
    } while (opcion != 0);

    return 0;
}
