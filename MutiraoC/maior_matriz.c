#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M 3
int maior_inteiro(int n1, int n2) {
  if (n1>n2){
    return n1;
  }
  else {
    return n2;
  }
}

int maior_diagonal(int A[M][M]){
	int maior=0;
	for (int i = 0; i < M; ++i)
	{
		for (int j = 0; j < M; ++j)
		{
			if(i==j){
				maior=maior_inteiro(maior,A[i][j]);
			}
		}
	}
	return maior;
}

int main(int argc, char *argv[]) {
  
  int matriz_teste[M][M] = {13,122,3432,4143,534,62,7,8,1};

  int m = maior_diagonal(matriz_teste);

  printf("%d\n", m);

  return 0;
}