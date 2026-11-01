#!/bin/bash

# Compila o projeto antes de iniciar os testes
make clean && make

if [ ! -f ./trabalho ]; then
    echo "Erro: O executável './trabalho' não foi encontrado."
    exit 1
fi

# Configurações do teste
READER="nasa"
INPUT="dados/asteroids.csv"
QUERY="consultas/q1.txt"
OUTPUT="resultado_temp.csv"
EXECUCOES=3  # Quantidade de vezes que cada configuração será executada para tirar a média

# Definição das estruturas a testar: "BASE RESULTADO"
COMBINACOES=(
    "pilha pilha"
    "fila fila"
    "lista_seq lista_seq"
    "lista_enc lista_enc"
)

# Testes de tamanho (N limit e K top-k)
LIMITS=(1000 5000 10000 25000 50000 0)
TOPKS=(10 50 100 500 1000 0)

echo "=========================================================================="
echo "INICIANDO BATERIA DE EXPERIMENTOS ($EXECUCOES EXECUÇÕES POR CONFIGURAÇÃO)"
echo "=========================================================================="

for combo in "${COMBINACOES[@]}"; do
    set -- $combo
    BASE=$1
    RESULT=$2

    echo ""
    echo "=========================================================================="
    echo " TESTANDO: --base $BASE | --result $RESULT"
    echo "=========================================================================="
    printf "%-8s | %-8s | %-8s | %-10s | %-10s | %-10s | %-10s\n" "N" "k" "CARGA(ms)" "BUSCA(ms)" "INSERÇÃO(ms)" "SAÍDA(ms)" "TOTAL(ms)"
    echo "--------------------------------------------------------------------------"

    for i in "${!LIMITS[@]}"; do
        LIMIT=${LIMITS[$i]}
        TOPK=${TOPKS[$i]}

        soma_load=0
        soma_search=0
        soma_insert=0
        soma_write=0

        for ((e=1; e<=EXECUCOES; e++)); do
            # Executa o trabalho e captura a saída
            SAIDA=$(./trabalho --reader $READER --input $INPUT --base $BASE --result $RESULT --query $QUERY --output $OUTPUT --top-k $TOPK --limit $LIMIT)

            # Extrai os tempos impressos pela tua main em milissegundos
            load=$(echo "$SAIDA" | grep "LOAD_MS=" | cut -d'=' -f2)
            search=$(echo "$SAIDA" | grep "SEARCH_MS=" | cut -d'=' -f2)
            insert=$(echo "$SAIDA" | grep "RESULT_INSERT_MS=" | cut -d'=' -f2)
            write=$(echo "$SAIDA" | grep "WRITE_MS=" | cut -d'=' -f2)

            # Acumula os tempos usando o bc para contas com casas decimais
            soma_load=$(echo "$soma_load + $load" | bc)
            soma_search=$(echo "$soma_search + $search" | bc)
            soma_insert=$(echo "$soma_insert + $insert" | bc)
            soma_write=$(echo "$soma_write + $write" | bc)
        done

        # Calcula a média das N execuções
        med_load=$(echo "scale=2; $soma_load / $EXECUCOES" | bc)
        med_search=$(echo "scale=2; $soma_search / $EXECUCOES" | bc)
        med_insert=$(echo "scale=2; $soma_insert / $EXECUCOES" | bc)
        med_write=$(echo "scale=2; $soma_write / $EXECUCOES" | bc)
        med_total=$(echo "scale=2; $med_load + $med_search + $med_insert + $med_write" | bc)

        # Formata N e K para exibição amigável
        N_STR=$LIMIT
        K_STR=$TOPK
        [ "$LIMIT" -eq 0 ] && N_STR="Toda"
        [ "$TOPK" -eq 0 ] && K_STR="Todos"

        printf "%-8s | %-8s | %-10s | %-10s | %-12s | %-10s | %-10s\n" "$N_STR" "$K_STR" "$med_load" "$med_search" "$med_insert" "$med_write" "$med_total"
    done
done

rm -f $OUTPUT
echo ""
echo "Experimentos concluídos com sucesso!"
