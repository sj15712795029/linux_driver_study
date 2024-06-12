#include <stdlib.h>
#include <stdint.h>

#include "stm32f4_regs.h"

#define CONFIG_HSE_HZ	8000000
#define CONFIG_PLL_M	8
#define CONFIG_PLL_N	336
#define CONFIG_PLL_P	2
#define CONFIG_PLL_Q	7
#define PLLCLK_HZ (((CONFIG_HSE_HZ / CONFIG_PLL_M) * CONFIG_PLL_N) / CONFIG_PLL_P)
#if PLLCLK_HZ == 168000000
#define FLASH_LATENCY	5
#else
#error PLL clock does not match 168 MHz
#endif

static void *gpio_base = (void *)GPIOA_BASE;

static void clock_setup(void)
{
	volatile uint32_t *RCC_CR = (void *)(RCC_BASE + 0x00);
	volatile uint32_t *RCC_PLLCFGR = (void *)(RCC_BASE + 0x04);
	volatile uint32_t *RCC_CFGR = (void *)(RCC_BASE + 0x08);
	volatile uint32_t *FLASH_ACR = (void *)(FLASH_BASE + 0x00);
	volatile uint32_t *RCC_AHB1ENR = (void *)(RCC_BASE + 0x30);
	volatile uint32_t *RCC_AHB2ENR = (void *)(RCC_BASE + 0x34);
	volatile uint32_t *RCC_AHB3ENR = (void *)(RCC_BASE + 0x38);
	volatile uint32_t *RCC_APB1ENR = (void *)(RCC_BASE + 0x40);
	volatile uint32_t *RCC_APB2ENR = (void *)(RCC_BASE + 0x44);
	volatile uint32_t *RCC_AHB1LPENR= (void *)(RCC_BASE + 0x50);
	uint32_t val;
	
	*RCC_CR |= RCC_CR_HSEON;
	while (!(*RCC_CR & RCC_CR_HSERDY)) {
	}
	
	val = *RCC_CFGR;
	val &= ~RCC_CFGR_HPRE_MASK;
	//val |= 0 << 4; // not divided
	val &= ~RCC_CFGR_PPRE1_MASK;
	val |= 0x5 << 10; // divided by 4
	val &= ~RCC_CFGR_PPRE2_MASK;
	val |= 0x4 << 13; // divided by 2
	*RCC_CFGR = val;
	
	val = 0;
	val |= RCC_PLLCFGR_PLLSRC_HSE;
	val |= CONFIG_PLL_M;
	val |= CONFIG_PLL_N << 6;
	val |= ((CONFIG_PLL_P >> 1) - 1) << 16;
	val |= CONFIG_PLL_Q << 24;
	*RCC_PLLCFGR = val;
	
	*RCC_CR |= RCC_CR_PLLON;
	while (!(*RCC_CR & RCC_CR_PLLRDY));
	
	*FLASH_ACR = FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_LATENCY;
	
	*RCC_CFGR &= ~RCC_CFGR_SW_MASK;
	*RCC_CFGR |= RCC_CFGR_SW_PLL;
	while ((*RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL) {
	}
	
	/*  Enable all clocks, unused ones will be gated at end of kernel boot */
	*RCC_AHB1ENR |= 0x7E7411FF;
	*RCC_AHB2ENR |= 0xF1;
	*RCC_AHB3ENR |= 0x1;
	*RCC_APB1ENR |= 0x36FEC9FF;
	*RCC_APB2ENR |= 0x75F33;
	
	
}

static void sram_init(void)
{
	/* GPIO AF */
	gpio_set_fmc(gpio_base, 'F', 0); // PF0   ------> FSMC_A0
	gpio_set_fmc(gpio_base, 'F', 1); // PF1   ------> FSMC_A1
	gpio_set_fmc(gpio_base, 'F', 2); // PF2   ------> FSMC_A2
	gpio_set_fmc(gpio_base, 'F', 3); // PF3   ------> FSMC_A3
	gpio_set_fmc(gpio_base, 'F', 4); // PF4   ------> FSMC_A4
	gpio_set_fmc(gpio_base, 'F', 5); // PF5   ------> FSMC_A5
	gpio_set_fmc(gpio_base, 'F', 12); // PF12   ------> FSMC_A6
	gpio_set_fmc(gpio_base, 'F', 13); // PF13   ------> FSMC_A7
	gpio_set_fmc(gpio_base, 'F', 14); // PF14   ------> FSMC_A8
	gpio_set_fmc(gpio_base, 'F', 15); // PF15   ------> FSMC_A9
	gpio_set_fmc(gpio_base, 'G', 0); // PG0   ------> FSMC_A10
	gpio_set_fmc(gpio_base, 'G', 1); // PG1   ------> FSMC_A11
	gpio_set_fmc(gpio_base, 'G', 2); // PG2   ------> FSMC_A12
	gpio_set_fmc(gpio_base, 'G', 3); // PG3   ------> FSMC_A13
	gpio_set_fmc(gpio_base, 'G', 4); // PG4   ------> FSMC_A14
	gpio_set_fmc(gpio_base, 'G', 5); // PG5   ------> FSMC_A15
	gpio_set_fmc(gpio_base, 'D', 14); // PD14   ------> FSMC_D0
	gpio_set_fmc(gpio_base, 'D', 15); // PD15   ------> FSMC_D1
	gpio_set_fmc(gpio_base, 'D', 0); // PD0   ------> FSMC_D2
	gpio_set_fmc(gpio_base, 'D', 1); // PD1   ------> FSMC_D3
	gpio_set_fmc(gpio_base, 'E', 7); // PE7   ------> FSMC_D4
	gpio_set_fmc(gpio_base, 'E', 8); // PE8   ------> FSMC_D5
	gpio_set_fmc(gpio_base, 'E', 9); // PE9   ------> FSMC_D6
	gpio_set_fmc(gpio_base, 'E', 10); // PE10   ------> FSMC_D7
	gpio_set_fmc(gpio_base, 'E', 11); // PE11   ------> FSMC_D8
	gpio_set_fmc(gpio_base, 'E', 12); // PE12   ------> FSMC_D9
	gpio_set_fmc(gpio_base, 'E', 13); // PE13   ------> FSMC_D10
	gpio_set_fmc(gpio_base, 'E', 14); // PE14   ------> FSMC_D11
	gpio_set_fmc(gpio_base, 'E', 15); // PE15   ------> FSMC_D12
	gpio_set_fmc(gpio_base, 'D', 8); // PD8   ------> FSMC_D13
	gpio_set_fmc(gpio_base, 'D', 9); // PD9   ------> FSMC_D14
	gpio_set_fmc(gpio_base, 'D', 10); // PD10   ------> FSMC_D15
	gpio_set_fmc(gpio_base, 'D', 4); // PD4   ------> FSMC_NOE
	gpio_set_fmc(gpio_base, 'D', 5); // PD5   ------> FSMC_NWE
	gpio_set_fmc(gpio_base, 'G', 10); // PG10   ------> FSMC_NE3
	gpio_set_fmc(gpio_base, 'G', 12); // PG12   ------> FSMC_NE4
	gpio_set_fmc(gpio_base, 'E', 0); // PE0   ------> FSMC_NBL0
	gpio_set_fmc(gpio_base, 'E', 1); // PE1   ------> FSMC_NBL1
}

int main(void)
{
	volatile uint32_t *FLASH_KEYR = (void *)(FLASH_BASE + 0x04);
	volatile uint32_t *FLASH_CR = (void *)(FLASH_BASE + 0x10);
	
	if (*FLASH_CR & FLASH_CR_LOCK) {
		*FLASH_KEYR = 0x45670123;
		*FLASH_KEYR = 0xCDEF89AB;
	}
	
	clock_setup();

}

static void noop(void)
{
	usart_putch(usart_base, 'E');
	while (1) {
	}
}

extern unsigned int _end_text;
extern unsigned int _start_data;
extern unsigned int _end_data;
extern unsigned int _start_bss;
extern unsigned int _end_bss;

void reset(void)
{
	unsigned int *src, *dst;

	asm volatile ("cpsid i");

	src = &_end_text;
	dst = &_start_data;
	while (dst < &_end_data) {
		*dst++ = *src++;
	}

	dst = &_start_bss;
	while (dst < &_end_bss) {
		*dst++ = 0;
	}

	main();
}

extern unsigned long _stack_top;

__attribute__((section(".vector_table")))
void (*vector_table[16 + 91])(void) = {
	(void (*))&_stack_top,
	reset,
	noop,
	noop,
	noop,
	noop,
	noop,
	NULL,
	NULL,
	NULL,
	NULL,
	noop,
	noop,
	NULL,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
	noop,
};