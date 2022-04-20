#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define STR_LEN 1000
#define MAX_CMDS 100

/* Grupo:
 * Nome: Fernanda Silva Bucheri RA: 135529
 * Nome: Magno Luiz Gonçalves Melo RA: 133688
 * Nome: Marcos Ferreira Villar Coelho RA: 135534
 */

int parseOperadores(char *string, int tamanho, char *argumentos);
void retiraAspas(char *string, int indiceInicio);
int insereNaMatriz(char *string, char **matriz, int *vetordeIndices);
void argParser(char **matriz, int quantidade);
void execOperacao(char **matrizComandos, const char *vetorOperadores, int *vetorIndices, int quantidadeOperadores,
                  int indice);
int execPipe(char **matrizComandos, const int *vetorIndices, int primeiroIndex, int qntsPipe, int background);
int execSemPipe(char **matrizComandos, const int *vetorDeIndices, int indice, int background);

int main() {
    char stringEntrada[STR_LEN], vetorOperadores[STR_LEN];
    char **matrizComandos = (char **) calloc(MAX_CMDS * sizeof(char **), 1);
    int vetorDeIndices[STR_LEN];

    vetorDeIndices[0] = 0;

    while (1) {
        fprintf(stderr, "Input:> ");

        scanf(" %[^\n]s", stringEntrada);

        if (strlen(stringEntrada) == 0 || stringEntrada[0] == '\0') {
            perror("Entrada vazia ou com caracteres invalidos.");
            exit(0);
        } else if (strcmp(stringEntrada, "exit") == 0) {
            printf("Adios");
            return 1;
        }

        // Quantidade de operacoes (pipe, etc)
        int quantidadeOperadores = parseOperadores(stringEntrada, (int) strlen(stringEntrada), vetorOperadores);

        // Quantidade de comandos
        int quantidadeComandos = insereNaMatriz(stringEntrada, matrizComandos, vetorDeIndices);

        if (quantidadeComandos < quantidadeOperadores) {
            printf("Entrada invalida.");
            exit(0);
        }

        argParser(matrizComandos, quantidadeComandos + quantidadeOperadores);

        execOperacao(matrizComandos, vetorOperadores, vetorDeIndices, quantidadeOperadores, 0);

        // Limpando memória
        memset(stringEntrada, '\0', sizeof(stringEntrada));
        memset(vetorOperadores, '\0', sizeof(vetorOperadores));
        memset(matrizComandos, '\0', MAX_CMDS * sizeof(char **));
    }
}

// passa pela entrada descobrindo quais são os operadores
int parseOperadores(char *string, int tamanho, char *argumentos) {
    int i = 0, quantidadeOperadores = 0;

    for (i = 0; i < tamanho; i++) {
        if (string[i] == '|') {
            if (string[i + 1] == '|') {
                argumentos[quantidadeOperadores++] = 'o'; // o de ou
                i++; //pula um indice pois ja sabemos o proximo
            } else {
                argumentos[quantidadeOperadores++] = 'p'; // de pipe
            }
        } else if (string[i] == '&') {
            if (string[i + 1] == '&') {
                argumentos[quantidadeOperadores++] = 'e'; // e comercial
                i++; //pulamos um tambem
            } else {
                argumentos[quantidadeOperadores++] = 'b'; // background
            }
        } else if (string[i] == ';') {
            argumentos[quantidadeOperadores] = 'v'; // ponto e virgula
            quantidadeOperadores++;
        } else if ((int) string[i] == 34 || (int) string[i] == 39) {
            retiraAspas(string, i);
            string[i] = ' ';
        }
    }
    return quantidadeOperadores;
}

// retira as aspas dos comandos
void retiraAspas(char *string, int indiceInicio) {
    int i = 0;

    // 34 e 39 são os valores das aspas duplas e simples na tabela ASCII
    for (i = indiceInicio + 1; (int) string[i] != 34 && (int) string[i] != 39; i++) {
        if (string[i] == ' ') {
            string[i] = '`';
        }
    }
    string[i] = ' ';
}

// insere na matriz de comandos mas com crase em palavras separadas por espaço entre aspas
int insereNaMatriz(char *string, char **matriz, int *vetordeIndices) {
    int quantidadeComandos = 0, k = 1, contador = 0;
    char *token;

    token = strtok(string, " ");

    while (token != NULL) {
        matriz[quantidadeComandos] = (char *) calloc(sizeof(token) * sizeof(char), 1);

        if (strcmp(token, "|") == 0 || strcmp(token, "||") == 0 || strcmp(token, "&") == 0 ||
            strcmp(token, "&&") == 0 || strcmp(token, ";") == 0) {//substituir pelo strcmp
            vetordeIndices[k] = quantidadeComandos + 1;
            matriz[quantidadeComandos] = NULL;
            quantidadeComandos++;
            k++;
            token = strtok(NULL, " ");
            continue; // n vou copiar esses operadores pra matriz de comandos
        }

        strcpy(matriz[quantidadeComandos], token);
        quantidadeComandos++;
        token = strtok(NULL, " ");
        contador++;
    }

    return contador; // nem todos do qtdoperadores sai.
}

// Altera o token crase por espaço
void argParser(char **matriz, int quantidade) {
    char *aux, *troca = NULL;

    for (int i = 0; i <= quantidade + 1; i++) {
        if (matriz[i] != NULL) {
            aux = strchr(matriz[i], '`');
            if (aux != NULL) {
                troca = matriz[i];
                int k = 0;
                while (k < strlen(troca)) {
                    if (troca[k] == '`') {
                        troca[k] = ' ';
                    }
                    k++;
                }
                strcpy(matriz[i], troca);
            }
        }
    } // arruma argumento no vetor
}

// lógica principal dos operadores
void execOperacao(char **matrizComandos, const char *vetorOperadores, int *vetorIndices, int quantidadeOperadores,
                  int indice) {
    int sair = 0, quantosPipe = 0, i = 0, j = 0, execResult = -2, rodarBackground = 0;

    while (j <= quantidadeOperadores && sair == 0) {
        switch (vetorOperadores[indice]) {
            case 'p':
                i = indice;
                while (vetorOperadores[i] == 'p') {
                    quantosPipe++;
                    i++;
                }
                if (vetorOperadores[i] == 'b') {
                    rodarBackground = 1;
                    quantosPipe++;
                }
                execResult = execPipe(matrizComandos, vetorIndices, indice, quantosPipe, rodarBackground);
                j += quantosPipe;
                indice += quantosPipe;
                quantosPipe = 0;
                rodarBackground = 0;
                break;

            case 'e':
                i = indice;
                if (execResult == 0) {// se executamos um pipe e chegamos num &&
                    //e o resultado der que foi bem executado podemos executar o proximo comando
                    execResult = execSemPipe(matrizComandos, vetorIndices, indice, 0);
                    //nessa linha a cima caso
                    //lado esquerdo ja realizado e chegamos num && novamente;
                    indice++;
                    break;
                } else if (execResult == -2) {//não realizamos a exec ainda então, nesse caso precisamos executar
                    execResult = execSemPipe(matrizComandos, vetorIndices, indice, 0);
                    if (execResult == 0) { //se essa exec retornou certo apenas seguimos;
                        indice++;
                        break;
                    } else {//execução deu errado entao precisamos achar proximo || ou ;
                        while (vetorOperadores[i] == 'p' || vetorOperadores[i] == 'e') {
                            i++;
                            j++;
                        }
                        indice = i;
                        break;
                    }
                }

            case 'v':
                execSemPipe(matrizComandos, vetorIndices, indice, 0);
                execResult = -2;
                indice++;
                break;

            case 'o':
                i = indice;
                if (execResult == 0) {// se bem executados precisamos avançar ate o proximo && ou ;
                    while (vetorOperadores[i] == 'p' || vetorOperadores[i] == 'o') {
                        i++;
                        j++;
                    }
                    indice = i;
                    break;
                } else if (execResult == -2) {//caso ainda nao tivermos executado algum comando
                    execResult = execSemPipe(matrizComandos, vetorIndices, indice, 0);
                    if (execResult == 0) { //se bem executado, avançamos novamente
                        i++;
                        while (vetorOperadores[i] == 'p' || vetorOperadores[i] == 'o') {
                            i++;
                            j++;
                        }
                        if (vetorOperadores[i] != 'v' && vetorOperadores[i] != 'e' && vetorOperadores[i] != 'b')
                            sair = 1;

                        indice = i;
                        break;
                    } else { // se mal executado, apenas prosseguimos;
                        indice++;
                        break;
                    }
                } else
                    indice++;

                break;

            case 'b':
                execSemPipe(matrizComandos, vetorIndices, indice, 1);
                indice++;
                execResult = -2;
                break;

            default:
                execResult = execSemPipe(matrizComandos, vetorIndices, indice, 0);
                if (indice != 0) {
                    j++;
                }
                break;
        }
        j++;
    }
}

// faz a execução dos comandos com pipe
int execPipe(char **matrizComandos, const int *vetorIndices, int primeiroIndex, int qntsPipe, int background) {
    int fd[2];
    int i = 0, k = primeiroIndex, aux1 = 0, status = 0;
    int fdAux = STDIN_FILENO;
    pid_t saida;
    while (i <= qntsPipe) {
        if (pipe(fd) < 0) {
            perror("pipe()");
            return (-1);
        }

        aux1 = vetorIndices[k];
        k++;
        char **cmd = &matrizComandos[aux1];

        pid_t pid = fork();
        if (pid == 0) {
            close(fd[0]);
            dup2(fdAux, STDIN_FILENO);
            if (i < qntsPipe) {
                dup2(fd[1], STDOUT_FILENO);
            }

            execvp(cmd[0], cmd);
        } else if (pid > 0) {  //pai
            fdAux = fd[0];
            close(fd[1]);
            if (background == 0) {
                saida = waitpid(pid, &status, 0);
            }

        } else return -1;
        i++;
    }

    return saida;
}

// faz a execução dos comandos sem pipe
int execSemPipe(char **matrizComandos, const int *vetorDeIndices, int indice, int background) {
    pid_t pid;
    int aux = vetorDeIndices[indice];

    char **cmd = &matrizComandos[aux];

    pid = fork();
    if (pid == 0) {
        if (execvp(cmd[0], cmd) < 0) {
            printf("Erro ao executar o comando");
            return -1;
        }
    } else if (pid > 0) {
        int status = 0;
        if (background == 0) {
            waitpid(pid, &status, 0);
        }
        return status;
    }
    return -1;
}
