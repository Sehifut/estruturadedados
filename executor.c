#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "executor.h"

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
/* lê "where codigo = N" e devolve N. Só aceita a coluna codigo. */
static int parse_where_codigo(const char *cmd, int *valor) {
    const char *w = strstr(cmd, " where ");
    if (w == NULL) return 0;

    const char *p = w + 7;                 /* pula " where " */
    while (*p == ' ') p++;
    if (strncmp(p, "codigo", 6) != 0) return 0;
    p += 6;
    while (*p == ' ') p++;
    if (*p != '=') return 0;
    p++;

    return sscanf(p, "%d", valor) == 1;
}

/* devolve 1 se algum pet usa este tipo */
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
    if (pessoa_buscar(pessoas, cod) == NULL) return 0;   /* não existe */
    if (pet_tem_dono(pets, cod)) return 0;               /* tem pets: não remove */
    return pessoa_remover(pessoas, cod);
}

static int executar_delete_tipo(ListaTipoPet *tipos, ListaPet *pets,
                                const char *cmd) {
    int cod;
    if (!parse_where_codigo(cmd, &cod)) return 0;
    if (tipo_pet_buscar(tipos, cod) == NULL) return 0;
    if (tipo_em_uso(pets, cod)) return 0;                /* há pets com esse tipo */
    return tipo_pet_remover(tipos, cod);
}

static int executar_delete_pet(ListaPet *pets, const char *cmd) {
    int cod;
    if (!parse_where_codigo(cmd, &cod)) return 0;
    return pet_remover(pets, cod);
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
    printf("[ainda não implementado tipo_pet] %s\n", comando);
    return 0;
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
    printf("[ainda não implementado pet] %s\n", comando);
    return 0;
}