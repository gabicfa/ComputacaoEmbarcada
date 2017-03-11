#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int num_har(int n1){

	int r = 0;
	int n2 =n1;
	while(n2>0){
		r+=n2%10;
		n2=n2/10;
	}
	if(n1%r==0){
		printf("%d\n", n1);
		printf("%d\n", r);
		return 1;
	}
	else{
		return 0;
	}
}

int main(int argc, char *argv[]) {
	
	int nt;
	printf("numero teste: ");	
	scanf("%d", &nt);
	num_har(nt);
	return 0;
}