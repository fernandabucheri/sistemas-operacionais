#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10
#define TAMSWAP 20
#define TAMFI 10
#define TAMVI 30

int qtdePaginasMemVirtual = 0;

struct paginas {
    int id;
    int ultimasRefs[TAM];
    struct paginas *ant, *prox;
};
typedef struct paginas *paginaPointer;

// Inicializa um vetor com um inteiro especificado
void inicializaVetor(int *ponteiro, int valorInit, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        ponteiro[i] = valorInit;
    }
}

// Cria a pagina
paginaPointer criarPagina(int id, paginaPointer inicio, int printFlag) {
    if (qtdePaginasMemVirtual >= TAMVI) {
        printf("A quantidade de paginas na memoria virtual ja atingiu o maximo\n");
        return inicio;
    }
    qtdePaginasMemVirtual++;
    paginaPointer aux = inicio;
    paginaPointer novo = (paginaPointer) malloc(sizeof(struct paginas));

    novo->id = id;
    inicializaVetor(novo->ultimasRefs, 0, TAM);
    novo->prox = NULL;

    if (inicio == NULL) {
        novo->ant = NULL;
        return novo;
    }

    while (aux->prox != NULL) {
        aux = aux->prox;
    }

    novo->ant = aux;
    novo->id = aux->id + 1;
    aux->prox = novo;

    if (printFlag) {
        printf("Novo id inserido na memoria virtual: %d\n", novo->id);
    }

    return inicio;
}

// Cria a "memoria virtual", ou seja, cria uma lista de paginas
paginaPointer criaMemoriaVirtual(int numPaginas, paginaPointer inicio, int printFlag) {
    int id = 0;
    while (numPaginas > 0) {
        inicio = criarPagina(id, inicio, printFlag);
        numPaginas--;
        id++;
    }
    return inicio;
}

// Busca por um id na lista encadeada
paginaPointer buscaLista(paginaPointer lista, int id) {
    paginaPointer aux = lista;
    while (aux != NULL) {
        if (aux->id == id) {
            return aux;    // encontrado
        }

        aux = aux->prox;
    }
    return NULL; // nao encontrado
}

// Inicializa a memoria fisica
void preencheMemoriaFisica(paginaPointer listaMemoriaVirtual, paginaPointer *listaMemoriaFisica) {
    paginaPointer aux;
    for (int i = 0; i < TAMFI; i++) {
        aux = buscaLista(listaMemoriaVirtual, i);
        listaMemoriaFisica[i] = aux;
    }
}

// Shift do bit no ultimasRefs
void shift(paginaPointer no, int ref) { // ref referenciado ou não assumindo 1 ou 0
    for (int i = TAM - 1; i > 0; i--) {
        no->ultimasRefs[i] = no->ultimasRefs[i - 1];
    }
    no->ultimasRefs[0] = ref;
}

// Faz uma referencia em uma pagina
void fazReferencia(paginaPointer *listaMemoriaFisica) {
    for (int i = 0; i < TAMFI; i++) {
        int ref = rand() % 2;
        shift(listaMemoriaFisica[i], ref);
    }
}

// Converte o numero binário para decimal
int conversorBinarioDecimal(const int vetorBinario[], int tamanho) {
    int decimal = 0;

    for (int i = 0; i < tamanho; ++i) {
        decimal <<= 1;
        decimal += vetorBinario[i] - 0;
    }
    return decimal;
}

// Insere uma pagina na memoria swap
int insereNaSwap(paginaPointer *listaMemoriaFisica, paginaPointer *listaMemoriaSwap, int indice) {
    for (int i = 0; i < TAMSWAP; ++i) {
        if (listaMemoriaSwap[i] == NULL) {
            listaMemoriaSwap[i] = listaMemoriaFisica[indice];
            return 1;
        }
    }
    return 0;
}

// Retorna um id que nao esta na memoria fisica
int idNaoPresenteNaMemFisica(paginaPointer *listaMemoriaFisica, int numPaginas) {
    int id = numPaginas - 1;
    for (int i = 0; i < TAMFI; ++i) {
        if (listaMemoriaFisica[i]->id == id) {
            id--;
            i = 0;
        }
    }
    return id;
}

// Remove da memoria swap
void tiraDaSwap(int id, paginaPointer *listaMemoriaSwap) {
    for (int i = 0; i < TAMSWAP; i++) {
        if (listaMemoriaSwap[i] != NULL && listaMemoriaSwap[i]->id == id) {
            listaMemoriaSwap[i] = NULL;
            break;
        }
    }
}

// Coloca uma pagina na memoria real
void colocaNaReal(paginaPointer *listaMemFisica, paginaPointer listaMemVirtual, paginaPointer *listaMemSwap,
                  int numPaginas, int printFlag) {
    int linhas[TAMFI] = {0};
    int indiceMenorValor = 0;

    // Converte o valor binario do vetor de ultimas referencias para numero decimal
    for (int i = 0; i < TAMFI; i++) {
        linhas[i] = conversorBinarioDecimal(listaMemFisica[i]->ultimasRefs, TAM);
    }

    // Verifica qual pagina foi menos referenciada para ser substituida
    for (int i = 0; i < TAMFI; i++) {
        if (linhas[i] < linhas[indiceMenorValor]) {
            indiceMenorValor = i;
        }
    }

    int id = idNaoPresenteNaMemFisica(listaMemFisica, numPaginas);
    paginaPointer aux = buscaLista(listaMemVirtual, id);

    if (printFlag) {
        printf("Retirando o id %d e inserindo o id %d na memoria fisica\n", listaMemFisica[indiceMenorValor]->id,
               aux->id);
    }

    // Zerando o que sai da memoria
    inicializaVetor(listaMemFisica[indiceMenorValor]->ultimasRefs, 0, TAM);

    // Se o id ja estiver na swap, remove da swap
    tiraDaSwap(id, listaMemSwap);

    if (!insereNaSwap(listaMemFisica, listaMemSwap, indiceMenorValor)) {
        printf("A memoria SWAP ja esta lotada.\n");
    }

    aux->ultimasRefs[0] = 1;
    listaMemFisica[indiceMenorValor] = aux;
}

// Inicializa a memoria swap
void inicializaSwap(paginaPointer *listaMemoriaSwap) {
    for (int i = 0; i < TAMSWAP; ++i) {
        listaMemoriaSwap[i] = NULL;
    }
}

// Imprime o vetor de ultimas referencias da memoria fisica
void imprimeReferenciasNaMemoria(paginaPointer *listaMemoriaFisica) {
    for (int i = 0; i < TAMFI; ++i) {
        printf("listaMemoriaFisica[%d] (id = %d) = ", i, listaMemoriaFisica[i]->id);
        for (int j = 0; j < TAM; ++j) {
            printf("%d ", listaMemoriaFisica[i]->ultimasRefs[j]);
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));
    paginaPointer inicioListaMemoriaVirtual = NULL;   // inicializa a lista da memoria virtual
    paginaPointer listaMemoriaFisica[TAMFI];    // vetor para a memoria fisica
    paginaPointer listaMemoriaSwap[TAMSWAP];

    int numPaginas;

    do {
        printf("digite numero de paginas superior a %d:\n", TAMFI);
        scanf("%d", &numPaginas);
    } while (numPaginas < TAMFI);

    if (numPaginas > TAMVI) {
        printf("Numero de paginas > tamanho da memoria virtual. Criando %d paginas.\n", TAMVI);
        numPaginas = TAMVI;
    }

    inicioListaMemoriaVirtual = criaMemoriaVirtual(numPaginas, inicioListaMemoriaVirtual, 0);

    preencheMemoriaFisica(inicioListaMemoriaVirtual, listaMemoriaFisica);

    fazReferencia(listaMemoriaFisica);

    inicializaSwap(listaMemoriaSwap);

    int operacao = 0;
    do {
        printf("Selecione uma opcao:\n");
        printf("1) Inserir uma pagina na memoria virtual\n");
        printf("2) Realizar referencia na memoria fisica\n");
        printf("3) Inserir na memoria fisica\n");
        printf("4) Imprimir todas as ultimas referencias\n");
        printf("0) Sair\n");
        printf("> ");
        scanf("%d", &operacao);
        switch (operacao) {
            case 0:
                return 0;
            case 1:   // caso de inserir mais uma pagina na memoria virtual
                if (numPaginas == TAMVI) {
                    printf("Impossivel inserir mais uma pagina. Limite de %d atingido.\n\n", TAMVI);
                } else {
                    printf("\nInserindo mais uma pagina na memoria virtual\n");
                    inicioListaMemoriaVirtual = criaMemoriaVirtual(1, inicioListaMemoriaVirtual, 1);
                    numPaginas++;
                }
                break;
            case 2:   // operacoes na memoria fisica
                printf("\nFazendo referencias 3 vezes\n");
                for (int i = 0; i < 3; ++i) {
                    fazReferencia(listaMemoriaFisica);
                }
                break;
            case 3:
                printf("\nInserindo na memoria real\n");
                colocaNaReal(listaMemoriaFisica, inicioListaMemoriaVirtual, listaMemoriaSwap, numPaginas, 1);
                break;
            case 4:
                imprimeReferenciasNaMemoria(listaMemoriaFisica);
                break;
            default:
                printf("\nEntre com um numero entre 0 a 4\n");
                break;
        }
    } while (operacao >= 0);
    return 0;
}
