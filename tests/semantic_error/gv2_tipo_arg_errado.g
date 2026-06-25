/* erro: argumento de tipo incompativel com parametro */
funcao [
  dobro(x : int) : int
  {
    retorne x * 2;
  }
]

principal
[ c : car; ]
{
  c = 'a';
  escreva dobro(c);
  novalinha;
}
