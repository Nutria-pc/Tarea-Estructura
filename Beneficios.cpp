#include "../include/Beneficios.hpp"
#include <iomanip>

ListaBeneficios::ListaBeneficios() : cabeza(nullptr) {}

ListaBeneficios::~ListaBeneficios() {
    limpiar();
}

void ListaBeneficios::destruirRecursivo(NodoBeneficio* actual) {
    if (actual == nullptr) return;
    destruirRecursivo(actual->siguiente);
    delete actual;
}

void ListaBeneficios::limpiar() {
    destruirRecursivo(cabeza);
    cabeza = nullptr;
}

int ListaBeneficios::obtenerSiguienteId() const {
    int maxId = 0;
    NodoBeneficio* aux = cabeza;
    while (aux != nullptr) {
        if (aux->id > maxId) maxId = aux->id;
        aux = aux->siguiente;
    }
    return maxId + 1;
}

bool ListaBeneficios::agregarBeneficio(int id, const std::string& nombre, int costoTokens, const std::string& descripcion) {
    if (nombre.empty() || costoTokens < 0) {
        return false;
    }
    if (buscarPorId(id) != nullptr || buscarPorNombre(nombre) != nullptr) {
        return false;
    }
    NodoBeneficio* nuevo = new NodoBeneficio(id, nombre, costoTokens, descripcion);
    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else {
        NodoBeneficio* aux = cabeza;
        while (aux->siguiente != nullptr) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
    return true;
}

NodoBeneficio* ListaBeneficios::buscarPorId(int id) const {
    NodoBeneficio* aux = cabeza;
    while (aux != nullptr) {
        if (aux->id == id) return aux;
        aux = aux->siguiente;
    }
    return nullptr;
}

NodoBeneficio* ListaBeneficios::buscarPorNombre(const std::string& nombre) const {
    NodoBeneficio* aux = cabeza;
    while (aux != nullptr) {
        if (aux->nombre == nombre) return aux;
        aux = aux->siguiente;
    }
    return nullptr;
}

bool ListaBeneficios::modificarBeneficio(int id, const std::string& nuevoNombre, int nuevoCosto, const std::string& nuevaDesc) {
    NodoBeneficio* obj = buscarPorId(id);
    if (obj == nullptr || nuevoNombre.empty() || nuevoCosto < 0) {
        return false;
    }
    NodoBeneficio* dup = buscarPorNombre(nuevoNombre);
    if (dup != nullptr && dup != obj) {
        return false;
    }
    obj->nombre = nuevoNombre;
    obj->costoTokens = nuevoCosto;
    obj->descripcion = nuevaDesc;
    return true;
}

bool ListaBeneficios::eliminarBeneficio(int id) {
    if (cabeza == nullptr) return false;
    if (cabeza->id == id) {
        NodoBeneficio* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
        return true;
    }
    NodoBeneficio* actual = cabeza;
    while (actual->siguiente != nullptr && actual->siguiente->id != id) {
        actual = actual->siguiente;
    }
    if (actual->siguiente == nullptr) return false;

    NodoBeneficio* eliminar = actual->siguiente;
    actual->siguiente = eliminar->siguiente;
    delete eliminar;
    return true;
}

void ListaBeneficios::imprimirRecursivoAux(NodoBeneficio* actual) const {
    if (actual == nullptr) {
        return;
    }
    std::cout << "  [" << actual->id << "] " << actual->nombre
              << " | Costo: " << actual->costoTokens << " Eco-Tokens\n"
              << "      Detalle: " << actual->descripcion << "\n";
    imprimirRecursivoAux(actual->siguiente);
}

void ListaBeneficios::imprimirRecursivo() const {
    std::cout << "\n=== CATALOGO DE BENEFICIOS Y RECOMPENSAS (IMPRESION RECURSIVA) ===\n";
    if (cabeza == nullptr) {
        std::cout << "  (No hay beneficios registrados en la lista enlazada)\n";
        return;
    }
    imprimirRecursivoAux(cabeza);
    std::cout << "===================================================================\n";
}
