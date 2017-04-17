/** 
 * 12- ADC
 * Rafael Corsi @ insper.edu.br
 * Abril 2017
 * 
 * Configura o ADC do SAME70 para fazer leitura
 * do sensor de temperatura interno
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "asf.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/** Header printf */

#define YEAR        2017
#define MOUNTH      3
#define DAY         27
#define WEEK        13
#define HOUR        9
#define MINUTE      5
#define SECOND      0

#define STRING_EOL    "\r"
#define STRING_HEADER "-- AFEC Temperature Sensor Example --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Reference voltage for AFEC,in mv. */
#define VOLT_REF        (3300)

/** The maximal digital value */
/** 2^12 - 1                  */
#define MAX_DIGITAL     (4095UL)

/** The conversion data is done flag */
volatile bool is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

/* Canal do sensor de temperatura */
#define AFEC_CHANNEL_TEMP_SENSOR 11

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/**
 * \brief Configure UART console.
 * BaudRate : 115200
 * 8 bits
 * 1 stop bit
 * sem paridade
 */

static void configure_console(void)
{
 
	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/** 
 * converte valor lido do ADC para temperatura em graus celsius
 * input : ADC reg value
 * output: Temperature in celsius
 */
static int32_t convert_adc_to_temp(int32_t ADC_value){
  
  int32_t ul_vol;
  int32_t ul_temp;
  
	ul_vol = ADC_value * VOLT_REF / MAX_DIGITAL;

  /*
   * According to datasheet, The output voltage VT = 0.72V at 27C
   * and the temperature slope dVT/dT = 2.33 mV/C
   */
  ul_temp = (ul_vol - 720)  * 100 / 233 + 27;
  return(ul_temp);
}

/************************************************************************/
/* Call backs / Handler                                                 */
/************************************************************************/

/**
 * \brief AFEC interrupt callback function.
 */
void TC_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC0, 0);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

}
void RTC_Handler(void)
{
	uint32_t ul_status = rtc_get_status(RTC);

	/* Second increment interrupt */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	}
	else{
		/* Time or date alarm */
		if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
			
			/*Atualiza hora */
			uint32_t ano,mes,dia,hora,minuto,segundo;
			
			rtc_get_date(RTC, &ano, &mes, &dia, NULL);
			rtc_get_time(RTC, &hora, &minuto, &segundo);
			
			/* incrementa minuto */
			if(segundo>=59){
				segundo = 0;
				if(minuto>=59){
					minuto = 0;
					if(hora>=23){
						hora=0;
						if(dia>=30){
							dia =1;
							if(mes>=12){
								mes =1;
								ano++;
							}
							else{
								mes++;
							}
						}
						else{
							dia++;
						}
					}
					else{
						hora++;
					}
				}
				else{
					minuto++;
				}
			}
			else{
				segundo++;
			}

			
			/* configura novo alarme do RTC */
			rtc_set_date_alarm(RTC, 1, mes, 1, dia);
			rtc_set_time_alarm(RTC, 1, hora, 1, minuto, 1, segundo);
						
			rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
		}
	}
}
static void AFEC_Temp_callback(void)
{
	g_ul_value = afec_channel_get_value(AFEC0, AFEC_CHANNEL_TEMP_SENSOR);
	is_conversion_done = true;
}
void TC_init( Tc *TC, uint32_t ID_TC,  uint32_t channel, uint32_t freq ){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();
	
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  4Mhz e interrupçcão no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, channel, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, channel, (ul_sysclk / ul_div) / freq);

	/* Configura e ativa interrupçcão no TC canal 0 */
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, channel, TC_IER_CPCS);

	/* Inicializa o canal 0 do TC */
	tc_start(TC, channel);
}
void RTC_init(){
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);
	
	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(RTC, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(RTC, YEAR, MOUNTH, DAY, WEEK);
	rtc_set_time(RTC, HOUR, MINUTE, SECOND);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 0);
	NVIC_EnableIRQ(RTC_IRQn);
	
	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(RTC,  RTC_IER_ALREN);
	
}
/************************************************************************/
/* Main                                                                 */
/************************************************************************/
/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{

	/* Initialize the SAM system. */
	sysclk_init();
  ioport_init();
  board_init();
  
  /* inicializa delay */
  delay_init(sysclk_get_cpu_hz());

  /* inicializa console (printf) */
	configure_console();
  
	/* Output example information. */
	puts(STRING_HEADER);

  /************************************* 
   * Ativa e configura AFEC
   *************************************/  

  /* Ativa AFEC - 0 */
	afec_enable(AFEC0);

  /* struct de configuracao do AFEC */
	struct afec_config afec_cfg;

  /* Carrega parametros padrao */
	afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
	afec_init(AFEC0, &afec_cfg);
  
  /* Configura trigger por software */
  afec_set_trigger(AFEC0, AFEC_TRIG_SW);
  
  /* configura call back */
 	afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_11,	AFEC_Temp_callback, 1); 
   
  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(AFEC0, AFEC_CHANNEL_TEMP_SENSOR, &afec_ch_cfg);
  
  /*
   * Calibracao:
	 * Because the internal ADC offset is 0x200, it should cancel it and shift
	 * down to 0.
	 */
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_TEMP_SENSOR, 0x200);

  /***  Configura sensor de temperatura ***/
	struct afec_temp_sensor_config afec_temp_sensor_cfg;

	afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
	afec_temp_sensor_set_config(AFEC0, &afec_temp_sensor_cfg);

  /* Selecina canal e inicializa conversão */  
	afec_channel_enable(AFEC0, AFEC_CHANNEL_TEMP_SENSOR);
	afec_start_software_conversion(AFEC0);
	TC_init(TC0, ID_TC0, 0, 1);
	RTC_init();
	/* configura alarme do RTC */
	rtc_set_date_alarm(RTC, 1, MOUNTH, 1, DAY);
	rtc_set_time_alarm(RTC, 1, HOUR, 1, MINUTE, 1, SECOND+1);

	while (1) {
		if(is_conversion_done == true) {
			is_conversion_done = false;
      
      printf("Temp : %d \r\n", convert_adc_to_temp(g_ul_value) );
      afec_start_software_conversion(AFEC0);
      delay_s(1);
		}
	}
}
