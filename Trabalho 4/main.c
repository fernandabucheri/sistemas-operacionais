#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define BLOCK 4096
#define MAXPATHSIZE 512

int syncFiles(int n, int b, struct dirent **sourceFileList, struct dirent **destFileList, char sourceDir[],
              char destDir[]) {
    struct stat buf;
    struct stat bufBackup;
    time_t userFileLastModified, backupFileLastModified;

    for (int i = 2; i < n; ++i) {
        // Essa variavel eh utilizada para apontar para o arquivo do usuario
        char sourceFilePath[MAXPATHSIZE] = "";
        // Passamos o diretorio para depois adicionarmos o arquivo no final "Ex: dir/file.format"
        strcpy(sourceFilePath, sourceDir);

        for (int j = 2; j < b; ++j) {
            // Essa variavel eh utilizada para apontar para o arquivo do backup
            char backupFilePath[MAXPATHSIZE] = "";
            strcpy(backupFilePath, destDir);

            // Se forem o mesmo arquivo, sameFile == 0
            int sameFile = strcmp(sourceFileList[i]->d_name, destFileList[j]->d_name);

            if (!sameFile) {
                // Apontamos para o arquivo do usuario e printamos quando foi a ultima modificacao
                strcat(sourceFilePath, sourceFileList[i]->d_name);
                if (stat(sourceFilePath, &buf) == -1) {
                    printf("Stat error: %s\n", strerror(errno));
                } else {
                    userFileLastModified = buf.st_mtime;
                    printf("Timestamp arquivo usuario - ultima vez modificado: %s\n", ctime(&userFileLastModified));
                }

                // Apontamos para o arquivo do backup
                strcat(backupFilePath, destFileList[j]->d_name);
                if (stat(backupFilePath, &bufBackup) == -1) {
                    printf("AUX Stat error: %s\n", strerror(errno));
                } else {
                    backupFileLastModified = bufBackup.st_mtime;
                    printf("Timestamp arquivo backup - ultima vez modificado: %s\n", ctime(&backupFileLastModified));
                }

                double diffBetweenModifiedTimes = difftime(userFileLastModified, backupFileLastModified);

                if (diffBetweenModifiedTimes > 0) {
                    printf("Arquivo modificado recentemente: %s\n", sourceFilePath);
                    printf("Backup que sera atualizado: %s\n", backupFilePath);

                    // Abre o arquivo do usuario
                    int fdUser = open(sourceFilePath, O_RDONLY);
                    if (fdUser == -1) {
                        printf("Open fdUser error: %s\n", strerror(errno));
                        return 1;
                    }

                    // Abre o arquivo do backup
                    int fdBackup = open(backupFilePath, O_CREAT | O_RDWR | O_TRUNC,
                                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (fdBackup == -1) {
                        printf("Open fdBackup error: %s\n", strerror(errno));
                        close(fdUser);
                        return 1;
                    }

                    int leitura, posicaoEscrita, escrita, tamanhoEscrita;
                    unsigned char buffer[BLOCK];
                    void *ptrBuff;
                    do {
                        // Lê os dados do arquivo da pasta origem
                        leitura = read(fdUser, buffer, BLOCK);

                        // Caso de erro na leitura
                        if (leitura == -1) {
                            printf("Erro na leitura: %s\n", strerror(errno));
                            close(fdUser); // Fecha o arquivo do usuário
                            close(fdBackup); //  Fecha o arquivo de backup
                            return 1;
                        } else if (leitura > 0) {
                            // Ocorreu a leitura
                            ptrBuff = buffer;
                            escrita = leitura;
                            posicaoEscrita = 0;
                            // Escreve os dados lidos no arquivo backup
                            do {
                                tamanhoEscrita = write(fdBackup, ptrBuff + posicaoEscrita, escrita);
                                // Se ocorrer erro na escrita
                                if (tamanhoEscrita == -1) {
                                    printf("Erro na escrita: %s\n", strerror(errno));
                                    close(fdUser); // Fecha o arquivo do usuário
                                    close(fdBackup); // Fecha o arquivo de backup
                                    return 1;
                                }
                                posicaoEscrita += tamanhoEscrita;
                                escrita -= tamanhoEscrita;
                            } while (escrita > 0);
                        }
                    } while (leitura > 0);
                    close(fdUser); // Fecha o arquivo do usuário
                    close(fdBackup); // Fecha o arquivo de backup
                    printf("Backup realizado com sucesso!\n\n");
                }
            }
        }
    }
    return 0;
}

int main() {
    struct dirent **sourceFileList;
    struct dirent **destFileList;

    char sourceDir[MAXPATHSIZE] = "/home/nanda/Área de Trabalho/Trab4SO/Files/";
    char destDir[MAXPATHSIZE] = "/home/nanda/Área de Trabalho/Trab4SO/Backup/";

    int n = scandir(sourceDir, &sourceFileList, NULL, alphasort);
    int b = scandir(destDir, &destFileList, NULL, alphasort);

    if (n < 0 || b < 0) {
        printf("Scandir error: %s\n", strerror(errno));
    } else {
        while (1) {
            if (syncFiles(n, b, sourceFileList, destFileList, sourceDir, destDir)) return 0;
            sleep(3);
        }
    }
    return 0;
}