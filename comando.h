#ifndef COMANDO_H
#define COMANDO_H

typedef struct Comando {
    char descricao[300];     
    struct Comando *prox;
} Comando;

typedef struct {
    Comando *inicio;
    Comando *fim;
    int tamanho;
} Fila;

void     fila_inicializar(Fila *f);
int      fila_vazia(Fila *f);
int      fila_enfileirar(Fila *f, const char *descricao);
Comando *fila_desenfileirar(Fila *f);   
void     fila_liberar(Fila *f);
int      comando_valido(const char *linha);

#endif