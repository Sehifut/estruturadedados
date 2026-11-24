#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "pessoa.h"
#include "tipo_pet.h"
#include "pet.h"

int executar_pessoa(ListaPessoa *pessoas, ListaPet *pets, const char *comando);
int executar_tipo(ListaTipoPet *tipos, ListaPet *pets, const char *comando);
int executar_pet(ListaPet *pets, ListaPessoa *pessoas,
                 ListaTipoPet *tipos, const char *comando);

#endif