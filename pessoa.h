#ifndef PESSOA_H
#define PESSOA_H

typedef struct {
    int  codigo;            
    char nome[100];         
    char fone[20];
    char endereco[150];
    char data_nascimento[11];
} Pessoa;

typedef struct NoPessoa {
    Pessoa dados;
    struct NoPessoa *ant;   
    struct NoPessoa *prox;  
} NoPessoa;

typedef struct {
    NoPessoa *inicio;
    NoPessoa *fim;
    int tamanho;
} ListaPessoa;

void lista_pessoa_inicializar(ListaPessoa *l);
int  pessoa_inserir(ListaPessoa *l, Pessoa p);
NoPessoa *pessoa_buscar(ListaPessoa *l, int codigo);

int  pessoa_remover(ListaPessoa *l, int codigo);
void lista_pessoa_liberar(ListaPessoa *l);

int pessoa_salvar(ListaPessoa *l, const char *arquivo);
int pessoa_carregar(ListaPessoa *l, const char *arquivo);

#endif