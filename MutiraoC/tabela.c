#include <stdio.h>

int main(void)
{
    char nome1[10], nome2[10], nome3[10];
    float preco1, preco2, preco3 ;
    int n1,n2,n3;
    float total1, total2, total3, subtotal, imposto, total;

    printf("\n nome produto1:");
    scanf("%s", nome1);
    printf("\n preco produto1:");
    scanf("%f", &preco1);
    printf("\n quantidade produto1:");
    scanf("%d", &n1);
    printf("\n nome produto2:");
    scanf("%s", nome2);
    printf("\n preco produto2:");
    scanf("%f", &preco2);
    printf("\n quantidade produto2:");
    scanf("%d", &n2);
    printf("\n nome produto3:");
    scanf("%s", nome3);
    printf("\n preco produto3:");
    scanf("%f", &preco3);
    printf("\n quantidade produto3:");
    scanf("%d", &n3);

    total1 = preco1*n1;
    total2=preco2*n2;
    total3=preco3*n3;
    subtotal=total1+total2+total3;
    imposto=subtotal*0.05;
    total=subtotal+imposto;

    printf("\n Produto   Preco Unitario   Quantidade   Preco Total \n");
    printf("\n %s       R$ %4.2f        %d      R$%4.2f \n", nome1, preco1, n1, total1);
    printf("\n %s       R$ %4.2f        %d      R$%4.2f \n", nome2, preco2, n2, total2);
    printf("\n %s       R$ %4.2f        %d      R$%4.2f \n", nome3, preco3, n3, total3);
    printf("\n                    Sub-total                     R$%4.2f",subtotal);
    printf("\n                    Imposto(x0.05)                R$%4.2f",imposto);
    printf("\n                    Total                         R$%4.2f \n", total);


    return 0;
}
