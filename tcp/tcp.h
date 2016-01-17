#define _TCP_H_
#ifndef _TCP_H_

#include <sys/types.h>

#define MAX_TCP_SEG_SIZE 8000
#define CLIENT_PORT 8042
#define TCP_HEADER_SIZE 20
#define IP_HEADER_SIZE 20
#define MAX_TCP_DATA_SIZE 7960

/* Flags */
#define FIN_FLAG 0x01
#define SYN_FLAG 0x02
#define RST_FLAG 0x04
#define PSH_FLAG 0x08
#define ACK_FLAG 0x10
#define URG_FLAG 0x20

typedef uint32_t tcp_u32t;
typedef uint16_t tcp_u16t;
typedef uint8_t tcp_u8t;

typedef enum {
    S_CLOSED, S_LISTEN, S_SYN_SENT, S_SYN_RCVD, S_ESTABLISHED, S_FIN_WAIT_1, 
    S_CLOSE_WAIT, S_FIN_WAIT_2, S_LAST_ACK, S_TIME_WAIT 
} state_t;

/* TCP Control Block */
typedef struct tcblock {
    ipaddr_t tcb_our_ipaddr;
    ipaddr_t tcb_their_ipaddr;
    tcp_u16t tcb_our_port;
    tcp_u16t tcb_their_port;
    tcp_u32t tcb_our_sequence_num;
    tcp_u32t tcb_their_sequence_num;
    tcp_u32t tcb_our_expected_ack;
    char tcb_data[TCB_BUF_SIZE];
    char *tcb_p_data;
    int tcb_data_left;
    state_t tcb_state;
    tcp_u8t tcb_flags;
    tcp_u16t tcb_window_size;
} tcb_t;

/* Pseudo header */
typedef struct pseudoheader {
	ipaddr_t src_ipaddr;
	ipaddr_t dst_ipaddr;
	tcp_u8t zero;
	tcp_u8t protocol_id;
	tcp_u16t tcp_segment_length;
} pseudoheader_t;

/* TCP header */
typedef struct tcp_header {
    tcp_u16t src_port;
    tcp_u16t dst_port;
    tcp_u32t seq_num;
    tcp_u32t ack_num;
    tcp_u8t data_offset;
    tcp_u8t flags;    
    tcp_u16t win_size;
    tcp_u16t checksum;    
    tcp_u16t urgent_ptr;    
} tcp_header_t;

/* Total TCP Segment header */
typedef struct tcp_segment_header_t {
	pseudoheader_t pseudo_header;
	tcp_header_t tcp_header;
};

/* Initializing TCB */
static tcb_t tcb = {
    0,       /* our_ipaddr        		*/
    0,       /* their_ipaddr   	  		*/
    0,       /* our_port          		*/
    0,       /* their_port        		*/
    0,       /* our_sequence_num        */
    0,       /* their_sequence_num      */
    0,       /* expected_ack      		*/
    "",      /* data     	     		*/
    "",      /* unacked_data      		*/
    0,       /* unacked_data_len  		*/
    S_START, /* state             		*/
    0,       /* flags				 	*/
    0,       /* window size			 	*/
};

/* TCP Library functions*/

int tcp_socket ();

int tcp_connect (ipaddr_t dst, int port);

int tcp_listen (int port, ipaddr_t *src);

int tcp_read (char *buf, int maxlen);

int tcp_write (char *buf, int len);

int tcp_close ();

int send_tcp_packet (ipaddr_t dst, tcp_u16t src_port, tcp_u16t dst_port, tcp_u32t seq_nb, 
        tcp_u32t ack_nb, tcp_u8t flags, tcp_u16t win_sz, const char* data, int data_sz);

int recv_tcp_packet (ipaddr_t *src, tcp_u16t *src_port, tcp_u16t *dst_port, tcp_u32t *seq_nb
        tcp_u32t *ack_nb, tcp_u8t *flags, tcp_u16t *win_sz, char *data, int *data_sz); 

#endif
