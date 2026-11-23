#ifndef BENEFICIOS_HPP
#define BENEFICIOS_HPP

#include <iostream>
#include <string>

struct NodoBeneficio {
    int id;
    std::string nombre;
    int costoTokens;
    std::string descripcion;
    NodoBeneficio* siguiente;

    NodoBeneficio(int _id, const std::string& _nombre, int _costo, const std::string& _desc)
        : id(_id), nombre(_nombre), costoTokens(_costo), descripcion(_desc), siguiente(nullptr) {}
};

class ListaBeneficios {
private:
    NodoBeneficio* cabeza;

    void imprimirRecursivoAux(NodoBeneficio* actual) const;
    void destruirRecursivo(NodoBeneficio* actual);

public:
    ListaBeneficios();
    ~ListaBeneficios();

    NodoBeneficio* getCabeza() const { return cabeza; }

    // CRUD
    bool agregarBeneficio(int id, const std::string& nombre, int costoTokens, const std::string& descripcion);
    NodoBeneficio* buscarPorId(int id) const;
    NodoBeneficio* buscarPorNombre(const std::string& nombre) const;
    bool modificarBeneficio(int id, const std::string& nuevoNombre, int nuevoCosto, const std::string& nuevaDesc);
    bool eliminarBeneficio(int id);
    void limpiar();

    // Reporte B.3: Imprimir utilizando unicamente recursividad
    void imprimirRecursivo() const;
    int obtenerSiguienteId() const;
};

#endif // BENEFICIOS_HPP
