#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pet.h"

void lista_pet_inicializar(ListaPet *l) {
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

NoPet *pet_buscar(ListaPet *l, int codigo) {
    NoPet *atual = l->inicio;
    while (atual != NULL) {
        if (atual->dados.codigo == codigo) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

int pet_inserir(ListaPet *l, pet p, ListaPessoa *pessoas, ListaTipoPet *tipos) {
    if (pet_buscar(l, p.codigo) != NULL) {
        return 0;
    }
    if (pessoa_buscar(pessoas, p.codigo_pes) == NULL) {
        return 0;
    }
    if (tipo_pet_buscar(tipos, p.codigo_tipo) == NULL) {
        return 0;
    }

    NoPet *novo = malloc(sizeof(NoPet));
    if (novo == NULL) {
        return 0;
    }

    novo->dados = p;
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

int pet_remover(ListaPet *l, int codigo) {
    NoPet *alvo = pet_buscar(l, codigo);
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

int pet_tem_dono(ListaPet *l, int codigo_pes) {
    for (NoPet *atual = l->inicio; atual != NULL; atual = atual->prox) {
        if (atual->dados.codigo_pes == codigo_pes) {
            return 1;
        }
    }
    return 0;
}

void lista_pet_liberar(ListaPet *l) {
    NoPet *atual = l->inicio;
    while (atual != NULL) {
        NoPet *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

int pet_salvar(ListaPet *l, const char *arquivo) {
    FILE *f = fopen(arquivo, "w");
    if (f == NULL) {
        return 0;
    }

    for (NoPet *atual = l->inicio; atual != NULL; atual = atual->prox) {
        pet *p = &atual->dados;
        fprintf(f, "%d;%d;%d;%s\n",
                p->codigo, p->codigo_pes, p->codigo_tipo, p->nome);
    }

    fclose(f);
    return 1;
}

int pet_carregar(ListaPet *l, const char *arquivo,
                 ListaPessoa *pessoas, ListaTipoPet *tipos) {
    FILE *f = fopen(arquivo, "r");
    if (f == NULL) {
        return 0;
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), f) != NULL) {
        pet p;

        if (sscanf(linha, "%d;%d;%d;%99[^\n]",
                   &p.codigo, &p.codigo_pes, &p.codigo_tipo, p.nome) != 4) {
            continue;
        }

        pet_inserir(l, p, pessoas, tipos);
    }

    fclose(f);
    return 1;
}