#!/bin/bash
# Uso: ./rodar_experimentos.sh
INPUT="asteroid_dataset.csv"
QUERY="consultas/q1_asteroides.txt"
READER="nasa"
RUNS=5
OUT="resultados_experimentos.csv"

# pares "limit:topk"
CONFIGS=("1000:10" "5000:50" "10000:100" "25000:500" "50000:1000" "0:0")

# pares base:result a testar
PARES=("pilha:pilha" "fila:fila" "lista_seq:lista_seq" "lista_enc:lista_enc" \
       "lista_cabeca:lista_cabeca" "lista_ord:lista_ord" \
       "lista_gen:lista_gen" "lista_cruz:lista_cruz")

echo "N,base,result,k,achados,carga,busca,insercao_res,saida,total" > "$OUT"

for par in "${PARES[@]}"; do
  BASE="${par%%:*}"; RES="${par##*:}"
  for cfg in "${CONFIGS[@]}"; do
    LIMIT="${cfg%%:*}"; K="${cfg##*:}"
    soma_l=0; soma_b=0; soma_i=0; soma_w=0; achados=0

    for ((r=1; r<=RUNS; r++)); do
      saida=$(./trabalho --reader "$READER" --input "$INPUT" \
              --base "$BASE" --result "$RES" --query "$QUERY" \
              --output /tmp/out.csv --top-k "$K" --limit "$LIMIT")

      l=$(echo "$saida" | grep '^LOAD_MS='          | cut -d= -f2)
      b=$(echo "$saida" | grep '^SEARCH_MS='        | cut -d= -f2)
      i=$(echo "$saida" | grep '^RESULT_INSERT_MS=' | cut -d= -f2)
      w=$(echo "$saida" | grep '^WRITE_MS='         | cut -d= -f2)
      achados=$(echo "$saida" | grep '^MATCHED_RECORDS=' | cut -d= -f2)

      soma_l=$(echo "$soma_l + $l" | bc -l)
      soma_b=$(echo "$soma_b + $b" | bc -l)
      soma_i=$(echo "$soma_i + $i" | bc -l)
      soma_w=$(echo "$soma_w + $w" | bc -l)
    done

    ml=$(echo "scale=3; $soma_l/$RUNS" | bc -l)
    mb=$(echo "scale=3; $soma_b/$RUNS" | bc -l)
    mi=$(echo "scale=3; $soma_i/$RUNS" | bc -l)
    mw=$(echo "scale=3; $soma_w/$RUNS" | bc -l)
    total=$(echo "scale=3; $ml+$mb+$mi+$mw" | bc -l)

    echo "$LIMIT,$BASE,$RES,$K,$achados,$ml,$mb,$mi,$mw,$total" >> "$OUT"
    echo "OK: $BASE+$RES N=$LIMIT k=$K total=${total}ms"
  done
done