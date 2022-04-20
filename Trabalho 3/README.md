# Simulação do algoritmo MRU

O trabalho 3 se trata de substituição de página (Algoritmo de Envelhecimento)

Foi solicitado a implementação de uma simulação do algoritmo MRU (Algoritmo de Envelhecimento - _Aging_). Para tanto, foi considerado as seguintes premissas: 

* Os tiques dos relógio são represados por eventos discretos. 

* Uma memória real, MR, que está sempre cheia. 

* Uma memória virtual, MV, onde |MV| > |MR|. 

* Uma memória swap, MS, onde |MS| >= |MV| - |MR| 

* A memória é representada por conjunto de páginas, onde cada página é representada por um registro (struct), contendo atributos de interesse. 

* A cada tique, um subconjunto aleatório de páginas H, onde H ⊂ MV, o qual foi referenciado em memória. 

<br>

## Implementação

No algoritmo desenvolvido cada página possui um inteiro ID (para identificá-la) e um vetor "_ultimasRefs"_ de tamanho TAM que tem como função fazer o controle de quando a página é referenciada (armazena 1 quando ela é referenciada e 0 quando não é).

* _inicializaVetor()_: inicializa um vetor atribuindo a ele o valor passado como parâmetro;

* _criarPagina()_: inicialmente verifica se é possível criar uma nova página. Se sim, a página é criada. É atribuída a ela um ID e seu vetor  "_ultimasRefs"_ é inicializado com zero. Por fim, insere essa nova página na memória virtual;

* _criaMemoriaVirtual()_: como o nome sugere, cria a memória virtual. Enquanto houver espaço, ela chama a função _criarPagina()_ para criar novas páginas e inserir na memória;

* _buscaLista()_: busca na memória virtual uma página;

* _preencheMemoriaFisica()_: busca uma página na memória virtual e a insere na física;

* _shift()_: responsável por marcar no vetor _"ultimasRefs"_ se a página foi referenciada ou não;

* _fazReferecia()_: responsável por referenciar (ou não) uma página. Posteriormente ela chama a função _shift()_ para marcar essa referência;

* _conversorBinarioDecimal()_: converte um valor binário em um valor decimal  (isso é utilizado para converter o valor binário presente no vetor "_ultimasRefs_" e verificar qual página foi menos referenciada);

* _insereNaSwap()_: passa uma página da memória física para a memória swap;

* _idNaoPresenteNaMemFisica()_: busca pelos ID's que não estão na memória física ;

* _tiraDaSwap()_: tira uma página da memória swap, pois ela vai para a memória principal;

* _colocaNaReal()_: insere uma página na memória física. Primeiramente busca-se qual a página menos referenciada na memória real (em um laço utiliza-se a função _conversorBinarioDecimal()_ e salva-se o resultado em um vetor auxiliar. Quando acaba de percorrer a memória física, basta verificar em qual posição desse vetor auxiliar está o menor valor e é nessa posição da memória real que está a página menos referenciada). Posteriormente busca-se um ID que não está presente nessa memória e procura-se esse ID na memória virtual. As referências da página menos referenciada são zeradas, tira da memória swap o ID que vai entrar na memória física e insere na swap a página que vai sair da memória física. É realizada uma referência nessa nova página inserida;

* _inicializaSwp()_: inicializa a memória swap;

* _imprimeReferenciasNaMemoria()_: imprime o vetor _"ultimasRefs"_ de todas as páginas na memória física; 

* _main()_: função principal do código. Primeiramente chama-se a função _srand()_ responsável por gerar números aleatórios. Posteriormente cria-se os ponteiros para memória virtual, física e swap, respectivamente. Então é solicitado ao usuário o número de páginas que se deseja criar (número esse sempre maior que o tamanho da memória física, para utilizar o mecanismo de substituição de páginas). Cria-se a memória virtual, preenche a memória física, referencia as páginas e inicializa a memória swap. Em seguida, é exibido para o usuário um menu com as seguintes opções:

```
  1) Inserir uma pagina na memoria virtual

  2) Realizar referencia na memoria fisica

  3) Inserir na memoria fisica

  4) Imprimir todas as ultimas referencias

  0) Sair
```

Onde são realizadas as ações específicas para cada caso.