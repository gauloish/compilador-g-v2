/* erro: var local no bloco externo da funcao com nome de parametro */
funcao [
  f(a : int) : int
  [ a : int; ]
  {
    a = 1;
    retorne a;
  }
]

principal
{
  escreva f(2);
  novalinha;
}
