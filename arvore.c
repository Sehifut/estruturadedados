#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"

NoArvore *arvore_inserir(NoArvore *raiz, Pessoa p) {
    if (raiz == NULL) {                       
        NoArvore *novo = malloc(sizeof(NoArvore));
        if (novo == NULL) return NULL;
        novo->dados = p;
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

   
    if (strcmp(p.nome, raiz->dados.nome) < 0) {
        raiz->esq = arvore_inserir(raiz->esq, p);
    } else {
        raiz->dir = arvore_inserir(raiz->dir, p);
    }
    return raiz;
}

void arvore_em_ordem(NoArvore *raiz) {
    if (raiz == NULL) return;
    arvore_em_ordem(raiz->esq);               
    printf("%d;%s;%s;%s;%s\n",               
           raiz->dados.codigo, raiz->dados.nome, raiz->dados.fone,
           raiz->dados.endereco, raiz->dados.data_nascimento);
    arvore_em_ordem(raiz->dir);              
}

void arvore_liberar(NoArvore *raiz) {
    if (raiz == NULL) return;
    arvore_liberar(raiz->esq);               
    arvore_liberar(raiz->dir);
    free(raiz);
}
NoArvorePet *arvore_pet_inserir(NoArvorePet *raiz, pet p) {
    if (raiz == NULL) {
        NoArvorePet *novo = malloc(sizeof(NoArvorePet));
        if (novo == NULL) return NULL;
        novo->dados = p;
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    if (strcmp(p.nome, raiz->dados.nome) < 0) {
        raiz->esq = arvore_pet_inserir(raiz->esq, p);
    } else {
        raiz->dir = arvore_pet_inserir(raiz->dir, p);
    }
    return raiz;
}

void arvore_pet_em_ordem(NoArvorePet *raiz) {
    if (raiz == NULL) return;
    arvore_pet_em_ordem(raiz->esq);
    printf("%d;%d;%d;%s\n",
           raiz->dados.codigo, raiz->dados.codigo_pes,
           raiz->dados.codigo_tipo, raiz->dados.nome);
    arvore_pet_em_ordem(raiz->dir);
}

void arvore_pet_liberar(NoArvorePet *raiz) {
    if (raiz == NULL) return;
    arvore_pet_liberar(raiz->esq);
    arvore_pet_liberar(raiz->dir);
    free(raiz);
}