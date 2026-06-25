/* erro: passa vetor onde a funcao espera parametro escalar */
funcao [
  dobro(x : int) : int
  {
    retorne x * 2;
  }
]

principal
[ v[3] : int; ]
{
  v[0] = 1;
  escreva dobro(v);
  novalinha;
}
