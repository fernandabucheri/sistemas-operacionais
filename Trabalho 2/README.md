# Implementação de algoritmos de escalonamento no MINIX3

O objetivo deste trabalho foi entender, implementar e avaliar diferentes algoritmos de escalonamentos de sistemas interativos, utilizando o sistema operacional **MINIX3** como base. 

Para isso foram escolhidos dois algoritmos de escalonamento: **Round-Robin de Fila Única** e **Loteria**.

<br>

## Transformação do escalonador padrão do MINIX3 em um escalonador Round-Robin de Fila Única

Para transformar o escalonador padrão do MINIX3 em um escalonador Round-Robin de Fila Única foi alterado o código [_schedule.c_](https://github.com/fernandabucheri/sistemas-operacionais/blob/master/Trabalho%202/schedule.c), especificamente a função _"schedule_process"_, onde adicionou-se um _if_  para verificar qual a prioridade do processo. Se a prioridade for maior que 7, ela é alterada para 7, sendo assim garante-se que os processos com prioridade de 7 a 16 vão para a fila 7. 

<br>

## Transformação do escalonador padrão do MINIX3 em um escalonador de Loteria

Para transformar o escalonador padrão do MINIX3 em um escalonador de Loteria  foi alterado o código [_proc.c_](https://github.com/fernandabucheri/sistemas-operacionais/blob/master/Trabalho%202/proc.c), especificamente a função _"pick_proc"_, onde criou-se duas váriaveis: um auxiliar "aux" e um contador "total_tickets". Posteriormente criou-se um laço _for_ para distribuir os tickets para os processos (a quantidade de tickets recebida por cada processo depende da sua prioridade). Depois é sorteado um ticket (com auxílio da função _random()_ - que escolhe um número aleatório entre 0 e o número total de tickets distribuídos). Em seguida a variável "total_tickets" é zerada e cria-se um novo _for_, bastante semelhante ao primeiro criado, porém agora ele é utilizado para buscar o ticket sorteado. O processo que estiver com o ticket é escolhido para rodar.

<br>

## Testes

Através do código [_test.c_](https://github.com/fernandabucheri/sistemas-operacionais/blob/master/Trabalho%202/test.c) foram realizados testes para avaliar o desempenho tanto do escalonador padrão do MINIX3, como dos escalonadores escolhidos. 

A pasta ["Tempos"](https://github.com/fernandabucheri/sistemas-operacionais/tree/master/Trabalho%202/Tempos) contém os testes realizados com os seguintes parâmetros:

```
./ test 100 100000 100000000
```

O que significa que foi realizado um experimento com 100 processos, sendo 50 processos de IO _bound_ que realizaram 100000 operações de saída e 50 processos de CPU _bound_ eque realizaram 100000000 de operações aritméticas.