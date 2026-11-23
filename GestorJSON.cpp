#include "../include/GestorJSON.hpp"
#include "../include/json.hpp"
#include <iostream>

bool GestorJSON::cargarRedElectrica(const std::string& rutaArchivo, GrafoRedElectrica& grafo) {
    try {
        sjson::Value root = sjson::parseFile(rutaArchivo);
        grafo.limpiar();

        if (!root.hasKey("regiones") || !root.at("regiones").isArray()) {
            std::cerr << "[Error JSON] Falta el arreglo 'regiones' en " << rutaArchivo << "\n";
            return false;
        }

        for (const auto& regObj : root.at("regiones").asArray()) {
            std::string nombreRegion = regObj.hasKey("nombre") ? regObj.at("nombre").asString() : "Sin Region";
            if (regObj.hasKey("puntos_interconexion") && regObj.at("puntos_interconexion").isArray()) {
                for (const auto& punto : regObj.at("puntos_interconexion").asArray()) {
                    std::string nombrePunto = punto.at("nombre").asString();
                    std::string tipoPunto = punto.hasKey("tipo") ? punto.at("tipo").asString() : "subestacion";
                    grafo.agregarVertice(nombreRegion, nombrePunto, tipoPunto);
                }
            }
        }

        if (root.hasKey("lineas_transmision") && root.at("lineas_transmision").isArray()) {
            for (const auto& linea : root.at("lineas_transmision").asArray()) {
                std::string origenPunto = linea.at("origen").at("punto").asString();
                std::string destinoPunto = linea.at("destino").at("punto").asString();
                std::string medio = linea.hasKey("medio") ? linea.at("medio").asString() : "alta_tension";
                double dist = linea.hasKey("distancia_km") ? linea.at("distancia_km").asDouble() : 10.0;
                double cap = linea.hasKey("capacidad_mw") ? linea.at("capacidad_mw").asDouble() : 250.0;
                double perdida = linea.hasKey("perdida_porcentaje") ? linea.at("perdida_porcentaje").asDouble() : 1.5;
                int uso = linea.hasKey("uso_conteo") ? linea.at("uso_conteo").asInt() : 1;

                grafo.agregarArco(origenPunto, destinoPunto, medio, dist, cap, perdida, uso);
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Excepcion al cargar red_electrica.json]: " << e.what() << "\n";
        return false;
    }
}

bool GestorJSON::guardarRedElectrica(const std::string& rutaArchivo, const GrafoRedElectrica& grafo) {
    try {
        sjson::Value root = sjson::Value::makeObject();
        sjson::Value regionesArr = sjson::Value::makeArray();

        // Agrupar vertices por region dinamicamente sin usar contenedores STL para el grafo
        struct NodoRegionTemp {
            std::string nombre;
            NodoRegionTemp* sig;
            NodoRegionTemp(const std::string& n) : nombre(n), sig(nullptr) {}
        };
        NodoRegionTemp* listaReg = nullptr;

        Vertice* v = grafo.getPrimerVertice();
        while (v != nullptr) {
            NodoRegionTemp* aux = listaReg;
            bool existe = false;
            while (aux != nullptr) {
                if (aux->nombre == v->region) {
                    existe = true;
                    break;
                }
                aux = aux->sig;
            }
            if (!existe) {
                NodoRegionTemp* nuevo = new NodoRegionTemp(v->region);
                nuevo->sig = listaReg;
                listaReg = nuevo;
            }
            v = v->siguienteVertice;
        }

        NodoRegionTemp* regAct = listaReg;
        while (regAct != nullptr) {
            sjson::Value regObj = sjson::Value::makeObject();
            regObj["nombre"] = regAct->nombre;
            sjson::Value puntosArr = sjson::Value::makeArray();

            Vertice* vert = grafo.getPrimerVertice();
            while (vert != nullptr) {
                if (vert->region == regAct->nombre) {
                    sjson::Value pObj = sjson::Value::makeObject();
                    pObj["nombre"] = vert->nombre;
                    pObj["tipo"] = vert->tipo;
                    puntosArr.push_back(pObj);
                }
                vert = vert->siguienteVertice;
            }
            regObj["puntos_interconexion"] = puntosArr;
            regionesArr.push_back(regObj);

            NodoRegionTemp* borrar = regAct;
            regAct = regAct->sig;
            delete borrar;
        }

        sjson::Value lineasArr = sjson::Value::makeArray();
        Vertice* vOrig = grafo.getPrimerVertice();
        while (vOrig != nullptr) {
            Arco* a = vOrig->listaArcos;
            while (a != nullptr) {
                sjson::Value lObj = sjson::Value::makeObject();
                sjson::Value origObj = sjson::Value::makeObject();
                origObj["region"] = a->origen->region;
                origObj["punto"] = a->origen->nombre;

                sjson::Value destObj = sjson::Value::makeObject();
                destObj["region"] = a->destino->region;
                destObj["punto"] = a->destino->nombre;

                lObj["origen"] = origObj;
                lObj["destino"] = destObj;
                lObj["medio"] = a->medio;
                lObj["distancia_km"] = a->distanciaKm;
                lObj["capacidad_mw"] = a->capacidadMw;
                lObj["perdida_porcentaje"] = a->perdidaPorcentaje;
                lObj["uso_conteo"] = a->usoConteo;

                lineasArr.push_back(lObj);
                a = a->siguienteArco;
            }
            vOrig = vOrig->siguienteVertice;
        }

        root["regiones"] = regionesArr;
        root["lineas_transmision"] = lineasArr;
        sjson::saveFile(rutaArchivo, root);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Excepcion al guardar red_electrica.json]: " << e.what() << "\n";
        return false;
    }
}

bool GestorJSON::cargarClientesYBeneficios(const std::string& rutaArchivo, ListaClientes& clientes, ListaBeneficios& beneficios) {
    try {
        sjson::Value root = sjson::parseFile(rutaArchivo);
        clientes.limpiar();
        beneficios.limpiar();

        if (root.hasKey("beneficios_disponibles") && root.at("beneficios_disponibles").isArray()) {
            for (const auto& bObj : root.at("beneficios_disponibles").asArray()) {
                int id = bObj.hasKey("id") ? bObj.at("id").asInt() : beneficios.obtenerSiguienteId();
                std::string nombre = bObj.at("nombre").asString();
                int costo = bObj.hasKey("costo_tokens") ? bObj.at("costo_tokens").asInt() : 100;
                std::string desc = bObj.hasKey("descripcion") ? bObj.at("descripcion").asString() : "Beneficio preferencial";
                beneficios.agregarBeneficio(id, nombre, costo, desc);
            }
        } else {
            // Catalogo por defecto si el JSON no incluye beneficios_disponibles
            beneficios.agregarBeneficio(1, "Descuento del 10% en tarifa mensual", 150, "Reduce un 10% el monto total de la factura mensual.");
            beneficios.agregarBeneficio(2, "Mantenimiento prioritario de red", 250, "Atencion tecnica inmediata ante fallas.");
            beneficios.agregarBeneficio(3, "Bono de energia limpia en hora pico", 300, "Exoneracion de recargo en franja horaria pico.");
        }

        if (!root.hasKey("clientes") || !root.at("clientes").isArray()) {
            std::cerr << "[Error JSON] Falta el arreglo 'clientes' en " << rutaArchivo << "\n";
            return false;
        }

        for (const auto& cObj : root.at("clientes").asArray()) {
            std::string nombre = cObj.at("nombre").asString();
            std::string tipo = cObj.hasKey("tipo_cliente") ? cObj.at("tipo_cliente").asString() : "General";
            double consumo = cObj.hasKey("consumo_kwh") ? cObj.at("consumo_kwh").asDouble() : 0.0;
            int tokens = cObj.hasKey("tokens") ? cObj.at("tokens").asInt() : 0;

            if (clientes.agregarCliente(nombre, tipo, consumo, tokens)) {
                NodoCliente* nodoC = clientes.buscarCliente(nombre);
                if (cObj.hasKey("subestaciones_conectadas") && cObj.at("subestaciones_conectadas").isArray()) {
                    for (const auto& sub : cObj.at("subestaciones_conectadas").asArray()) {
                        clientes.registrarSubestacionCliente(nodoC, sub.asString());
                    }
                }
                if (cObj.hasKey("beneficios") && cObj.at("beneficios").isArray()) {
                    for (const auto& ben : cObj.at("beneficios").asArray()) {
                        clientes.registrarBeneficioCliente(nodoC, ben.asString());
                    }
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Excepcion al cargar clientes.json]: " << e.what() << "\n";
        return false;
    }
}

bool GestorJSON::guardarClientesYBeneficios(const std::string& rutaArchivo, const ListaClientes& clientes, const ListaBeneficios& beneficios) {
    try {
        sjson::Value root = sjson::Value::makeObject();

        sjson::Value benArr = sjson::Value::makeArray();
        NodoBeneficio* b = beneficios.getCabeza();
        while (b != nullptr) {
            sjson::Value bObj = sjson::Value::makeObject();
            bObj["id"] = b->id;
            bObj["nombre"] = b->nombre;
            bObj["costo_tokens"] = b->costoTokens;
            bObj["descripcion"] = b->descripcion;
            benArr.push_back(bObj);
            b = b->siguiente;
        }
        root["beneficios_disponibles"] = benArr;

        sjson::Value cliArr = sjson::Value::makeArray();
        NodoCliente* c = clientes.getCabeza();
        while (c != nullptr) {
            sjson::Value cObj = sjson::Value::makeObject();
            cObj["nombre"] = c->nombre;
            cObj["tipo_cliente"] = c->tipoCliente;
            cObj["consumo_kwh"] = c->consumoKwh;
            cObj["tokens"] = c->tokens;

            sjson::Value subArr = sjson::Value::makeArray();
            NodoTexto* s = c->subestacionesConectadas;
            while (s != nullptr) {
                subArr.push_back(s->dato);
                s = s->siguiente;
            }
            cObj["subestaciones_conectadas"] = subArr;

            sjson::Value benCliArr = sjson::Value::makeArray();
            NodoTexto* bc = c->beneficiosCanjeados;
            while (bc != nullptr) {
                benCliArr.push_back(bc->dato);
                bc = bc->siguiente;
            }
            cObj["beneficios"] = benCliArr;

            cliArr.push_back(cObj);
            c = c->siguiente;
        }
        root["clientes"] = cliArr;

        sjson::saveFile(rutaArchivo, root);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Excepcion al guardar clientes.json]: " << e.what() << "\n";
        return false;
    }
}
