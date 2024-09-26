# JassonMarcosLeonardo_FinalProject_AA_RR_2024

# Projeto Solver SAT em C

## Descrição

Este projeto consiste na implementação de dois solvers SAT em C para determinar a satisfatibilidade de fórmulas proposicionais bem-formadas (wffs) no formato DIMACS CNF. O projeto inclui:

1. **Solver por Geração de Atribuições**: Gera todas as atribuições possíveis para as variáveis e verifica se alguma delas satisfaz a fórmula.
2. **Solver com Backtracking Simples**: Utiliza a técnica de backtracking para encontrar uma solução satisfatória para a fórmula, explorando atribuições de forma mais eficiente.

## Funcionalidades

- **Entrada**: As fórmulas devem ser fornecidas no formato DIMACS CNF.
- **Saída**: O programa imprime o tempo de execução e se a fórmula é satisfatível ou insatisfatível.
- **Prints**: Exibe um print no início da execução e um no final com o tempo total de execução.

## Estrutura

- `sat_solver.c`: Implementa a solução utilizando geração de todas as atribuições possíveis.
- `sat_solver_2.c`: Implementa a solução utilizando backtracking.
- `sat_2SAT.c`: Solucionador para apenas 2SAT usando grafos que faz melhor que O(2^v)
  
## Compilação e Execução

### Compilar:

```bash
gcc sat_solver.c -o sat_solver
gcc sat_solver_2.c -o sat_solver_2


