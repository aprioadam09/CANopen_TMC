#include "can.h"
#include "stm32f4xx.h"
#include "rcc.h"
#include "gpio.h"
#include <string.h>
#include <stdbool.h>

// --- Ring Buffer for CAN message reception (logic copied from PoC) ---
#define CAN_RX_BUFFER_SIZE 32
static struct can_msg rx_buffer[CAN_RX_BUFFER_SIZE];
static volatile uint32_t rx_head = 0;
static volatile uint32_t rx_tail = 0;

void can_init(bool loopback_mode) {
    // 1. Enable Clocks
    rcc_gpio_port_clock_enable(GPIOB);
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

    // 2. Configure GPIO pins
    // CAN_RX: PB8, CAN_TX: PB9. Both use Alternate Function 9.
    gpio_configure_alternate_function(GPIOB, 8, 9);
    gpio_configure_alternate_function(GPIOB, 9, 9);

    // 3. Configure CAN Peripheral
    // Enter initialization mode
    CAN1->MCR |= CAN_MCR_INRQ;
    while ((CAN1->MSR & CAN_MSR_INAK) == 0); // Wait for acknowledgment

    // Exit sleep mode
    CAN1->MCR &= ~CAN_MCR_SLEEP;

    // Set CAN options
    CAN1->MCR |= CAN_MCR_ABOM; // Automatic Bus-Off Management
    CAN1->MCR &= ~CAN_MCR_TTCM; // Time Triggered Mode disabled
    CAN1->MCR &= ~CAN_MCR_AWUM; // Automatic Wakeup Mode disabled
    CAN1->MCR &= ~CAN_MCR_NART; // No Automatic Retransmission
    CAN1->MCR &= ~CAN_MCR_RFLM; // Receive FIFO Locked Mode disabled
    CAN1->MCR &= ~CAN_MCR_TXFP; // Transmit FIFO Priority disabled

    // 4. Set Baud Rate to 125 kbps
    // APB1 clock is 42MHz. 42MHz / (14 * (1+10+3)) = 125kHz
    // Prescaler = 24, TS1 = 10, TS2 = 3, SJW = 1
    CAN1->BTR = (0U << 24)        // SJW[1:0] = 0 (1 quantum)
			  | (1U << 20)        // TS2[2:0] = 1 (2 quanta)
			  | (10U << 16)       // TS1[3:0] = 10 (11 quanta)
			  | (23U << 0);       // BRP[9:0] = 23 (Prescaler 24)

    if (loopback_mode) {
            CAN1->BTR |= CAN_BTR_LBKM;
        }

    // 5. Configure CAN Filters (Accept all messages)
    CAN1->FMR |= CAN_FMR_FINIT; // Enter filter initialization mode

    CAN1->sFilterRegister[0].FR1 = 0x00000000; // Filter 0, ID
    CAN1->sFilterRegister[0].FR2 = 0x00000000; // Filter 0, Mask
    CAN1->FM1R &= ~(1 << 0);   // Set filter 0 to Mask mode
    CAN1->FS1R |= (1 << 0);    // Set filter 0 to 32-bit scale
    CAN1->FFA1R &= ~(1 << 0);  // Assign filter 0 to FIFO 0
    CAN1->FA1R |= (1 << 0);    // Activate filter 0

    CAN1->FMR &= ~CAN_FMR_FINIT; // Leave filter initialization mode

    // 6. Enable Interrupts
    CAN1->IER |= CAN_IER_FMPIE0; // FIFO 0 Message Pending Interrupt Enable
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_SetPriority(CAN1_RX0_IRQn, 5); // Set a moderate priority

    // 7. Leave initialization mode and start CAN
    CAN1->MCR &= ~CAN_MCR_INRQ;
    while ((CAN1->MSR & CAN_MSR_INAK) != 0); // Wait for acknowledgment
}

size_t can_send(const struct can_msg *msg) {
    uint32_t transmit_mailbox;

    // Check if any transmit mailbox is empty
    if ((CAN1->TSR & CAN_TSR_TME0) == CAN_TSR_TME0) {
        transmit_mailbox = 0;
    } else if ((CAN1->TSR & CAN_TSR_TME1) == CAN_TSR_TME1) {
        transmit_mailbox = 1;
    } else if ((CAN1->TSR & CAN_TSR_TME2) == CAN_TSR_TME2) {
        transmit_mailbox = 2;
    } else {
        return 0; // No mailbox available
    }

    // Set up the ID and DLC
    CAN1->sTxMailBox[transmit_mailbox].TIR = (msg->id << 21);
    CAN1->sTxMailBox[transmit_mailbox].TDTR = (msg->len & 0x0F);

    // Set up the data
    CAN1->sTxMailBox[transmit_mailbox].TDLR =
        (msg->data[3] << 24) | (msg->data[2] << 16) | (msg->data[1] << 8) | msg->data[0];
    CAN1->sTxMailBox[transmit_mailbox].TDHR =
        (msg->data[7] << 24) | (msg->data[6] << 16) | (msg->data[5] << 8) | msg->data[4];

    // Request transmission
    CAN1->sTxMailBox[transmit_mailbox].TIR |= CAN_TI0R_TXRQ;

    return 1;
}

size_t can_recv(struct can_msg *msgs, size_t n) {
    size_t count = 0;
    uint32_t current_tail = rx_tail;

    while (count < n && current_tail != rx_head) {
        memcpy(msgs, &rx_buffer[current_tail], sizeof(struct can_msg));
        current_tail = (current_tail + 1) % CAN_RX_BUFFER_SIZE;
        msgs++;
        count++;
    }

    rx_tail = current_tail;
    return count;
}

// CAN1 RX0 Interrupt Handler
void CAN1_RX0_IRQHandler(void) {
    // Check if the interrupt is for a message pending in FIFO0
    if ((CAN1->RF0R & CAN_RF0R_FMP0) != 0) {
        uint32_t next_head = (rx_head + 1) % CAN_RX_BUFFER_SIZE;
        if (next_head != rx_tail) {
            // Read ID, DLC
            rx_buffer[rx_head].id = (CAN1->sFIFOMailBox[0].RIR >> 21);
            rx_buffer[rx_head].len = (CAN1->sFIFOMailBox[0].RDTR & 0x0F);

            // Read data
            rx_buffer[rx_head].data[0] = (CAN1->sFIFOMailBox[0].RDLR >> 0) & 0xFF;
            rx_buffer[rx_head].data[1] = (CAN1->sFIFOMailBox[0].RDLR >> 8) & 0xFF;
            rx_buffer[rx_head].data[2] = (CAN1->sFIFOMailBox[0].RDLR >> 16) & 0xFF;
            rx_buffer[rx_head].data[3] = (CAN1->sFIFOMailBox[0].RDLR >> 24) & 0xFF;
            rx_buffer[rx_head].data[4] = (CAN1->sFIFOMailBox[0].RDHR >> 0) & 0xFF;
            rx_buffer[rx_head].data[5] = (CAN1->sFIFOMailBox[0].RDHR >> 8) & 0xFF;
            rx_buffer[rx_head].data[6] = (CAN1->sFIFOMailBox[0].RDHR >> 16) & 0xFF;
            rx_buffer[rx_head].data[7] = (CAN1->sFIFOMailBox[0].RDHR >> 24) & 0xFF;

            rx_buffer[rx_head].flags = 0;

            rx_head = next_head;
        }

        // Release the FIFO0 message
        CAN1->RF0R |= CAN_RF0R_RFOM0;
    }
}
