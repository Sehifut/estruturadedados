#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipo_pet.h"

void lista_tipo_pet_inicializar(ListaTipoPet *l) {
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

NoTipoPet *tipo_pet_buscar(ListaTipoPet *l, int codigo) {
    NoTipoPet *atual = l->inicio;
    while (atual != NULL) {
        if (atual->dados.codigo == codigo) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

int tipo_pet_inserir(ListaTipoPet *l, tipo_pet t) {
    if (tipo_pet_buscar(l, t.codigo) != NULL) {
        return 0;
    }

    NoTipoPet *novo = malloc(sizeof(NoTipoPet));
    if (novo == NULL) {
        return 0;
    }

    novo->dados = t;
    novo->prox = NULL;
    novo->ant = l->fim;

    if (l->fim == NULL) {
        l->inicio = novo;
    } else {
        l->fim->prox = novo;
    }

    l->fim = novo;
    l->tamanho++;
    return 1;
}

int tipo_pet_remover(ListaTipoPet *l, int codigo) {
    NoTipoPet *alvo = tipo_pet_buscar(l, codigo);
    if (alvo == NULL) {
        return 0;
    }

    if (alvo->ant != NULL) {
        alvo->ant->prox = alvo->prox;
    } else {
        l->inicio = alvo->prox;
    }

    if (alvo->prox != NULL) {
        alvo->prox->ant = alvo->ant;
    } else {
        l->fim = alvo->ant;
    }

    free(alvo);
    l->tamanho--;
    return 1;
}

void lista_tipo_pet_liberar(ListaTipoPet *l) {
    NoTipoPet *atual = l->inicio;
    while (atual != NULL) {
        NoTipoPet *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

int tipo_pet_salvar(ListaTipoPet *l, const char *arquivo) {
    FILE *f = fopen(arquivo, "w");
    if (f == NULL) {
        return 0;
    }

    for (NoTipoPet *atual = l->inicio; atual != NULL; atual = atual->prox) {
        fprintf(f, "%d;%s\n", atual->dados.codigo, atual->dados.descricao);
    }

    fclose(f);
    return 1;
}

int tipo_pet_carregar(ListaTipoPet *l, const char *arquivo) {
    FILE *f = fopen(arquivo, "r");
    if (f == NULL) {
        return 0;
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), f) != NULL) {
        tipo_pet t;
        char desc[100];

        if (sscanf(linha, "%d;%99[^\n]", &t.codigo, desc) != 2) {
            continue;
        }

        strcpy(t.descricao, desc);
        tipo_pet_inserir(l, t);
    }

    fclose(f);
    return 1;
}