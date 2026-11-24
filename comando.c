#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comando.h"

void fila_inicializar(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

int fila_vazia(Fila *f) {
    return f->inicio == NULL;
}

int fila_enfileirar(Fila *f, const char *descricao) {
    Comando *novo = malloc(sizeof(Comando));
    if (novo == NULL) {
        return 0;
    }

    strncpy(novo->descricao, descricao, sizeof(novo->descricao) - 1);
    novo->descricao[sizeof(novo->descricao) - 1] = '\0';  
    novo->prox = NULL;

    if (f->fim == NULL) {
        f->inicio = novo;    
    } else {
        f->fim->prox = novo;
    }
    f->fim = novo;
    f->tamanho++;
    return 1;
}

Comando *fila_desenfileirar(Fila *f) {
    if (fila_vazia(f)) {
        return NULL;
    }

    Comando *removido = f->inicio;
    f->inicio = removido->prox;
    if (f->inicio == NULL) {
        f->fim = NULL;        
    }
    f->tamanho--;
    removido->prox = NULL;
    return removido;
}

void fila_liberar(Fila *f) {
    Comando *c;
    while ((c = fila_desenfileirar(f)) != NULL) {
        free(c);
    }
}



static int comeca_com(const char *s, const char *prefixo) {
    return strncmp(s, prefixo, strlen(prefixo)) == 0;
}

static int contem(const char *s, const char *trecho) {
    return strstr(s, trecho) != NULL;
}

static int tabela_valida(const char *nome) {
    return strcmp(nome, "pessoa") == 0
        || strcmp(nome, "tipo_pet") == 0
        || strcmp(nome, "pet") == 0;
}

int comando_valido(const char *linha) {
    char s[300];
    char tabela[32];

    
    strncpy(s, linha, sizeof(s) - 1);
    s[sizeof(s) - 1] = '\0';
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ')) {
        s[--n] = '\0';
    }

   
    if (n == 0 || s[n - 1] != ';') {
        return 0;
    }

    
    if (comeca_com(s, "insert into ")) {
        if (sscanf(s, "insert into %31[a-z_]", tabela) != 1) return 0;
        return tabela_valida(tabela) && contem(s, "(") && contem(s, "values(");
    }

    /* update tabela set ... where ...; */
    if (comeca_com(s, "update ")) {
        if (sscanf(s, "update %31[a-z_]", tabela) != 1) return 0;
        return tabela_valida(tabela) && contem(s, " set ") && contem(s, " where ");
    }

    /* delete from tabela where ...; */
    if (comeca_com(s, "delete from ")) {
        if (sscanf(s, "delete from %31[a-z_]", tabela) != 1) return 0;
        return tabela_valida(tabela) && contem(s, " where ");
    }

    /* select * from tabela [where ...] [order by ...]; */
    if (comeca_com(s, "select * from ")) {
        if (sscanf(s, "select * from %31[a-z_]", tabela) != 1) return 0;
        return tabela_valida(tabela);
    }

    return 0;   /* não começa com nenhum comando conhecido */
}