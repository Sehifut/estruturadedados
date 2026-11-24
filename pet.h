#ifndef pet_h
#define pet_h

#include "pessoa.h"
#include "tipo_pet.h"

typedef struct {
    int  codigo;
    int  codigo_pes;            
    int codigo_tipo;
    char nome[100];
    
} pet;

typedef struct NoPet {
    pet dados;
    struct NoPet *ant;
    struct NoPet *prox;
} NoPet;

typedef struct {
    NoPet *inicio;
    NoPet *fim;
    int tamanho;
} ListaPet;

void lista_pet_inicializar(ListaPet *l);
NoPet *pet_buscar(ListaPet *l, int codigo);
int  pet_inserir(ListaPet *l, pet p, ListaPessoa *pessoas, ListaTipoPet *tipos);
int  pet_remover(ListaPet *l, int codigo);
int  pet_tem_dono(ListaPet *l, int codigo_pes);
void lista_pet_liberar(ListaPet *l);
int  pet_salvar(ListaPet *l, const char *arquivo);
int  pet_carregar(ListaPet *l, const char *arquivo,
                  ListaPessoa *pessoas, ListaTipoPet *tipos);

#endif