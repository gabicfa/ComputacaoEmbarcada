/**************************************************************************
* Rafael Corsi   - Insper 
* rafael.corsi@insper.edu.br        
*
* Computa��o Embarcada
*
* 08-PIO-ENTRADA
************************************************************************/


#include "asf.h"
#include "conf_clock.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * LEDs
 */
#define LED_PIO_ID		ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN			8
#define LED_PIN_MASK	(1<<LED_PIN) 

/**
 * Bot�o
 */ 
#define BUT_PIO_ID		ID_PIOA
#define BUT_PIO         PIOA
#define BUT_PIN			11
#define BUT_PIN_MASK	(1<<BUT_PIN)	

/************************************************************************/
/* Prototipa��o                                                        */
/************************************************************************/
void ledConfig();

/************************************************************************/
/* Fun��es	                                                            */
/************************************************************************/

/**
 * @Brief Inicializa o pino do LED
 */
void ledConfig(){
	PMC->PMC_PCER0 = (1<<LED_PIO_ID);
	PIOC->PIO_OER  = (1 <<LED_PIN);
	PIOC->PIO_PER  = (1 <<LED_PIN);
	PIOC->PIO_CODR = (1 <<LED_PIN);
};

/************************************************************************/
/* Main                                                                 */
/************************************************************************/
int main(void)
{

	/************************************************************************/
	/* Inicializa��o b�sica do uC                                           */
	/************************************************************************/
	sysclk_init();
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	/************************************************************************/
	/* Inicializa perifericos                                               */
	/************************************************************************/
	// Configura LED em modo sa�da
	ledConfig();

	// Configura botao
	PMC->PMC_PCER0= (1<<ID_PIOA);
	PIOA->PIO_PER = (1<<BUT_PIN);
	PIOA->PIO_ODR = (1<<BUT_PIN);
	PIOA->PIO_PUER= (1<<BUT_PIN);
	PIOA->PIO_IFER= (1<<BUT_PIN);
	
	//Placa
	PMC->PMC_PCER0= (1<<ID_PIOD);
	PIOA->PIO_PER = (1<<28);
	PIOA->PIO_ODR = (1<<28);
	PIOA->PIO_PUER= (1<<28);
	PIOA->PIO_IFER= (1<<28);
	
	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/
	while(1){
		if( PIOD->PIO_PDSR & (1<<28) ){
			PIOC->PIO_SODR = (1 << 8);
		}
		else{
			PIOC->PIO_CODR = (1 << 8);
		}
	};
}


