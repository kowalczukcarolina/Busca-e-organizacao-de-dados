# Busca-e-organizacao-de-dados
Implementação em C de estruturas de dados (TADs) para busca, organização e filtragem de datasets (Nasa Asteroids &amp; Dados Abertos). Projeto 1 da disciplina SCC0202 - Algoritmos e Estruturas de Dados I (ICMC USP).


## Sobre o projeto
Foi implementado um programa em C capaz de carregar, buscar e organizar registros usando diferentes estruturas de dados. O usuário pode escolher a estrutura que armazenará todo o dataset e
a estrutura que armazenará os resultados da busca. Na primeira implementação, utilizamos um dataset de registros de aproximação de asteroides fornecido pelo professor da disciplina nas especificações do projeto. Na segunda implementação, escolhemos o dataset de [Chamados de Tecnologia da Informação do MDHC, MIR e MMulheres](https://dados.gov.br/dados/conjuntos-dados/chamados-de-tecnologia-da-informacao-do-mdhc-mir-e-mmulheres), que contém dados sobre os chamados atendidos pela central de serviços de TI e foi encontrados através do site de Dados Abertos do governo. Optamos pela escolha do segundo dataset por se aproximar da nossa área de atuação, considerando que todos os autores do programa cursam Ciência da Computação no ICMC-USP.


## Autores
* Julia Barbosa Nogueira - 17901347 - [Github Julia Nogueira](https://github.com/juliab2nogueira)
* Carolina Goulart Kowalczuk - 13854629 - [Github Carolina Kowalczuk](https://github.com/kowalczukcarolina)
* Victor Hugo Adão de Oliveira - 17969072 - [Github Victor Adão](https://github.com/VihAdao)

## Estruturas de dados implementadas
* Pilha
* Fila
* Lista Sequencial
* Lista Encadeada
* Lista com Cabeça (Sentinela)
* Lista Ordenada
* Lista Generalizada (Árvore/Subgrupos)
* Lista Cruzada (Múltiplos ponteiros sem duplicação)

## Diretórios do projeto:

* dados/ : Guardar os datasets de entrada em formato .csv
* consultas/ : Guardar os ficheiros de texto (.txt) com as regras de busca (os comandos FILTER, SIM, ORDER, etc.)
* src/ : Código-fonte (main.c, reader_nada.c, etc)
* include/ e include/tads/ : arquivos .h (headers)



## Execução do programa

| Argumento | Função |
| --- | --- |
| -reader| leitor do dataset utilizado |
| –input | arquivo de entrada |
|–base | estrutura usada para armazenar o dataset completo |
|–result| estrutura usada para armazenar o resultado da busca |
|–query | arquivo contendo os critérios da busca |
|–output | arquivo que receberá os registros encontrados |
|–top-k | máximo de registros exportados; 0 significa todos |
| –limit | máximo de registros carregados; 0 significa toda a base |

## Consulta e saída 
A consulta é fornecida em arquivo texto.

Os comandos têm as seguintes funções:

* FILTER: condição obrigatória para o registro ser aceito;
* SIM: critério numérico de similaridade, no formato campo|alvo|tolerancia|peso;
* ACCEPT: limiar mínimo para a similaridade total;
* ORDER: chave usada pela lista ordenada;
* GROUP: campos usados pela lista generalizada;
* CROSS: campos usados pela lista cruzada.
