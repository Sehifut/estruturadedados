#ifndef tipo_pet_h
#define tipo_pet_h

typedef struct {
    int  codigo;            
    char descricao[100];
} tipo_pet;

typedef struct NoTipoPet {
    tipo_pet dados;
    struct NoTipoPet *ant;
    struct NoTipoPet *prox;
} NoTipoPet;

typedef struct {
    NoTipoPet *inicio;
    NoTipoPet *fim;
    int tamanho;
} ListaTipoPet;

void lista_tipo_pet_inicializar(ListaTipoPet *l);
int  tipo_pet_inserir(ListaTipoPet *l, tipo_pet t);
NoTipoPet *tipo_pet_buscar(ListaTipoPet *l, int codigo);
int  tipo_pet_remover(ListaTipoPet *l, int codigo);
void lista_tipo_pet_liberar(ListaTipoPet *l);
int  tipo_pet_salvar(ListaTipoPet *l, const char *arquivo);
int  tipo_pet_carregar(ListaTipoPet *l, const char *arquivo);

#endif