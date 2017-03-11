#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int maior_inteiro(int n1, int n2) {
  if (n1>n2){
    return n1;
  }
  else {
    return n2;
  }
}


int maior_no_vetor(int lista[], int tamanho){

  printf("O tamanho é %d\n",tamanho);
  int maior=0;

  for (int i = 0; i < tamanho; ++i){
      maior=maior_inteiro(maior,lista[i]);
  }
  return maior;
}

int main(int argc, char *argv[]) {
  
  int lista_teste[] = {3,37,8,10,99};
  int tamanho = sizeof(lista_teste)/sizeof(int);


  int m = maior_no_vetor(lista_teste, tamanho);

  printf("%d\n", m);

  return 0;
}


