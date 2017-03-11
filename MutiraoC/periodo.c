#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float periodo(float L) {
	float g = 9.8;
	float T = 2*M_PI*sqrt(L/g);
	return T;
}

float frequencia(float L){
	float T = periodo(L);
	float f = 1/T;
	return f;
}

void compute_pendulum_swing(float L, float *T, float *f){
	float t1 = periodo(L);
	float f1 = frequencia(L);
	T= &t1;
	f=&f1;
}

int main(int argc, char *argv[]) {
  
  float L;
  float *tp;
  float *fp;
  printf("comprimento do pendulo: ");
  scanf("%f", &L);

  float T = periodo(L);
  float f = frequencia(L);
  compute_pendulum_swing(L, tp, fp);

  printf("periodo: %f\n", T);
  printf("frequencia: %f\n", f);


  return 0;
}