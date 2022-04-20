# Sincronização transparente de arquivos 

Foi solicitado a implementação de um mecanismo de sincronização local de arquivos transparente.

As principais operações previstas são:

- Verificação periódica dos tempos dos arquivos:
  
  A cada x ms, verifica-se as informações de tempo, t_origem, de todos os arquivos na sub-árvore de diretórios de origem (/minhapasta) e compara-se com os tempos, t_destino, das cópias na pasta de destino (/backup_minhapasta). 

<br>

- Sincronização:
  
  Se na última verificação de t_origem, um dado arquivo f teve seus tempos alterados em relação ao tempo t_destino da sua cópia, então o arquivo f de /minhapasta deve ser sincronizado (nesse caso, sobreposto) em /backup_minhapasta. 

<br>

## Implementação
O código elaborado consiste em duas funções: a main e a função syncFiles, responsável por realizar as operações solicitadas.

