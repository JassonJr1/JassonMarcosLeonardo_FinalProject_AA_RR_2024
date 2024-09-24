#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

typedef struct {
    int num_vars;     // Número de variáveis
    int num_clauses;  // Número de cláusulas
    int **clauses;    // Matriz para armazenar as cláusulas
} CNF;

// Função para liberar memória do CNF
void liberar_cnf(CNF* cnf) {
    for (int i = 0; i < cnf->num_clauses; i++) {
        free(cnf->clauses[i]);
    }
    free(cnf->clauses);
    free(cnf);
}

// Função para remover literais duplicados em uma cláusula
void remover_duplicados(int* clausula, int* tamanho) {
    for (int i = 0; i < *tamanho; i++) {
        if (clausula[i] == 0) break; // Fim da cláusula
        for (int j = i + 1; j < *tamanho; j++) {
            if (clausula[j] == 0) break;
            if (clausula[i] == clausula[j]) {
                // Se encontrar duplicado, shift para a esquerda para remover
                for (int k = j; k < *tamanho - 1; k++) {
                    clausula[k] = clausula[k + 1];
                }
                clausula[*tamanho - 1] = 0; // Ajustar o final
                (*tamanho)--; // Reduzir o tamanho
                j--; // Reavaliar a nova posição
            }
        }
    }
}

// Função para ler a próxima fórmula do arquivo DIMACS CNF com vírgulas
CNF* ler_proxima_formula(FILE* f) {
    CNF* cnf = (CNF*)malloc(sizeof(CNF));
    char linha[256];
    
    // Ignorar comentários e procurar pela linha 'p cnf'
    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == 'p') {
            sscanf(linha, "p cnf %d %d", &cnf->num_vars, &cnf->num_clauses);
            break;
        }
    }

    // Se não encontrou uma fórmula (EOF), retorna NULL
    if (feof(f)) {
        free(cnf);
        return NULL;
    }

    // Alocar memória para as cláusulas
    cnf->clauses = (int**)malloc(cnf->num_clauses * sizeof(int*));
    for (int i = 0; i < cnf->num_clauses; i++) {
        cnf->clauses[i] = (int*)malloc((cnf->num_vars + 1) * sizeof(int));
    }

    // Ler as cláusulas
    int i = 0;
    while (fgets(linha, sizeof(linha), f) && i < cnf->num_clauses) {
        if (linha[0] == 'c' || linha[0] == 'p') continue;

        int literal, j = 0;
        // Modificar o delimitador para aceitar vírgulas e espaços
        char* token = strtok(linha, ", ");
        while (token != NULL) {
            literal = atoi(token);
            if (literal != 0) {
                cnf->clauses[i][j++] = literal;
            }
            token = strtok(NULL, ", ");
        }
        cnf->clauses[i][j] = 0;  // Terminador da cláusula

        // Remover duplicados da cláusula
        int tamanho = j;  // Tamanho atual da cláusula
        remover_duplicados(cnf->clauses[i], &tamanho);
        cnf->clauses[i][tamanho] = 0;  // Atualizar o final
        i++;
    }

    return cnf;
}

// Função para verificar se a atribuição atual satisfaz todas as cláusulas
bool satisfaz(CNF* cnf, int* atribuicao) {
    for (int i = 0; i < cnf->num_clauses; i++) {
        bool clausula_satisfeita = false;
        for (int j = 0; cnf->clauses[i][j] != 0; j++) {
            int literal = cnf->clauses[i][j];
            if ((literal > 0 && atribuicao[literal - 1] == 1) ||
                (literal < 0 && atribuicao[-literal - 1] == 0)) {
                clausula_satisfeita = true;
                break;
            }
        }
        if (!clausula_satisfeita) return false;
    }
    return true;
}

// Função de backtracking para verificar a satisfatibilidade
bool backtracking(CNF* cnf, int* atribuicao, int var) {
    if (var == cnf->num_vars) {
        return satisfaz(cnf, atribuicao);
    }

    // Tentar atribuir 0 à variável atual
    atribuicao[var] = 0;
    if (backtracking(cnf, atribuicao, var + 1)) {
        return true;
    }

    // Tentar atribuir 1 à variável atual
    atribuicao[var] = 1;
    if (backtracking(cnf, atribuicao, var + 1)) {
        return true;
    }

    return false;
}

// Função para verificar a satisfatibilidade usando backtracking
bool verifica_satisfatibilidade_backtracking(CNF* cnf) {
    int* atribuicao = (int*)calloc(cnf->num_vars, sizeof(int));
    bool resultado = backtracking(cnf, atribuicao, 0);
    free(atribuicao);
    return resultado;
}

// Função principal
int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_dimacs>\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (!f) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    int formula_num = 1;
    CNF* cnf;
    double tempo_total = 0.0;  // Variável para acumular o tempo total

    while ((cnf = ler_proxima_formula(f)) != NULL) {
        printf("Processando fórmula %d...\n", formula_num);

        clock_t inicio = clock();  // Inicia a medição do tempo

        bool resultado = verifica_satisfatibilidade_backtracking(cnf);

        clock_t fim = clock();  // Finaliza a medição do tempo
        double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;
        tempo_total += tempo_execucao;  // Acumula o tempo de execução

        if (resultado) {
            printf("Fórmula %d: SATISFATÍVEL\n", formula_num);
        } else {
            printf("Fórmula %d: INSATISFATÍVEL\n", formula_num);
        }

        printf("Tempo de execução da fórmula %d: %f segundos\n\n", formula_num, tempo_execucao);

        liberar_cnf(cnf);
        formula_num++;
    }

    printf("Tempo total de execução de todas as fórmulas: %f segundos\n", tempo_total);

    fclose(f);
    return 0;
}
