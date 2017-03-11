#include <stdio.h>

#define MAXELEM 1000

int main(int argc,char *argv[])
{
	int n;
	int numeros[n];
	int i;

	printf("Quantos numeros? : \n");
	scanf("%d",&n);

	if(n < 0 || n > MAXELEM){
		printf("digite menor que %d e maior que zero \n", MAXELEM);
		return -1;
	}

	for(i = 0; i<n; i++){
		printf("Qual numero?: \n");
		scanf("%d",&numeros[i]);
	}

	printf("numeros: ");

	for (i=n-1; i>=0; i--){

		printf("%d ",numeros[i]);
	}

	return 0;

}