
#ifndef _TLSR8269_H
#define _TLSR8269_H

// PS, these are just a place holder for TC32 registers
// 

#define REG8  (volatile unsigned char*)
#define REG16 (volatile unsigned short*)
#define REG32 (volatile unsigned int*)


/*##############################################################################
## MISC
##############################################################################*/



/*##############################################################################
## GPIO - General Purpose I/O
##############################################################################*/

#define GPIO_IOPIN      (*(REG32 (0xE0028000))) /* ALTERNATE NAME GPIO = GPIO0 */
#define GPIO_IOSET      (*(REG32 (0xE0028004)))
#define GPIO_IODIR      (*(REG32 (0xE0028008)))
#define GPIO_IOCLR      (*(REG32 (0xE002800C)))

#define GPIO0_IOPIN     (*(REG32 (0xE0028000))) /* ALTERNATE NAME GPIO = GPIO0 */
#define GPIO0_IOSET     (*(REG32 (0xE0028004)))
#define GPIO0_IODIR     (*(REG32 (0xE0028008)))
#define GPIO0_IOCLR     (*(REG32 (0xE002800C)))


/*##############################################################################
## UART0 / UART1
##############################################################################*/


/*##############################################################################
## I2C
##############################################################################*/

#define I2C_I2CONSET    (*(REG32 (0xE001C000)))
#define I2C_I2STAT      (*(REG32 (0xE001C004)))
#define I2C_I2DAT       (*(REG32 (0xE001C008)))
#define I2C_I2ADR       (*(REG32 (0xE001C00C)))
#define I2C_I2SCLH      (*(REG32 (0xE001C010)))
#define I2C_I2SCLL      (*(REG32 (0xE001C014)))
#define I2C_I2CONCLR    (*(REG32 (0xE001C018)))


/*##############################################################################
## SPI - Serial Peripheral Interface
##############################################################################*/

#define SPI_SPCR        (*(REG32 (0xE0020000)))
#define SPI_SPSR        (*(REG32 (0xE0020004)))
#define SPI_SPDR        (*(REG32 (0xE0020008)))
#define SPI_SPCCR       (*(REG32 (0xE002000C)))
#define SPI_SPTCR       (*(REG32 (0xE0020010)))
#define SPI_SPTSR       (*(REG32 (0xE0020014)))
#define SPI_SPTOR       (*(REG32 (0xE0020018)))
#define SPI_SPINT       (*(REG32 (0xE002001C)))


/*##############################################################################
## Timer 0 and Timer 1
##############################################################################*/


/*##############################################################################
## PWM
##############################################################################*/

#define PWM_IR          (*(REG32 (0xE0014000)))
#define PWM_TCR         (*(REG32 (0xE0014004)))
#define PWM_TC          (*(REG32 (0xE0014008)))
#define PWM_PR          (*(REG32 (0xE001400C)))
#define PWM_PC          (*(REG32 (0xE0014010)))
#define PWM_MCR         (*(REG32 (0xE0014014)))
#define PWM_MR0         (*(REG32 (0xE0014018)))
#define PWM_MR1         (*(REG32 (0xE001401C)))
#define PWM_MR2         (*(REG32 (0xE0014020)))
#define PWM_MR3         (*(REG32 (0xE0014024)))
#define PWM_MR4         (*(REG32 (0xE0014040)))
#define PWM_MR5         (*(REG32 (0xE0014044)))
#define PWM_MR6         (*(REG32 (0xE0014048)))
#define PWM_EMR         (*(REG32 (0xE001403C)))
#define PWM_PCR         (*(REG32 (0xE001404C)))
#define PWM_LER         (*(REG32 (0xE0014050)))
#define PWM_CCR         (*(REG32 (0xE0014028)))
#define PWM_CR0         (*(REG32 (0xE001402C)))
#define PWM_CR1         (*(REG32 (0xE0014030)))
#define PWM_CR2         (*(REG32 (0xE0014034)))
#define PWM_CR3         (*(REG32 (0xE0014038)))

/*##############################################################################
## RTC
##############################################################################*/


/*##############################################################################
## WD - Watchdog
##############################################################################*/

#define WD_WDMOD        (*(REG32 (0xE0000000)))
#define WD_WDTC         (*(REG32 (0xE0000004)))
#define WD_WDFEED       (*(REG32 (0xE0000008)))
#define WD_WDTV         (*(REG32 (0xE000000C)))


/*##############################################################################
## System Control Block
##############################################################################*/

#define SCB_EXTINT      (*(REG32 (0xE01FC140)))
#define SCB_EXTWAKE     (*(REG32 (0xE01FC144)))
#define SCB_MEMMAP      (*(REG32 (0xE01FC040)))
#define SCB_PLLCON      (*(REG32 (0xE01FC080)))
#define SCB_PLLCFG      (*(REG32 (0xE01FC084)))
#define SCB_PLLSTAT     (*(REG32 (0xE01FC088)))
#define SCB_PLLFEED     (*(REG32 (0xE01FC08C)))
#define SCB_PCON        (*(REG32 (0xE01FC0C0)))
#define SCB_PCONP       (*(REG32 (0xE01FC0C4)))
#define SCB_VPBDIV      (*(REG32 (0xE01FC100)))


#endif /* 8269_h */

