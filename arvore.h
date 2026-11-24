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

#include "pet.h"

typedef struct NoArvorePet {
    pet dados;
    struct NoArvorePet *esq;
    struct NoArvorePet *dir;
} NoArvorePet;

NoArvorePet *arvore_pet_inserir(NoArvorePet *raiz, pet p);
void         arvore_pet_em_ordem(NoArvorePet *raiz);
void         arvore_pet_liberar(NoArvorePet *raiz);
#endif