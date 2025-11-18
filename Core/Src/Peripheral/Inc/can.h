#ifndef PERIPHERAL_INC_CAN_H_
#define PERIPHERAL_INC_CAN_H_

#include <lely/can/msg.h>
#include <stddef.h> // For size_t
#include <stdbool.h>

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
size_t can_recv(struct can_msg *msgs, size_t n);

/**
 * @brief Queues a single CAN message for transmission.
 * @param msg A pointer to the can_msg_t to be sent.
 * @return 1 if the message was successfully queued, 0 otherwise.
 */
size_t can_send(const struct can_msg *msg);

#endif /* PERIPHERAL_INC_CAN_H_ */
