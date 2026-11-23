CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

SRCS = src/main.cpp src/Grafo.cpp src/Clientes.cpp src/Beneficios.cpp src/GestorJSON.cpp
TARGET = smart_grid

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
