# ====================================================================
# Makefile para Simulador de Hidrômetro
# ====================================================================

# Nome do executável
TARGET = hidrometro_sim

# Diretórios
SRC_DIR = C++/src
CORE_DIR = $(SRC_DIR)/core
MAIN_DIR = $(SRC_DIR)/main
UI_DIR = $(SRC_DIR)/ui
BUILD_DIR = build

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I$(SRC_DIR) -I./libs -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lQt5Widgets -lQt5Core -lQt5Gui

# Arquivos fonte
SRCS = $(MAIN_DIR)/main.cpp \
       $(CORE_DIR)/Configuracao.cpp \
       $(CORE_DIR)/Controladora.cpp \
       $(CORE_DIR)/Entrada.cpp \
       $(CORE_DIR)/Hidrometro.cpp \
       $(CORE_DIR)/Display.cpp \
       $(CORE_DIR)/Relogio.cpp \
       $(UI_DIR)/HidrometroUI.cpp

# Arquivos objeto
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Arquivos moc
MOC_SRCS = $(UI_DIR)/HidrometroUI.moc.cpp
MOC_OBJS = $(MOC_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

.PHONY: all clean debug run rebuild help

# Regra padrão
all: $(BUILD_DIR) $(TARGET)

debug: CXXFLAGS := $(CXXFLAGS) -g
debug: $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

# Regra para criar o executável
$(TARGET): $(OBJS) $(MOC_OBJS)
	@echo "Linkando: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regra para compilar os arquivos .cpp em .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compilando: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para executar o moc
$(UI_DIR)/HidrometroUI.moc.cpp: $(UI_DIR)/HidrometroUI.hpp
	@echo "Executando moc para: $<"
	moc $< -o $@

$(BUILD_DIR)/%.o: $(UI_DIR)/%.moc.cpp | $(BUILD_DIR)
	@echo "Compilando moc: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para criar os diretórios necessários
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/main
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/ui

-include $(DEPS)

clean:
	@echo "Limpando arquivos gerados..."
	@rm -rf $(BUILD_DIR) $(TARGET)

help:
	@echo "Opções:"
	@echo "  make          - Compila em release"
	@echo "  make debug    - Compila em debug"
	@echo "  make run      - Compila e executa"
	@echo "  make clean    - Limpa tudo"
	@echo "  make rebuild  - Rebuild completo"