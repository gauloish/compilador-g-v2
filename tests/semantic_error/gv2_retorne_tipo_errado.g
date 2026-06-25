/* erro: retorne com tipo incompativel com tipo de retorno da funcao */
funcao [
  falha() : int
  [ c : car; ]
  {
    c = 'x';
    retorne c;
  }
]

principal {
  escreva falha();
  novalinha;
}
