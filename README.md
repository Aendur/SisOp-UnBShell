# SisOp-UnBShell

Implementação do UnBShell, um shell do linux para a disciplina de Sistemas Operacionais / UnB

Autor: Diogo César Ferreira

Link para o repositório: https://github.com/Aendur/SisOp-UnBShell


## Compilação
Para compilar, executar o comando 
```
$ make
```
ou
```
$ make main
```
O executável será criado no caminho `bin/unbsh`.


## Shell interativo
Se o programa for invocado **sem** argumentos, será realizada a execução no modo interativo, onde o usuário terá acesso à linha de comando, onde poderá enviar instruções para o shell.


## Shell *standalone*
Se o programa for invocado **com** argumentos, será realizada a execução no modo *standalone*: o progama executará os comandos contidos no arquivo indicado pelo primeiro argumento (excluindo-se o nome do programa). Por exemplo:

```
$ unbsh script.sh    # executa os comandos contidos em script.sh
``` 


## Configuração do ambiente
O UnBShell utiliza um arquivo denominado `.unbshrc_profile` para configurar o ambiente de execução do usuário. Este arquivo deve estar localizado diretamente na pasta _home_ do usuário (diretório indicado pela variável `$HOME`). Na ausência deste arquivo, uma mensagem de advertência será exibida e o UnBShell utilizará as variáveis de ambiente previamente estabelecidas ao ser invocado.

**ATENÇÃO** Na ausência do `.unbshrc_profile`, se o shell for configurado como padrão do sistema, nenhuma variável de ambiente será carregada.

O diretório do projeto `env` contem um arquivos de exemplo `.unbshrc_profile` que pode ser utilizado.


## Dados adicionais
O programa foi testado utilizando-se o compilador/sistema operacional `g++ (Debian 11.2.0-16) 11.2.0`

