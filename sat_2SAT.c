#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_VARS 1000

typedef struct {
    int *adj[MAX_VARS * 2];  // Lista de adjacência
    int size[MAX_VARS * 2];  // Tamanho de cada lista de adjacência
} Graph;

Graph graph, rev_graph;
bool visited[MAX_VARS * 2];
int order[MAX_VARS * 2], scc[MAX_VARS * 2], component;
int var_count, clause_count, order_pos;

// Mapeia a variável x para o índice do grafo
int var_index(int x) {
    if (x > 0) return (x - 1) * 2;
    return (-x - 1) * 2 + 1;
}

// Adiciona aresta u -> v no grafo
void add_edge(Graph *g, int u, int v) {
    g->adj[u] = (int*) realloc(g->adj[u], (g->size[u] + 1) * sizeof(int));
    g->adj[u][g->size[u]++] = v;
}

// DFS para encontrar a ordem de processamento
void dfs1(Graph *g, int v) {
    visited[v] = true;
    for (int i = 0; i < g->size[v]; i++) {
        int next = g->adj[v][i];
        if (!visited[next]) dfs1(g, next);
    }
    order[order_pos++] = v;
}

// DFS para marcar componentes fortemente conectadas
void dfs2(Graph *g, int v) {
    visited[v] = true;
    scc[v] = component;
    for (int i = 0; i < g->size[v]; i++) {
        int next = g->adj[v][i];
        if (!visited[next]) dfs2(g, next);
    }
}

// Lê a próxima fórmula do arquivo DIMACS CNF com vírgulas
bool read_cnf(FILE *f) {
    char linha[256];

    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == 'p') {
            sscanf(linha, "p cnf %d %d", &var_count, &clause_count);
            break;
        }
    }

    if (feof(f)) return false;

    // Inicializa os grafos
    for (int i = 0; i < 2 * var_count; i++) {
        graph.size[i] = rev_graph.size[i] = 0;
        graph.adj[i] = rev_graph.adj[i] = NULL;
    }

    // Lê as cláusulas e adiciona as arestas
    for (int i = 0; i < clause_count; i++) {
        if (!fgets(linha, sizeof(linha), f)) break;

        int x, y;
        sscanf(linha, "%d , %d , 0", &x, &y);

        add_edge(&graph, var_index(-x), var_index(y));  // ~x -> y
        add_edge(&graph, var_index(-y), var_index(x));  // ~y -> x
        add_edge(&rev_graph, var_index(y), var_index(-x));  // y -> ~x
        add_edge(&rev_graph, var_index(x), var_index(-y));  // x -> ~y
    }

    return true;
}

// Executa o algoritmo de Kosaraju para encontrar SCCs
void kosaraju() {
    order_pos = 0;
    component = 0;

    // Passo 1: DFS no grafo original para encontrar a ordem
    for (int i = 0; i < 2 * var_count; i++) visited[i] = false;
    for (int i = 0; i < 2 * var_count; i++) {
        if (!visited[i]) dfs1(&graph, i);
    }

    // Passo 2: DFS no grafo transposto para encontrar as SCCs
    for (int i = 0; i < 2 * var_count; i++) visited[i] = false;
    for (int i = order_pos - 1; i >= 0; i--) {
        int v = order[i];
        if (!visited[v]) {
            dfs2(&rev_graph, v);
            component++;
        }
    }
}

// Verifica se a fórmula 2-SAT é satisfatível
bool is_satisfiable() {
    for (int i = 0; i < var_count; i++) {
        if (scc[i * 2] == scc[i * 2 + 1]) return false;
    }
    return true;
}

// Função principal
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_dimacs>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    int formula_num = 1;
    clock_t tempo_total_inicio = clock();  // Inicia a contagem do tempo total

    while (read_cnf(f)) {
        printf("Processando fórmula %d...\n", formula_num);

        clock_t inicio = clock();  // Inicia a medição do tempo da fórmula

        // Executa o algoritmo de Kosaraju para descobrir SCCs
        kosaraju();

        bool resultado = is_satisfiable();
        clock_t fim = clock();  // Finaliza a medição do tempo da fórmula

        double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

        if (resultado) {
            printf("Fórmula %d: SATISFATÍVEL\n", formula_num);
        } else {
            printf("Fórmula %d: INSATISFATÍVEL\n", formula_num);
        }

        printf("Tempo de execução para a fórmula %d: %f segundos\n\n", formula_num, tempo_execucao);
        formula_num++;
    }

    fclose(f);

    // Tempo total de execução
    clock_t tempo_total_fim = clock();
    double tempo_total_execucao = (double)(tempo_total_fim - tempo_total_inicio) / CLOCKS_PER_SEC;
    printf("Tempo total de execução: %f segundos\n", tempo_total_execucao);

    return 0;
}
