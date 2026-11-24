#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"

void lista_pessoa_inicializar(ListaPessoa *l) {
    l->inicio = NULL;   
    l->fim = NULL;
    l->tamanho = 0;
}

int pessoa_inserir(ListaPessoa *l, Pessoa p) {
    if (pessoa_buscar(l, p.codigo) != NULL) {
        return 0;   
    }

    NoPessoa *novo = malloc(sizeof(NoPessoa));  
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
NoPessoa *pessoa_buscar(ListaPessoa *l, int codigo) {
    NoPessoa *atual = l->inicio;
   while (atual != NULL){
        if (atual->dados.codigo == codigo){
            return atual;
        }
        atual = atual->prox;
}

return NULL;
}

int pessoa_remover(ListaPessoa *l, int codigo) {
    NoPessoa *alvo = pessoa_buscar(l, codigo);
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

void lista_pessoa_liberar(ListaPessoa *l) {
    NoPessoa *atual = l->inicio;
    while (atual != NULL) {
        NoPessoa *proximo = atual->prox;  
        free(atual);
        atual = proximo;
    }
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}
static const char *campo_ou_traco(const char *s) {
    return (s[0] == '\0') ? "-" : s;
}

int pessoa_salvar(ListaPessoa *l, const char *arquivo) {
    FILE *f = fopen(arquivo, "w");
    if (f == NULL) {
        return 0;
    }

    for (NoPessoa *atual = l->inicio; atual != NULL; atual = atual->prox) {
        Pessoa *p = &atual->dados;
        fprintf(f, "%d;%s;%s;%s;%s\n",
                p->codigo,
                campo_ou_traco(p->nome),
                campo_ou_traco(p->fone),
                campo_ou_traco(p->endereco),
                campo_ou_traco(p->data_nascimento));
    }

    fclose(f);
    return 1;
}

int pessoa_carregar(ListaPessoa *l, const char *arquivo) {
    FILE *f = fopen(arquivo, "r");
    if (f == NULL) {
        return 0; 
    }

    char linha[400];
    while (fgets(linha, sizeof(linha), f) != NULL) {
        Pessoa p;
        char nome[100], fone[20], endereco[150], nasc[11];

    
        if (sscanf(linha, "%d;%99[^;];%19[^;];%149[^;];%10[^\n]",
                   &p.codigo, nome, fone, endereco, nasc) != 5) {
            continue; 
        }

        strcpy(p.nome, strcmp(nome, "-") == 0 ? "" : nome);
        strcpy(p.fone, strcmp(fone, "-") == 0 ? "" : fone);
        strcpy(p.endereco, strcmp(endereco, "-") == 0 ? "" : endereco);
        strcpy(p.data_nascimento, strcmp(nasc, "-") == 0 ? "" : nasc);

        pessoa_inserir(l, p); 
    }

    fclose(f);
    return 1;
}