#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tcp.h"

/* <------------------------CONNECTIONLESS PART----------------------> */

/* Refered to <>'s github code for checksum calculation. */
tcp_u16t tcp_checksum(ipaddr_t src, ipaddr_t dst, void *tcp_seg, tcp_u32t tcp_seg_len) {

	unsigned short *mov_ptr;
	unsigned long checksum, word = 0x00010000;
	int ct;
	
	/* Initializing pseudo header fields. */
    pseudoheader_t pheader;
    pheader.dst_ipaddr = dst;
    pheader.src_ipaddr = src;
    pheader.zero = 0;
    pheader.protocol_id = IP_PROTO_TCP;
    pheader.tcp_segment_length = htons(tcp_len);
    
    //TODO : CHECKSUM CALCULATION.
    ct = (pseudoheader_t) >> 1;
    
    for (checksum = 0; mov_ptr = (unsigned short *)&pheader; ct--) {
    	checkum += *mov_ptr++;
    	if (checksum >= word) {
    		checksum -= word;
    		checksum++;
    	}
    }
    
    ct = len >> 1;
    for (; mov_ptr = (unsigned short *)tcp_seg; ct--) {
    	checksum += *mov_ptr++;
    	if (checksum >= word) {
    		checksum -= word;
    		checksum++;
    	}
    }
    
    if (len & 1) {
	    checksum += ((unsigned short) * ((char *) mov_ptr)) & 0x00ff;
	    if (checksum >= word) {
	    	checksum -=word;
	    	checksum++;
	    }
	}
	
	return ~checksum;
}

int send_tcp_packet(ipaddr_t dst, tcp_u16t src_port, tcp_u16t dst_port, tcp_u32t seq_nb, 
        tcp_u32t ack_nb, tcp_u8t flags, tcp_u16t win_sz, const char* data, int data_sz)  {

    tcp_u32t header_sz = sizeof(tcp_header_t);
    tcp_u32t tcp_sz = header_sz + data_sz;
    char buf[MAX_TCP_SEG_SIZE - IP_HEADER_SIZE];
    tcp_header_t *theader;
    theader = (tcp_header_t *) buf;

    /* Initializing the tcp segment header fields. */
    theader->src_port = htons(src_port);
    theader->dst_port = htons(dst_port);
    theader->seq_num = htonl(seq_nb);
    theader->ack_num = htonl(ack_nb);
    theader->data_offset = (header_sz >> 2) << 4;
    theader->flags = flags;
    theader->win_size = htons(win_sz);
    theader->checksum = 0x00;
    theader->urgent_ptr = 0;
    
    /* Encoding headers to hex. */
	memcpy(&buf[header_sz], buf, data_sz);
	theader->checksum = tcp_checksum(my_ipaddr, dst, theader, tcp_sz);
	
    /* Transmit packet using ip_send () */
    int sent_bytes = ip_send (dst, IP_PROTO_TCP, 2, theader, tcp_sz);

    free (buf);

    if (sent_bytes < 0)
        return sent_bytes;
    else {
        if (header_sz > result)
            return -1;
        else 
            return sent_bytes - header_sz;
    }
}

int recv_tcp_packet(ipaddr_t *src, u16_t *src_port, u16_t *dst_port,
        u32_t *seq_nb, u32_t *ack_nb, u8_t *flags, u16_t *win_sz, char *data,
        int *data_sz)
{
    char *buf;
    buf = malloc(MAX_PACKET_SIZE - IP_HEADER_SIZE);
    int recv_bytes;

    tcp_header_t *theader;
    theader = (tcp_headrr_t *) segment;

    tcp_u16t protocol_num, id;
    tcp_u16t pkt_checksum, calc_checksum;
    
    while(1)
    {
        memset(buf, '\0', sizeof(buf));
        recv_bytes = ip_receive(src, &dst, &protocol_num, &id, &buf);

        if((proto != IP_PROTO_TCP) || (recv_bytes <= TCP_HEADER_SIZE))
            continue;
        else
            break;
    }

    memcpy(theader, buf, TCP_HEADER_SIZE);
    memcpy(data, (&buf + TCP_HEADER_SIZE), (ret - TCP_HEADER_SIZE));
    free(buf);
    pkt_checksum = theader.checksum;
    
    theader.source_port = ntohs(theader.source_port);
    theader.dest_port = ntohs(theader.dest_port);
    theader.sequence_num = ntohl(theader.sequence_num);
    theader.ack_num = ntohl(theader.ack_num);
    theader.window_size = ntohs(theader.window_size);  
	theader.checksum = 0;
	
    calc_checksum = tcp_checksum(*src, &dst, theader, tcp_sz);
    
    if (! (calc_checksum & pkt_checksum) ) {
    	*data_sz = recv_bytes;
    	flags = theader.flags;
    	return recv_bytes;
    }

}

/* <---------------CONNECTION-ORIENTED PART-------------------> */

/* The sending and receiving hosts attached themselves to a port 
   more generically known as a Transport Service Access Point. */
   
void send_syn() {
	char flags = PSH_FLAG | SYN_FLAG;

	int sent_syn_len = send_tcp_packet (tcb.tcb_their_ipaddr, tcb.tcb_our_port, tcb.tcb_their_port, 
			tcb.tcb_our_sequence_num, tcb.tcb_our_expected_ack, flags, 
			tcb.tcb_window_size, NULL, 0);
			
	tcb.tcb_their_sequence_num = tcb.tcb_our_expected_ack;
	tcb.tcb_our_expected_ack++;
	return sent_syn_len;
			
}

int send_ack () {
	char flags = PSH_FLAG | ACK_FLAG;
	
	flags |= PSH_FLAG;
    flags |= ACK_FLAG;

	return send_tcp_packet (tcb.tcb_their_ipaddr, tcb.tcb_our_port, tcb.tcb_their_port, 
			tcb.tcb_our_sequence_num, tcb.tcb_our_expected_ack, flags, 
			tcb.tcb_window_size, NULL, 0);
			
}

int handle_syn_ack () {

	char flags = PSH_FLAG | ACK_FLAG;

	return send_tcp_packet (tcb.tcb_their_ipaddr, tcb.tcb_our_port, tcb.tcb_their_port, 
			tcb.tcb_our_sequence_num, tcb.tcb_our_expected_ack, flags, 
			tcb.tcb_window_size, NULL, 0);
			
}

int handle_data(tcb_t temp_tcb)
{
    memcpy(&tcb, &temp_tcb, sizeof(temp_tcb));
    return send_ack();
}

void handle_syn () {
	send_syn();
}

void handle_ack (tcp_u8t flags, tcp_u32t ack_num) {
    if (ack_num == tcb.expected_ack) {
        tcb.tcb_our_sequence_num = ack_num;
        tcb.tcb_data_left = 0;
	}
}

void handle_fin (tcp_u8t flags, tcp_u32t seq_num) {
	int state = tcb.tcb_state;
	if (state == S_ESTABLISHED || state == S_FIN_WAIT_1 || state == S_FIN_WAIT_2) {
     	tcb.their_sequence_num = seq_num + 1;
        tcb.expected_ack = seq_num + 1;
        send_ack();
    }
}

static int handle_packet()
{
	tcb_t temp_tcb;
	ipaddr_t their_ip;
    tcp_u16t src_port, dst_port, win_sz;
    tcp_u32t seq_num, ack_num;
    tcp_u8t flags;
    char data[MAX_TCP_DATA];
    int data_sz = 0, rcvd;
	
    rcvd = recv_tcp_packet(&their_ip, &src_port, &dst_port, 
                        &seq_num, &ack_num, &flags, &win_sz, data, &data_sz);

    if(len > TCP_HEADER_SIZE) {
        /* This is a data packet */
        handle_data(temp_tcb);
        return len;
    }
    else {
        /* This is a control packet */
        if((temp_tcb.tcb_flags & SET_SYN) && (temp_tcb.tcb_flags & SET_ACK)) {
            return handle_syn_ack();
        }
        else if(temp_tcb.tcb_flags & SET_ACK) {
            return handle_ack(flags, ack_num);
        }
        else if(temp_tcb.tcb_flags & SET_SYN) {
            return handle_syn();
        }
        else if(g_tcb.tcb_flags & SET_FIN) {
            return handle_ack(flags, seq_num);
        }
        else {
            /* Unknown packet */
            return -1;
        }
    }
}

int min (int a, int b) {
    return (a < b) ? a : b;
}

int tcp_socket() 
{
	if (!my_ipaddr)
		ip_init();
		
	if (!my_ipaddr)
		return -1;
		
    tcb.tcb_our_ipaddr = my_ipaddr;
    tcb.tcb_state = S_CLOSED;
 
    return 0;
}

int tcp_listen(int port, ipaddr_t *src) {
    tcb.tcb_our_port = port;
    tcb.tcb_our_ipaddr = src;
    tcb.tcb_state = S_LISTEN;
    return handle_packet();
}

int tcp_connect(ipaddr_t dst, int port) {
	tcb.tcb_their_port = port;
	tcb.tcb_their_ipaddr = dst;
	tcb.tcb_our_port = CLIENT_PORT;
	send_syn();
	handle_packet();
	tcb.tcb_state = S_ESTABLISHED;
	return 0;
}

int tcp_read(char *buf, int maxlen) 
{
    int received_len = 0, add_len = 0;
    
    while (received_len < maxlen) {
    	add_len = handle_packet();
    	if (add_len < 0) 
	    	break;
	    	
	    memcpy((data + received_len), tcb.tcb_data, add_len);
	    received_len += add_len;
    }

	return received_len;
}

int tcp_write (char *buf, int len) {

	if (tcb.state != S_ESTABLISHED) 
        return -1;
        
    char add_data[MAX_TCP_DATA_SIZE], acked_data[MAX_TCP_DATA_SIZE];
    
    tcb.tcb_data_left = len;
    int data_left = 0;
    int retransmission, pkt_size, sent;
    while (tcb.tcb_data_left > 0) {
    	retransmission = 0;
    	pkt_size = min(tcb.tcb_data_left, MAX_TCP_DATA_SIZE);
    	memcpy(add_data, (data + len), pkt_size);
    	do {
    		sent = send_tcp_packet(dst, src_port, dst_port, seq_nb, 
	        ack_nb, flags, win_sz, data, data_sz);
    		retransmission++;
    	} while (retransmission < 11 && (! handle_packet()) );
    	if (retransmission < 11) {
    		len +=sent;
    		tcb.tcb_data_left -= sent;		
    	}
    	else 
    		break;
    }
    
    return len;
}

int tcp_close () 
{
    tcb.tcb_their_ipaddr = 0;
    tcb.tcb_their_port = 0;
    tcb.tcb_state = S_CLOSED;
    
	return 0;
}
