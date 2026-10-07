EXEC = trabalho #nome do arquivo executável final
CC = gcc #compilador a ser usado é o gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -Iinclude

# -Wall -Wextra -pedantic: Ativam todos os avisos (warnings) do compilador para garantir que o código não tenha erros ocultos ou boas práticas violadas.
# -std=c99: Define o padrão da linguagem C (C99)
# -g: Inclui informações de depuração no binário
# -Iinclude: Diz ao compilador onde procurar os arquivos .h


SRCS = $(shell find src -name '*.c') #Executa um comando no terminal para buscar todos os arquivos com extensão .c dentro da pasta src/ e de suas subpastas
OBJS = $(SRCS:.c=.o) #Cria uma lista equivalente substituindo a extensão .c por .o para cada arquivo encontrado em SRCS

all: $(EXEC)

$(EXEC): $(OBJS) #Cria o executável ligando todos os arquivos .o
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) # Junta os .o e cria o programa final

%.o: %.c #Regra genérica para compilar código-fonte individualmente
	$(CC) $(CFLAGS) -c $< -o $@

clean: #Permite limpar a pasta do projeto executando make clean no terminal
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean