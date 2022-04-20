# Interpretador de comandos em terminal para Linux

O objetivo deste trabalho é implementar um interpretador de comandos em terminal para Linux.

<br>

## Requisitos

Os requisitos são:

1. Executar comandos unitários, com múltiplos parâmetros. Exemplo: 

```
# ls -la 
```

2. Executar comandos encadeados, utilizando o operador pipe “|” para combinar saída e entrada entre os comandos. Exemplo: 

```
# ls -la | grep "teste"

# cat /proc/cpuinfo | grep "model name" | wc -l 
```

3.  Executar sequências de comandos independentes com o operador “;”. Exemplo: 

```
# echo “SO 2021” ; echo “ADE Viva!” 
```
   ​    
4. Executar comandos condicionados com operadores OR (||) e AND (&&). Exemplo: 

```
# cat 12345 || echo "aquivo inexistente"

# ping -c1 www.unifesp.br.br && echo "SERVIDOR DISPONIVEL" || echo "SERVIDOR INDISPONIVEL"
```

5. Executar comandos em background, liberando o shell para receber novos comandos do usuário. 

```
# ping -c5 www.unifesp.br &
```

<br>

## Implementação

No código implementado foram definidas, além da função main, sete funções para atender aos requisitos solicitados.

- parseOperadores( ): passa pela entrada descobrindo quais são os operadores;
- retiraAspas( ): retira as aspas dos comandos;
- insereNaMatriz( ): insere na matriz de comandos, mas com crase em palavras separadas por espaço entre aspas;
- argParser( ): altera o token crase por espaço;
- execOperacao( ): lógica principal dos operadores;
- execPipe( ): faz a execução dos comandos com pipe;
- execSemPipe( ): faz a execução dos comandos sem pipe;