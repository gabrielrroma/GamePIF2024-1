# Makefile para o jogo Pong

# Compilador
CC = gcc

# Opções de compilação
CFLAGS = -std=c11 -Wall -Wextra -pedantic -MMD
INCLUDES = -Isrc

# Nome do executável
EXEC = pong_game

# Diretórios
SRC_DIR = src
OBJ_DIR = obj

# Arquivos fonte
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPENDENCIES = $(OBJECTS:.o=.d)

# Regra de compilação principal
all: $(EXEC)

# Regra de compilação do executável
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Regra de compilação dos objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Cria o diretório de objetos se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpeza dos arquivos intermediários e do executável
clean:
	rm -f $(OBJECTS) $(DEPENDENCIES) $(EXEC)

# Inclui as dependências
-include $(DEPENDENCIES)
