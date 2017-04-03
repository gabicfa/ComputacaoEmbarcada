#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

/** IRQ priority for PIO (The lower the value, the greater the priority) */
// [main_def_pio_irq_prior]
#define IRQ_PRIOR_PIO    0
// [main_def_pio_irq_prior]

/** LED0 blink time, LED1 blink half this time, in ms */
#define BLINK_PERIOD     1000

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Getting Started Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL
		
int mseconds_to_wait=500;

/** LED0 blinking control. */
// [main_var_led0_control]
volatile bool g_b_led0_active = true;
// [main_var_led0_control]

#ifdef LED1_GPIO
/** LED1 blinking control. */
// [main_var_led1_control]
volatile bool g_b_led1_active = true;
// [main_var_led1_control]
#endif

/** Global g_ul_ms_ticks in milliseconds since start of application */
// [main_var_ticks]
volatile uint32_t g_ul_ms_ticks = 0;
// [main_var_ticks]

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 *  \brief Process 
 s Events
 *
 *  Change active states of LEDs when corresponding button events happened.
 */
static void ProcessButtonEvt(uint8_t uc_button)
{
// [main_button1_evnt_process]
	if (uc_button == 0) {
		//g_b_led0_active = !g_b_led0_active;
		if(mseconds_to_wait == 500){
			mseconds_to_wait= 100;
		}
		else{
			mseconds_to_wait=500;
		}
		if (!g_b_led0_active) {
			ioport_set_pin_level(LED0_GPIO, IOPORT_PIN_LEVEL_HIGH);
		}
	}
// [main_button1_evnt_process]
#ifdef LED1_GPIO 
	else {
// [main_button2_evnt_process]
		g_b_led1_active = !g_b_led1_active;

		/* Enable LED#2 and TC if they were enabled */
		if (g_b_led1_active) {
			ioport_set_pin_level(LED1_GPIO, IOPORT_PIN_LEVEL_LOW);
			tc_start(TC0, 0);
		}
		/* Disable LED#2 and TC if they were disabled */
		else {
			ioport_set_pin_level(LED1_GPIO, IOPORT_PIN_LEVEL_HIGH);
			tc_stop(TC0, 0);
		}
// [main_button2_evnt_process]
	}
#endif
}

/**
 *  \brief Handler for System Tick interrupt.
 *
 *  Process System Tick Event
 *  Increments the g_ul_ms_ticks counter.
 */
// [main_systick_handler]
void SysTick_Handler(void)
{
	g_ul_ms_ticks++;
}
// [main_systick_handler]

/**
 *  \brief Handler for Button 1 rising edge interrupt.
 *
 *  Handle process led1 status change.
 */
// [main_button1_handler]
static void Button1_Handler(uint32_t id, uint32_t mask)
{
	if (PIN_PUSHBUTTON_1_ID == id && PIN_PUSHBUTTON_1_MASK == mask) {
		ProcessButtonEvt(0);
	}
}
// [main_button1_handler]

#ifndef BOARD_NO_PUSHBUTTON_2
/**
 *  \brief Handler for Button 2 falling edge interrupt.
 *
 *  Handle process led2 status change.
 */
// [main_button2_handler] 
static void Button2_Handler(uint32_t id, uint32_t mask)
{
	if (PIN_PUSHBUTTON_2_ID == id && PIN_PUSHBUTTON_2_MASK == mask) {
		ProcessButtonEvt(1);
	}
}
// [main_button2_handler]
#endif

/**
 *  \brief Configure the Pushbuttons
 *
 *  Configure the PIO as inputs and generate corresponding interrupt when
 *  pressed or released.
 */
static void configure_buttons(void)
{
// [main_button1_configure]
	/* Configure Pushbutton 1 */
	pmc_enable_periph_clk(PIN_PUSHBUTTON_1_ID);
	pio_set_debounce_filter(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK, 10);
	/* Interrupt on rising edge  */
	pio_handler_set(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_ID,
			PIN_PUSHBUTTON_1_MASK, PIN_PUSHBUTTON_1_ATTR, Button1_Handler);
	NVIC_EnableIRQ((IRQn_Type) PIN_PUSHBUTTON_1_ID);
	pio_handler_set_priority(PIN_PUSHBUTTON_1_PIO,
			(IRQn_Type) PIN_PUSHBUTTON_1_ID, IRQ_PRIOR_PIO);
	pio_enable_interrupt(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK);
// [main_button1_configure]
#ifndef BOARD_NO_PUSHBUTTON_2
// [main_button2_configure]
	/* Configure Pushbutton 2 */
	pmc_enable_periph_clk(PIN_PUSHBUTTON_2_ID);
	pio_set_debounce_filter(PIN_PUSHBUTTON_2_PIO, PIN_PUSHBUTTON_2_MASK, 10);
	/* Interrupt on falling edge */
	pio_handler_set(PIN_PUSHBUTTON_2_PIO, PIN_PUSHBUTTON_2_ID,
			PIN_PUSHBUTTON_2_MASK, PIN_PUSHBUTTON_2_ATTR, Button2_Handler);
	NVIC_EnableIRQ((IRQn_Type) PIN_PUSHBUTTON_2_ID);
	pio_handler_set_priority(PIN_PUSHBUTTON_2_PIO,
			(IRQn_Type) PIN_PUSHBUTTON_2_ID, IRQ_PRIOR_PIO);
	pio_enable_interrupt(PIN_PUSHBUTTON_2_PIO, PIN_PUSHBUTTON_2_MASK);
// [main_button2_configure]
#endif
}

/**
 *  Interrupt handler for TC0 interrupt. Toggles the state of LED\#2.
 */
// [main_tc0_handler]
#ifndef BOARD_NO_LED_1
void TC0_Handler(void)
{
	volatile uint32_t ul_dummy;

	/* Clear status bit to acknowledge interrupt */
	ul_dummy = tc_get_status(TC0, 0);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

#ifdef LED1_GPIO
	/** Toggle LED state. */
	ioport_toggle_pin_level(LED1_GPIO);
#endif

	printf("2 ");
}
// [main_tc0_handler]

/**
 *  Configure Timer Counter 0 to generate an interrupt every 250ms.
 */
// [main_tc_configure]
static void configure_tc(void)
{
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configure PMC */
	pmc_enable_periph_clk(ID_TC0);
#if SAMG55
	/* Enable PCK output */
	pmc_disable_pck(PMC_PCK_3);
	pmc_switch_pck_to_sclk(PMC_PCK_3, PMC_PCK_PRES(0));
#endif

	/** Configure TC for a 4Hz frequency and trigger on RC compare. */
	tc_find_mck_divisor(4, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC0, 0, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC0, 0, (ul_sysclk / ul_div) / 4);

	/* Configure and enable interrupt on RC compare */
	NVIC_EnableIRQ((IRQn_Type) ID_TC0);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);

#ifdef LED1_GPIO
	/** Start the counter if LED1 is enabled. */
	if (g_b_led1_active) {
		tc_start(TC0, 0);
	}
#else
	tc_start(TC0, 0);
#endif
}
#endif
// [main_tc_configure]

/**
 *  Configure UART console.
 */
// [main_console_configure]
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

// [main_console_configure]

/**
 * \brief Wait for the given number of milliseconds (using the g_ul_ms_ticks
 * generated by the SAM's microcontrollers's system tick).
 *
 * \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
// [main_ms_delay]
static void mdelay(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}
// [main_ms_delay]

/**
 *  \brief getting-started Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
// [main]
int main(void)
{
//! [main_step_sys_init]
	/* Initialize the SAM system */
	sysclk_init();
	board_init();
//! [main_step_sys_init]

#ifndef BOARD_NO_PUSHBUTTON_2
#if (SAMV71 || SAMV70 || SAMS70 || SAME70)
	if (GPIO_PUSH_BUTTON_2 == PIO_PB12_IDX) {
		matrix_set_system_io(matrix_get_system_io() | CCFG_SYSIO_SYSIO12);
	}
	ioport_set_pin_dir(GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(GPIO_PUSH_BUTTON_2, GPIO_PUSH_BUTTON_2_FLAGS);
	ioport_set_pin_sense_mode(GPIO_PUSH_BUTTON_2, GPIO_PUSH_BUTTON_2_SENSE);
#endif
#endif
//! [main_step_console_init]
	/* Initialize the console uart */
	configure_console();
//! [main_step_console_init]

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure systick for 1 ms */
	puts("Configure system tick to get 1ms tick period.\r");
//! [main_step_systick_init]
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-F- Systick configuration error\r");
		while (1);
	}
//! [main_step_systick_init]

#ifndef BOARD_NO_LED_1
	puts("Configure TC.\r");
//! [main_step_tc_init]
	configure_tc();
//! [main_step_tc_init]
#endif

	puts("Configure buttons with debouncing.\r");
//! [main_step_btn_init]
	configure_buttons();
//! [main_step_btn_init]

	printf("Press %s to Start/Stop the %s blinking.\r\n",
			PUSHBUTTON_1_NAME, LED_0_NAME);

#ifndef BOARD_NO_PUSHBUTTON_2
	printf("Press %s to Start/Stop the %s blinking.\r\n",
			PUSHBUTTON_2_NAME, LED_1_NAME);
#endif

//! [main_step_loop]
	while (1) {
		/* Wait for LED to be active */
		while (!g_b_led0_active);

		/* Toggle LED state if active */
		if (g_b_led0_active) {
			ioport_toggle_pin_level(LED0_GPIO);
			int frequencia =  sysclk_get_cpu_hz() ;
			printf("1 ");
		}

		/* Wait for 500ms */
		mdelay(mseconds_to_wait);
	}
//! [main_step_loop]
}
// [main]
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond