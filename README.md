# Projeto de Compilador da G-V2

O presente projeto implementa o compilador para a linguagem G-V2 da matéria de **Compiladores** usando Flex e Bison para geração automática de analisadores léxico e sintâtico e usando MIPS para geração de código executável.

## ALunos

Nosso grupo é composto por:

- Matheus Guilherme de Oliveira Moreno (202301473)
- Monik Alves da Costa (202301476)

## Requisitos

Para a execução do projeto é necessário ter os seguintes utilitários e suas respectivas versões:

- **GCC:** 15.2.1
- **Make:** 4.4.1
- **Flex:** 2.6.4
- **Bison:** 3.8.2
- **Spim:** ?

Talvez seja possível compilar o projeto com versões diferentes desses utilitários, mas isso não foi testado. As versões sugeridas foram as que nós usamos no projeto.

## Compilação

Para compilar o programa, basta executar o seguinte comando na raiz do projeto:

```sh
make
```

E para limpar os arquivos intermediários gerados (arquivos .o e .c), vasta executar:

```sh
make clean
```

Quando a compilação encerrar, teremos o executável `g-v2` na raiz do projeto.

## Execução

Abaixo temos o passo a passo para executar o projeto.

### Compilação do Código-Fonte G-V2

Para executarmos o compilador temos duas opções. A primeira nós passamos apenas o nome do arquivo a ser compilado:

```sh
./g-v2 <arquivo_entrada>
```

Nesse caso você deve substituir o trecho `<arquivo_entrada>` pelo nome do arquivo (com caminho absoluto ou relativo) e o compilador irá gerar um arquivo chamado `assembly.s` na mesma pasta do executável.

No segundo caso, podemos passar o nome do arquivo de saída:

```sh
./g-v2 <arquivo_entrada> <arquivo_saida>
```

Analogamente ao primeiro caso, você deve substituir os trechos `<arquivo_entrada>` e `<arquivo_saida>` pelos nomes dos arquivos de entrada e saída, respectiamente. Diferente do primeiro caso, o compilador irá gerar um arquivo chamado `<arquivo_saida>.s` na mesma pasta do executável.

### Execução do Código-Fonte G-V1

Para executar o programa compilado, precisamos de um simulador MIPS para executar as instruções presentes no arquivo assembly gerado pelo nosso compilador. Existem algumas opções de simulador conhecidas, como SPIM e MARS, e nesse projeto fizemos uso do SPIM. Dessa forma, segue abaixo o comando para executar o código assembly gerado:

```sh
spim -file <arquivo_saida>.s
```

Com isso o código-fonte será executado.