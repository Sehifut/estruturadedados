#ifndef ARVORE_H
#define ARVORE_H

#include "pessoa.h"

typedef struct NoArvore {
    Pessoa dados;
    struct NoArvore *esq;
    struct NoArvore *dir;
} NoArvore;

NoArvore *arvore_inserir(NoArvore *raiz, Pessoa p);  
void      arvore_em_ordem(NoArvore *raiz);        
void      arvore_liberar(NoArvore *raiz);

#endif