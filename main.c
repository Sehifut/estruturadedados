#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"
#include "tipo_pet.h"
#include "pet.h"
#include "comando.h"
#include "executor.h"

#define ARQ_PESSOAS "pessoas.txt"
#define ARQ_TIPOS   "tipos_pet.txt"
#define ARQ_PETS    "pets.txt"

typedef enum { ENT_NENHUMA, ENT_PESSOA, ENT_TIPO, ENT_PET } Entidade;

static Entidade entidade_do_comando(const char *s) {
    const char *p;
    char tabela[32];

    if (strncmp(s, "insert into ", 12) == 0)          p = s + 12;
    else if (strncmp(s, "update ", 7) == 0)          p = s + 7;
    else if (strncmp(s, "delete from ", 12) == 0)     p = s + 12;
    else if (strncmp(s, "select * from ", 14) == 0)   p = s + 14;
    else return ENT_NENHUMA;

    if (sscanf(p, "%31[a-z_]", tabela) != 1) return ENT_NENHUMA;

    if (strcmp(tabela, "pessoa") == 0)   return ENT_PESSOA;
    if (strcmp(tabela, "tipo_pet") == 0) return ENT_TIPO;
    if (strcmp(tabela, "pet") == 0)      return ENT_PET;
    return ENT_NENHUMA;
}

static void tirar_quebra(char *s) {
    s[strcspn(s, "\r\n")] = '\0';
}

static void carregar_script(Fila *entrada) {
    char linha[320];
    char nome[260];

    printf("1 - ler script de arquivo\n2 - digitar script\nEscolha: ");
    if (fgets(linha, sizeof(linha), stdin) == NULL) return;

    if (linha[0] == '1') {
        printf("Nome do arquivo: ");
        if (fgets(nome, sizeof(nome), stdin) == NULL) return;
        tirar_quebra(nome);

        FILE *f = fopen(nome, "r");
        if (f == NULL) {
            printf("Não foi possível abrir %s\n", nome);
            return;
        }
        while (fgets(linha, sizeof(linha), f) != NULL) {
            tirar_quebra(linha);
            if (linha[0] == '\0') continue;
            fila_enfileirar(entrada, linha);
        }
        fclose(f);
    } else {
        printf("Digite os comandos (linha vazia para terminar):\n");
        while (fgets(linha, sizeof(linha), stdin) != NULL) {
            tirar_quebra(linha);
            if (linha[0] == '\0') break;
            fila_enfileirar(entrada, linha);
        }
    }
}

int main(void) {
    ListaPessoa pessoas;
    ListaTipoPet tipos;
    ListaPet pets;
    Fila entrada, fila_pessoa, fila_tipo, fila_pet;
    Comando *c;

    lista_pessoa_inicializar(&pessoas);
    lista_tipo_pet_inicializar(&tipos);
    lista_pet_inicializar(&pets);
    fila_inicializar(&entrada);
    fila_inicializar(&fila_pessoa);
    fila_inicializar(&fila_tipo);
    fila_inicializar(&fila_pet);

    pessoa_carregar(&pessoas, ARQ_PESSOAS);
    tipo_pet_carregar(&tipos, ARQ_TIPOS);
    pet_carregar(&pets, ARQ_PETS, &pessoas, &tipos);
    printf("Carregados: %d pessoa(s), %d tipo(s), %d pet(s)\n\n",
           pessoas.tamanho, tipos.tamanho, pets.tamanho);

    carregar_script(&entrada);

    while ((c = fila_desenfileirar(&entrada)) != NULL) {
        if (!comando_valido(c->descricao)) {
            printf("Ignorado (inválido): %s\n", c->descricao);
        } else {
            switch (entidade_do_comando(c->descricao)) {
                case ENT_PESSOA: fila_enfileirar(&fila_pessoa, c->descricao); break;
                case ENT_TIPO:   fila_enfileirar(&fila_tipo, c->descricao);   break;
                case ENT_PET:    fila_enfileirar(&fila_pet, c->descricao);    break;
                default:
                    printf("Ignorado (tabela desconhecida): %s\n", c->descricao);
            }
        }
        free(c);
    }

    while ((c = fila_desenfileirar(&fila_pessoa)) != NULL) {
        executar_pessoa(&pessoas, &pets, c->descricao);
        free(c);
    }
    while ((c = fila_desenfileirar(&fila_tipo)) != NULL) {
        executar_tipo(&tipos, &pets, c->descricao);
        free(c);
    }
    while ((c = fila_desenfileirar(&fila_pet)) != NULL) {
        executar_pet(&pets, &pessoas, &tipos, c->descricao);
        free(c);
    }

    pessoa_salvar(&pessoas, ARQ_PESSOAS);
    tipo_pet_salvar(&tipos, ARQ_TIPOS);
    pet_salvar(&pets, ARQ_PETS);
    printf("\nDados salvos.\n");

    lista_pessoa_liberar(&pessoas);
    lista_tipo_pet_liberar(&tipos);
    lista_pet_liberar(&pets);
    fila_liberar(&entrada);
    fila_liberar(&fila_pessoa);
    fila_liberar(&fila_tipo);
    fila_liberar(&fila_pet);

    return 0;
}