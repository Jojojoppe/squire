#include "../../../../common/arch/debugprint.h"
#include "config.h"
#include "../../vas.h"

// UART is located at 0x00401000/4KB

#ifdef KERNEL_LOG_PRINT_ENABLE

#define XUARTPS_CR_TXRST	0x00000002U  /**< TX logic reset */
#define XUARTPS_CR_RXRST	0x00000001U  /**< RX logic reset */

#define XUARTPS_CR_OFFSET       0x0000U  /**< Control Register [8:0] */
#define XUARTPS_MR_OFFSET       0x0004U  /**< Mode Register [9:0] */
#define XUARTPS_BAUDGEN_OFFSET  0x0018U  /**< Baud Rate Generator [15:0] */
#define XUARTPS_BAUDDIV_OFFSET  0x0034U  /**< Baud Rate Divider [7:0] */
#define XUARTPS_FIFO_OFFSET     0x0030U  /**< FIFO [7:0] */
#define XUARTPS_SR_OFFSET       0x002CU  /**< Channel Status [14:0] */

#define XUARTPS_MR_CHMODE_NORM		0x00000000U /**< Normal mode */
#define XUARTPS_MR_STOPMODE_1_BIT	0x00000000U /**< 1 stop bit */
#define XUARTPS_MR_PARITY_NONE		0x00000020U /**< No parity mode */
#define XUARTPS_MR_CHARLEN_8_BIT	0x00000000U /**< 8 bits data */
#define XUARTPS_MR_CLKSEL			0x00000001U /**< Input clock selection */

#define XUARTPS_SR_TNFUL	0x00004000U /**< TX FIFO Nearly Full Status */
#define XUARTPS_SR_TACTIVE	0x00000800U /**< TX active */
#define XUARTPS_SR_RXEMPTY	0x00000002U /**< RX FIFO empty */

#define XUARTPS_CR_TX_DIS	0x00000020U  /**< TX disabled. */
#define XUARTPS_CR_TX_EN	0x00000010U  /**< TX enabled */
#define XUARTPS_CR_RX_DIS	0x00000008U  /**< RX disabled. */
#define XUARTPS_CR_RX_EN	0x00000004U  /**< RX enabled */

#define XUARTPS_IER_OFFSET  0x00000008
#define XUARTPS_IMR_OFFSET  0x00000010

#define POINTER_TO_REGISTER(REG)  ( *((volatile unsigned int*)(REG)))

#define XPS_UART1_BASEADDR      0xe0001000U
#define UART_VIRT_OFFSET        (0xe0000000U - 0x00400000U)

#define UART_BASE      (XPS_UART1_BASEADDR-UART_VIRT_OFFSET)
#define UART_CTRL      POINTER_TO_REGISTER(UART_BASE + XUARTPS_CR_OFFSET)      // Control Register
#define UART_MODE      POINTER_TO_REGISTER(UART_BASE + XUARTPS_MR_OFFSET)      // Mode Register

#define UART_BAUD_GEN  POINTER_TO_REGISTER(UART_BASE + XUARTPS_BAUDGEN_OFFSET) // Baud Rate Generator "CD"
#define UART_BAUD_DIV  POINTER_TO_REGISTER(UART_BASE + XUARTPS_BAUDDIV_OFFSET) // Baud Rate Divider "BDIV"
#define UART_FIFO      POINTER_TO_REGISTER(UART_BASE + XUARTPS_FIFO_OFFSET)    // FIFO
#define UART_STATUS    POINTER_TO_REGISTER(UART_BASE + XUARTPS_SR_OFFSET)      // Channel Status

#define UART_IER        POINTER_TO_REGISTER(UART_BASE + XUARTPS_IER_OFFSET)
#define UART_IMR        POINTER_TO_REGISTER(UART_BASE + XUARTPS_IMR_OFFSET)

void arch_debugprint_putc(char c){
    while (UART_STATUS & XUARTPS_SR_TNFUL);
    UART_FIFO = c;
    while (UART_STATUS & XUARTPS_SR_TACTIVE);
}

void arch_debugprint_init(){

    arch_vas_map(XPS_UART1_BASEADDR, UART_BASE, VAS_FLAGS_KWRITE);

    while (UART_STATUS & XUARTPS_SR_TACTIVE);
    while (UART_STATUS & XUARTPS_SR_TNFUL);

    unsigned int r = 0; // Temporary value variable
    r = UART_CTRL;
    r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
    r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
    UART_CTRL = r;

    UART_MODE = 0;
    UART_MODE &= ~XUARTPS_MR_CLKSEL; // Clear "Input clock selection" - 0: clock source is uart_ref_clk
    UART_MODE |= XUARTPS_MR_CHARLEN_8_BIT; 	// Set "8 bits data"
    UART_MODE |= XUARTPS_MR_PARITY_NONE; 	// Set "No parity mode"
    UART_MODE |= XUARTPS_MR_STOPMODE_1_BIT; // Set "1 stop bit"
    UART_MODE |= XUARTPS_MR_CHMODE_NORM; 	// Set "Normal mode"

    // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
    UART_BAUD_DIV = 6; // ("BDIV")
    UART_BAUD_GEN = 124; // ("CD")
    // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps

    UART_CTRL |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

    r = UART_CTRL;
    r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
    r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
    UART_CTRL = r;
}

#endif
