#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "executor.h"
#include "arvore.h"

#define OBRIG_CODIGO 1
#define OBRIG_NOME   2
#define OBRIG_NASC   4

static int proximo_valor(const char **p, char *saida, size_t tam) {
    const char *s = *p;
    size_t i = 0;

    while (*s == ' ') s++;
    if (*s == '\0') return 0;

    if (*s == '\'') {        
        s++;
        while (*s != '\0' && *s != '\'' && i < tam - 1) {
            saida[i++] = *s++;
        }
        if (*s != '\'') return -1;             
        s++;                                   
    } else {                                   
        while (*s != '\0' && *s != ',' && i < tam - 1) {
            saida[i++] = *s++;
        }
        while (i > 0 && saida[i - 1] == ' ') i--;  
    }
    saida[i] = '\0';

    while (*s == ' ') s++;
    if (*s == ',') {
        s++;
    } else if (*s != '\0') {
        return -1;                             
    }
    *p = s;
    return 1;
}


static int data_valida(const char *d) {
    int dia, mes, ano;

    if (strlen(d) != 10) return 0;
    if (d[2] != '/' || d[5] != '/') return 0;
    if (sscanf(d, "%2d/%2d/%4d", &dia, &mes, &ano) != 3) return 0;

    return dia >= 1 && dia <= 31 && mes >= 1 && mes <= 12 && ano >= 1900;
}


static int atribuir_pessoa(Pessoa *p, int *obrig, const char *col, const char *val) {
    if (strcmp(col, "codigo") == 0) {
        if (sscanf(val, "%d", &p->codigo) != 1) return 0;
        *obrig |= OBRIG_CODIGO;
    } else if (strcmp(col, "nome") == 0) {
        snprintf(p->nome, sizeof p->nome, "%s", val);
        *obrig |= OBRIG_NOME;
    } else if (strcmp(col, "fone") == 0) {
        snprintf(p->fone, sizeof p->fone, "%s", val);
    } else if (strcmp(col, "endereco") == 0) {
        snprintf(p->endereco, sizeof p->endereco, "%s", val);
    } else if (strcmp(col, "data_nascimento") == 0) {
        if (!data_valida(val)) return 0;
        snprintf(p->data_nascimento, sizeof p->data_nascimento, "%s", val);
        *obrig |= OBRIG_NASC;
    } else {
        return 0;                              
    }
    return 1;
}

static int executar_insert_pessoa(ListaPessoa *pessoas, const char *cmd) {
   
    const char *ini_col = strchr(cmd, '(');
    const char *ini_val = strstr(cmd, "values(");
    const char *fim_val = strrchr(cmd, ')');
    if (ini_col == NULL || ini_val == NULL || fim_val == NULL) return 0;

    const char *fim_col = strchr(ini_col, ')');
    if (fim_col == NULL || fim_col > ini_val) return 0;

    
    char colunas[300], valores[300];
    size_t n_col = (size_t)(fim_col - ini_col - 1);
    size_t n_val = (size_t)(fim_val - (ini_val + 7));   
    if (n_col >= sizeof colunas || n_val >= sizeof valores) return 0;

    memcpy(colunas, ini_col + 1, n_col);
    colunas[n_col] = '\0';
    memcpy(valores, ini_val + 7, n_val);
    valores[n_val] = '\0';

    Pessoa p;
    memset(&p, 0, sizeof p);
    int obrig = 0;
    const char *pc = colunas;
    const char *pv = valores;
    char col[64], val[300];

    while (1) {
        int rc = proximo_valor(&pc, col, sizeof col);
        int rv = proximo_valor(&pv, val, sizeof val);
        if (rc == -1 || rv == -1) return 0;
        if (rc == 0 && rv == 0) break;          
        if (rc != rv) return 0;                 
        if (!atribuir_pessoa(&p, &obrig, col, val)) return 0;
    }

    if (obrig != (OBRIG_CODIGO | OBRIG_NOME | OBRIG_NASC)) return 0;

    return pessoa_inserir(pessoas, p);
}

static int executar_insert_tipo(ListaTipoPet *tipos, const char *cmd) {
    const char *ini_col = strchr(cmd, '(');
    const char *ini_val = strstr(cmd, "values(");
    const char *fim_val = strrchr(cmd, ')');
    if (ini_col == NULL || ini_val == NULL || fim_val == NULL) return 0;

    const char *fim_col = strchr(ini_col, ')');
    if (fim_col == NULL || fim_col > ini_val) return 0;

    char colunas[300], valores[300];
    size_t n_col = (size_t)(fim_col - ini_col - 1);
    size_t n_val = (size_t)(fim_val - (ini_val + 7));
    if (n_col >= sizeof colunas || n_val >= sizeof valores) return 0;

    memcpy(colunas, ini_col + 1, n_col);
    colunas[n_col] = '\0';
    memcpy(valores, ini_val + 7, n_val);
    valores[n_val] = '\0';

    tipo_pet t;
    memset(&t, 0, sizeof t);
    int obrig = 0;
    const char *pc = colunas;
    const char *pv = valores;
    char col[64], val[300];

    while (1) {
        int rc = proximo_valor(&pc, col, sizeof col);
        int rv = proximo_valor(&pv, val, sizeof val);
        if (rc == -1 || rv == -1) return 0;
        if (rc == 0 && rv == 0) break;
        if (rc != rv) return 0;

        if (strcmp(col, "codigo") == 0) {
            if (sscanf(val, "%d", &t.codigo) != 1) return 0;
            obrig |= OBRIG_CODIGO;
        } else if (strcmp(col, "descricao") == 0) {
            snprintf(t.descricao, sizeof t.descricao, "%s", val);
            obrig |= OBRIG_NOME;  
        } else {
            return 0;
        }
    }

    if (obrig != (OBRIG_CODIGO | OBRIG_NOME)) return 0;
    return tipo_pet_inserir(tipos, t);
}
static int parse_where_codigo(const char *cmd, int *valor) {
    const char *w = strstr(cmd, " where ");
    if (w == NULL) return 0;

    const char *p = w + 7;                
    while (*p == ' ') p++;
    if (strncmp(p, "codigo", 6) != 0) return 0;
    p += 6;
    while (*p == ' ') p++;
    if (*p != '=') return 0;
    p++;

    return sscanf(p, "%d", valor) == 1;
}
static int tipo_em_uso(ListaPet *pets, int codigo_tipo) {
    for (NoPet *a = pets->inicio; a != NULL; a = a->prox) {
        if (a->dados.codigo_tipo == codigo_tipo) return 1;
    }
    return 0;
}

static int executar_delete_pessoa(ListaPessoa *pessoas, ListaPet *pets,
                                  const char *cmd) {
    int cod;
    if (!parse_where_codigo(cmd, &cod)) return 0;
    if (pessoa_buscar(pessoas, cod) == NULL) return 0;  
    if (pet_tem_dono(pets, cod)) return 0;             
    return pessoa_remover(pessoas, cod);
}

static int executar_delete_tipo(ListaTipoPet *tipos, ListaPet *pets,
                                const char *cmd) {
    int cod;
    if (!parse_where_codigo(cmd, &cod)) return 0;
    if (tipo_pet_buscar(tipos, cod) == NULL) return 0;
    if (tipo_em_uso(pets, cod)) return 0;               
    return tipo_pet_remover(tipos, cod);
}

static int executar_delete_pet(ListaPet *pets, const char *cmd) {
    int cod;
    if (!parse_where_codigo(cmd, &cod)) return 0;
    return pet_remover(pets, cod);
}
static NoArvore *montar_arvore_pessoas(ListaPessoa *pessoas) {
    NoArvore *raiz = NULL;
    for (NoPessoa *a = pessoas->inicio; a != NULL; a = a->prox) {
        raiz = arvore_inserir(raiz, a->dados);
    }
    return raiz;
}

static void executar_select_pessoa(ListaPessoa *pessoas, const char *cmd) {
    printf("codigo;nome;fone;endereco;nascimento\n");

    if (strstr(cmd, "order by nome") != NULL) {
        NoArvore *raiz = montar_arvore_pessoas(pessoas);
        arvore_em_ordem(raiz);
        arvore_liberar(raiz);
    } else {
        for (NoPessoa *a = pessoas->inicio; a != NULL; a = a->prox) {
            printf("%d;%s;%s;%s;%s\n",
                   a->dados.codigo, a->dados.nome, a->dados.fone,
                   a->dados.endereco, a->dados.data_nascimento);
        }
    }
}
#define PET_CODIGO  1
#define PET_DONO    2
#define PET_NOME    4
#define PET_TIPO    8

static int executar_insert_pet(ListaPet *pets, ListaPessoa *pessoas,
                               ListaTipoPet *tipos, const char *cmd) {
    const char *ini_col = strchr(cmd, '(');
    const char *ini_val = strstr(cmd, "values(");
    const char *fim_val = strrchr(cmd, ')');
    if (ini_col == NULL || ini_val == NULL || fim_val == NULL) return 0;

    const char *fim_col = strchr(ini_col, ')');
    if (fim_col == NULL || fim_col > ini_val) return 0;

    char colunas[300], valores[300];
    size_t n_col = (size_t)(fim_col - ini_col - 1);
    size_t n_val = (size_t)(fim_val - (ini_val + 7));
    if (n_col >= sizeof colunas || n_val >= sizeof valores) return 0;

    memcpy(colunas, ini_col + 1, n_col);
    colunas[n_col] = '\0';
    memcpy(valores, ini_val + 7, n_val);
    valores[n_val] = '\0';

    pet p;
    memset(&p, 0, sizeof p);
    int obrig = 0;
    const char *pc = colunas;
    const char *pv = valores;
    char col[64], val[300];

    while (1) {
        int rc = proximo_valor(&pc, col, sizeof col);
        int rv = proximo_valor(&pv, val, sizeof val);
        if (rc == -1 || rv == -1) return 0;
        if (rc == 0 && rv == 0) break;
        if (rc != rv) return 0;

        if (strcmp(col, "codigo") == 0) {
            if (sscanf(val, "%d", &p.codigo) != 1) return 0;
            obrig |= PET_CODIGO;
        } else if (strcmp(col, "codigo_cli") == 0 || strcmp(col, "codigo_pes") == 0) {
            if (sscanf(val, "%d", &p.codigo_pes) != 1) return 0;
            obrig |= PET_DONO;
        } else if (strcmp(col, "nome") == 0) {
            snprintf(p.nome, sizeof p.nome, "%s", val);
            obrig |= PET_NOME;
        } else if (strcmp(col, "codigo_tipo") == 0) {
            if (sscanf(val, "%d", &p.codigo_tipo) != 1) return 0;
            obrig |= PET_TIPO;
        } else {
            return 0;
        }
    }

    if (obrig != (PET_CODIGO | PET_DONO | PET_NOME | PET_TIPO)) return 0;

    return pet_inserir(pets, p, pessoas, tipos);
}
static int proximo_set(const char **p, char *col, size_t tc, char *val, size_t tv) {
    const char *s = *p;
    size_t i = 0;

    while (*s == ' ') s++;
    if (*s == '\0') return 0;

    while (*s != '\0' && *s != '=' && i < tc - 1) col[i++] = *s++;
    if (*s != '=') return -1;
    while (i > 0 && col[i - 1] == ' ') i--;
    col[i] = '\0';

    s++;                                   
    while (*s == ' ') s++;

    const char *resto = s;
    if (proximo_valor(&resto, val, tv) != 1) return -1;  
    *p = resto;
    return 1;
}

static int extrair_set(const char *cmd, char *saida, size_t tam) {
    const char *ini = strstr(cmd, " set ");
    const char *fim = strstr(cmd, " where ");
    if (ini == NULL || fim == NULL || fim < ini) return 0;

    ini += 5;
    size_t n = (size_t)(fim - ini);
    if (n == 0 || n >= tam) return 0;
    memcpy(saida, ini, n);
    saida[n] = '\0';
    return 1;
}

static int copiar_campo(char *dst, size_t tam, const char *src) {
    if (strlen(src) >= tam) return 0;
    strcpy(dst, src);
    return 1;
}
static int executar_update_pessoa(ListaPessoa *pessoas, const char *cmd) {
    char sets[300];
    int cod;
    if (!extrair_set(cmd, sets, sizeof sets) || !parse_where_codigo(cmd, &cod)) return 0;

    NoPessoa *n = pessoa_buscar(pessoas, cod);
    if (n == NULL) return 0;

    Pessoa novo = n->dados;                
    const char *p = sets;
    char col[64], val[300];
    int alterou = 0, r;

    while ((r = proximo_set(&p, col, sizeof col, val, sizeof val)) == 1) {
        alterou = 1;
        if (strcmp(col, "nome") == 0) {
            if (!copiar_campo(novo.nome, sizeof novo.nome, val)) return 0;
        } else if (strcmp(col, "fone") == 0) {
            if (!copiar_campo(novo.fone, sizeof novo.fone, val)) return 0;
        } else if (strcmp(col, "endereco") == 0) {
            if (!copiar_campo(novo.endereco, sizeof novo.endereco, val)) return 0;
        } else if (strcmp(col, "data_nascimento") == 0) {
            if (!data_valida(val)) return 0;
            if (!copiar_campo(novo.data_nascimento, sizeof novo.data_nascimento, val)) return 0;
        } else {
            return 0;                      
        }
    }
    if (r == -1 || !alterou) return 0;

    n->dados = novo;
    return 1;
}

static int executar_update_tipo(ListaTipoPet *tipos, const char *cmd) {
    char sets[300];
    int cod;
    if (!extrair_set(cmd, sets, sizeof sets) || !parse_where_codigo(cmd, &cod)) return 0;

    NoTipoPet *n = tipo_pet_buscar(tipos, cod);
    if (n == NULL) return 0;

    tipo_pet novo = n->dados;
    const char *p = sets;
    char col[64], val[300];
    int alterou = 0, r;

    while ((r = proximo_set(&p, col, sizeof col, val, sizeof val)) == 1) {
        alterou = 1;
        if (strcmp(col, "descricao") == 0) {
            if (!copiar_campo(novo.descricao, sizeof novo.descricao, val)) return 0;
        } else {
            return 0;
        }
    }
    if (r == -1 || !alterou) return 0;

    n->dados = novo;
    return 1;
}

static int executar_update_pet(ListaPet *pets, ListaPessoa *pessoas,
                               ListaTipoPet *tipos, const char *cmd) {
    char sets[300];
    int cod;
    if (!extrair_set(cmd, sets, sizeof sets) || !parse_where_codigo(cmd, &cod)) return 0;

    NoPet *n = pet_buscar(pets, cod);
    if (n == NULL) return 0;

    pet novo = n->dados;
    const char *p = sets;
    char col[64], val[300];
    int alterou = 0, r;

    while ((r = proximo_set(&p, col, sizeof col, val, sizeof val)) == 1) {
        alterou = 1;
        if (strcmp(col, "codigo_cli") == 0 || strcmp(col, "codigo_pes") == 0) {
            if (sscanf(val, "%d", &novo.codigo_pes) != 1) return 0;
        } else if (strcmp(col, "codigo_tipo") == 0) {
            if (sscanf(val, "%d", &novo.codigo_tipo) != 1) return 0;
        } else if (strcmp(col, "nome") == 0) {
            if (!copiar_campo(novo.nome, sizeof novo.nome, val)) return 0;
        } else {
            return 0;
        }
    }
    if (r == -1 || !alterou) return 0;

    if (pessoa_buscar(pessoas, novo.codigo_pes) == NULL) return 0;
    if (tipo_pet_buscar(tipos, novo.codigo_tipo) == NULL) return 0;

    n->dados = novo;
    return 1;
}
int executar_pessoa(ListaPessoa *pessoas, ListaPet *pets, const char *comando) {
    if (strncmp(comando, "insert into ", 12) == 0) {
        if (executar_insert_pessoa(pessoas, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (dados inválidos ou código repetido): %s\n", comando);
        }
        return 1;
    }
    if (strncmp(comando, "delete from ", 12) == 0) {
        if (executar_delete_pessoa(pessoas, pets, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (não encontrada ou possui pets): %s\n", comando);
        }
        return 1;
    }
        if (strncmp(comando, "select * from ", 14) == 0) {
        executar_select_pessoa(pessoas, comando);
        return 1;
    }
        if (strncmp(comando, "update ", 7) == 0) {
        if (executar_update_pessoa(pessoas, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (dados inválidos ou registro inexistente): %s\n", comando);
        }
        return 1;
    }
    printf("[ainda não implementado pessoa] %s\n", comando);
    return 0;
}
int executar_tipo(ListaTipoPet *tipos, ListaPet *pets, const char *comando) {
    if (strncmp(comando, "insert into ", 12) == 0) {
        if (executar_insert_tipo(tipos, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (dados inválidos ou código repetido): %s\n", comando);
        }
        return 1;
    }
    if (strncmp(comando, "delete from ", 12) == 0) {
        if (executar_delete_tipo(tipos, pets, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (não encontrado ou em uso por algum pet): %s\n", comando);
        }
        return 1;
    }
        if (strncmp(comando, "update ", 7) == 0) {
        if (executar_update_tipo(tipos, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (dados inválidos ou registro inexistente): %s\n", comando);
        }
        return 1;
    }
    printf("[ainda não implementado tipo_pet] %s\n", comando);
    return 0;
}
int executar_pet(ListaPet *pets, ListaPessoa *pessoas,
                 ListaTipoPet *tipos, const char *comando) {
    if (strncmp(comando, "insert into ", 12) == 0) {
        if (executar_insert_pet(pets, pessoas, tipos, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (dados inválidos, dono/tipo inexistente ou código repetido): %s\n", comando);
        }
        return 1;
    }
    if (strncmp(comando, "delete from ", 12) == 0) {
        if (executar_delete_pet(pets, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (não encontrado): %s\n", comando);
        }
        return 1;
    }
        if (strncmp(comando, "update ", 7) == 0) {
        if (executar_update_pet(pets, pessoas, tipos, comando)) {
            printf("OK: %s\n", comando);
        } else {
            printf("ERRO (dados inválidos ou registro inexistente): %s\n", comando);
        }
        return 1;
    }
    printf("[ainda não implementado pet] %s\n", comando);
    return 0;
}