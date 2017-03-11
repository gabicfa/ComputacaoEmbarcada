#include <stdio.h>

#define MAXELEM 1000

int main(int argc,char *argv[])
{
	int matriz[3][3];
	int transposta[3][3];

	for(int linha = 0; linha<3; linha ++ ){
		for(int coluna=0; coluna<3; coluna++){
			printf("elemento %d x %d? : \n", linha+1, coluna+1);
			scanf("%d",&matriz[linha][coluna]);
		}
	}

	for(int linha = 0; linha<3; linha ++ ){
		for(int coluna=0; coluna<3; coluna++){
			transposta[linha][coluna]=matriz[coluna][linha];
	    	printf("%d     ", transposta[linha][coluna]);
    	}
	    printf("\n");
	}
	
	 return 0;
}