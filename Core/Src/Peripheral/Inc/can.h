#ifndef PERIPHERAL_INC_CAN_H_
#define PERIPHERAL_INC_CAN_H_

#include <stdint.h>
#include <stddef.h> // For size_t
#include <stdbool.h>

// A generic CAN message structure, independent of any stack.
// This mirrors the essential fields needed for CAN communication.
typedef struct {
    uint32_t id;     // Standard CAN ID (11-bit)
    uint8_t  len;    // Data Length Code (0-8)
    uint8_t  data[8];
} can_msg_t;

/**
 * @brief Initializes the CAN1 peripheral and its GPIOs (PB8, PB9).
 *
 * Configures CAN1 for 125 kbps, sets up a filter to accept all messages,
 * and enables the receive interrupt.
 */
void can_init(bool loopback_mode);

/**
 * @brief Attempts to receive up to 'n' CAN messages from the internal ring buffer.
 * @param msgs A pointer to an array of can_msg_t to be filled.
 * @param n    The maximum number of messages to retrieve.
 * @return The number of messages actually retrieved from the buffer.
 */
size_t can_recv(can_msg_t *msgs, size_t n);

/**
 * @brief Queues a single CAN message for transmission.
 * @param msg A pointer to the can_msg_t to be sent.
 * @return 1 if the message was successfully queued, 0 otherwise.
 */
size_t can_send(const can_msg_t *msg);

#endif /* PERIPHERAL_INC_CAN_H_ */
