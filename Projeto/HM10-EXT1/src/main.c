#include "asf.h"

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/
/**
 *  Informacoes para o RTC
 *  poderia ser extraida do __DATE__ e __TIME__
 *  ou ser atualizado pelo PC.
 */
#define YEAR        2017
#define MOUNTH      3
#define DAY         27
#define WEEK        13
#define HOUR        9
#define MINUTE      5
#define SECOND      0

/**
 * LEDs
 */
#define LED_PIO_ID		ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		    8
#define LED_PIN_MASK    (1<<LED_PIN)

/**
 * Botão
 */
#define BUT_PIO_ID      ID_PIOA
#define BUT_PIO         PIOA
#define BUT_PIN		    11
#define BUT_PIN_MASK    (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79


/** XDMAC channel used in this example. */
#define XDMAC_TX_CH 0
#define XDMAC_RX_CH 1

/** XDMAC channel HW Interface number for SPI0,refer to datasheet. */
#define USART_XDMAC_TX_CH_NUM  XDAMC_CHANNEL_HWID_USART1_TX
#define USART_XDMAC_RX_CH_NUM  XDAMC_CHANNEL_HWID_USART1_RX

/** XDMAC channel configuration. */
static xdmac_channel_config_t xdmac_tx_cfg,xdmac_rx_cfg;

/** The buffer size for transfer  */
#define BUFFER_SIZE          100

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/
volatile uint8_t flag_led0 = 1;

/** 
 * XDMA
 */
uint8_t tx_buffer[]             = "This is message from SPI master transferred by XDMAC test";
uint8_t rx_buffer[BUFFER_SIZE]  = "0";
uint32_t g_size                 = sizeof(tx_buffer);

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void BUT_init(void);
void LED_init(int estado);
void TC1_init(void);
void RTC_init(void);
void pin_toggle(Pio *pio, uint32_t mask);
static void uart_xdmac_configure(Usart *const pusart);


/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
 *  Handle Interrupcao botao 1
 */
static void Button1_Handler(uint32_t id, uint32_t mask)
{
  pin_toggle(PIOD, (1<<28));
  pin_toggle(LED_PIO, LED_PIN_MASK);
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/** 
 *  Toggle pin controlado pelo PIO (out)
 */
void pin_toggle(Pio *pio, uint32_t mask){
   if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
   else
    pio_set(pio,mask);
}

/**
 * @Brief Inicializa o pino do BUT
 */
void BUT_init(void){
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(BUT_PIO_ID);
    pio_set_input(BUT_PIO, BUT_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    
    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(BUT_PIO, BUT_PIN_MASK);
    pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIN_MASK, PIO_IT_FALL_EDGE, Button1_Handler);
    
    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(BUT_PIO_ID);
    NVIC_SetPriority(BUT_PIO_ID, 1);
};

/**
 * @Brief Inicializa o pino do LED
 */
void LED_init(int estado){
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_set_output(LED_PIO, LED_PIN_MASK, estado, 0, 0 );
};

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate =		CONF_UART_BAUDRATE,
		.charlength =	CONF_UART_CHAR_LENGTH,
		.paritytype =	CONF_UART_PARITY,
		.stopbits =		CONF_UART_STOP_BITS,
	};

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}


/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void){
  
      char buf[100];

  
	  /* Initialize the SAM system */
	  sysclk_init();
    
    /* Disable the watchdog */
	  WDT->WDT_MR = WDT_MR_WDDIS;
    
    board_init();

    /* Configura Leds */
    LED_init(1);
	
	  /* Configura os botões */
	  BUT_init();  
  
    
    /* delay */
    uint32_t cpuFreq = sysclk_get_cpu_hz();
    delay_init(cpuFreq);
   
    /* CONFIGURE USART0 */
    sysclk_enable_peripheral_clock(ID_PIOB);
    pio_set_peripheral(PIOB, PIO_PERIPH_C, PIO_PB0);
    pio_set_peripheral(PIOB, PIO_PERIPH_C, PIO_PB1);
          
    const usart_serial_options_t uart_serial_options = {
      	.baudrate =		9600,
      	.charlength =	US_MR_CHRL_8_BIT,
      	.paritytype =	US_MR_PAR_NO,
      	.stopbits =		US_MR_NBSTOP_1_BIT,
    	};
      
   	sysclk_enable_peripheral_clock(ID_USART0);
    stdio_serial_init(USART0, &uart_serial_options);
    
   // pmc_enable_periph_clk(ID_PIOD);
   // pio_set_output(PIOD, (1<<28), 0, 0, 0 );    
   // pio_clear(PIOD, (1<<28));
   
   	//sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
   	//stdio_serial_init(CONF_UART, & _serial_options);

   // usart_putchar(USART0, 0xFA);
    
   // while(usart_read(USART0, &buf[0]);)
    //usart_read(USART0, &buf[0]);

  char bufferRX[100];

	while (1) {
	/* Entra em modo sleep */
		 //printf("AT+ROLE=1\r\n");
    printf("%s \n", "Ola voce" );
    gets(bufferRX);
    //usart_putchar(USART0, 0xFA);
    //printf("%AT\r\n");
    delay_ms(500);


	}
}
