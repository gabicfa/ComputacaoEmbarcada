/**
 * Computaçao Embarcada
 * Mutirão C
 * Aula 1
 * Rafael Corsi e Fábio Ayres
 *
 */

#include <stdio.h>

double main(void)
{
    int  PV;
    float i;
    int n;

    scanf("%d", &PV);
    scanf("%f", &i);
    scanf("%d", &n);

    for (n; n>0 ; n--){
    	PV= PV*(1+i);
    	printf("%d \n", PV);
    }

    printf("\nO valor final eh: %d \n", PV);
    return 0;
}
