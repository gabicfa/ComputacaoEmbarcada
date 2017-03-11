#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void intercala(int a[], int b[], int n, int c[]){
	int m=0;
	int p=0;
	for(int i = 0; i< 2*n; i++){
		if (i%2==0){
			c[i]=a[m];
			m+=1;
		}
		else{
			c[i]=b[p];
			p+=1;
		}
	}
}

int main(int argc, char *argv[]) {
	
	int a[]={1,2,3,4,5};
	int b[]={99,88,77,66,55};
	int n = 3;
	int c[]={6,7,8,9,10,11,12,13,14,15,16,17};

	intercala(a, b, n, c);
	int tamanho = sizeof(c)/sizeof(int);

	for(int i= 0; i< tamanho; i++)
	{
		printf("%d ", c[i]);
	}

	return 0;
}