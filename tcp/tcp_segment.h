#define _TCP_SEGMENT_H_
#ifndef _TCP_SEGMENT_H_

#include "utils.h"

/* Defining flags. */
#define TCP_ACK 1
#define TCP_PSH 2
#define TCP_RST 3
#define TCP_SYN 4
#define TCP_FIN 5

/* Limits on the */
#define MSG_MAX_SIZE        1024
#define MSG_ID_OFFSET       0
#define MSG_SIZE_OFFSET     6
#define MSG_COUNT_OFFSET    8
#define MSG_FLAGS_OFFSET    14
#define MSG_OK_FLAG         0x01
#define MSG_FRAGMENT_FLAG   0x02
#define MSG_LAST_FLAG       0x04
#define MSG_RESEND_FLAG     0x08

/* Structure of a TCP header. */
typedef struct {
    u16_t src_port;
    u16_t dst_port;
    u32_t seq_nb;
    u32_t ack_nb;
    u8_t data_offset;
    u8_t reserved;
    u8_t flags;
    u16_t window;
    u32_t checksum;
} tcp_header_t;

#endif
