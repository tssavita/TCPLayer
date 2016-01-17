// extra fittings 

void create_listening_socket ()
{
    struct sockaddr_in sin;

    int listening_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listening_socket < 0) {
        dprint ("Could not create TCP socket.\n");
        exit (EXIT_FAILURE);
    }

    int optval = 1;
    int options = setsockopt (listening_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    if (options != 0) {
        dprint (Could not set options on socket);
        exit (EXIT_FAILURE);
    }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = tcb.our_port;
    sin.sin_addr.s_addr = INADDR_ANY;

    int result = bind (listening_socket, (struct sockaddr *)&sin, sizeof(sin));
    if (result < 0) {
        dprint ("Could not bind to virtual IP and port", tcb.our_ipaddr, tcb.our_port);
        exit (EXIT_FAILURE);
    }
}

/* Creating a TCP socket to use it for sending data. */
void create_sending_socket () 
{
    int sending_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sending_socket < 0) {
        dprint ("Could not create TCP socket.\n");
        exit (EXIT_FAILURE);
    }
}

/* Connection less part of the TCP. */

/* Encoding the pseudo header of the TCP segment into a hex 
   string for checksum calculation. Could be improved. */
char *encode (ipaddr_t src_ip, ipaddr_t dst_ip, void *data) {
	
	char *encoded, *srcip, *dstip, tcp_len

    sprintf(&srcip, "%0x", tcb.our_ipaddr);
    sprintf(&dstip, "%0x", tcb.their_ipaddr);

    int segment_size = sizeof((char *) data) + sizeof(struct tcp_segment_t);
    sprintf(&tcp_len, "%0x",segment_size);

    strcat(encoded, srcip);
    strcat(encoded, dstip);
    strcat(encoded, "0");
    strcat(encoded, "6");
    strcat(encoded, tcp_len);    

    while ((sizeof(encoded) % 4) != 0)
        encoded = strcat("0", encoded);

	return encoded;
}

/* Calculating checksum. Could be improved. */
char *calculate_checksum(void *encoded) 
{
    char **half_words;
    int hw_num = (sizeof(encoded) / 4);
    half_words = malloc (sizeof(char) * hw_num);

    for (i = 0; i < hw_num; i++) {
        half_words[i] = malloc(sizeof(char) * 4);
        half_words[i] = "";
        strcat (strcat (strcat (strcat (half_words[i], encoded[4*i]), encoded[4*i+1]), encoded[4*i+2]), encoded[4*i+3]);
    }

    long int sum = 0;
    char *checksum = malloc (sizeof(char) * 4);
    int exceed = 0;

    for (i = 0; i < hw_num; i++) 
        sum += strtol(half_words[i], NULL, 16);
    
    if (sum > 0xFFFF) {
        exceed = sum - 0xFFFF;
        sum = 0xFFFF + exceed;
        sum = ~sum;
        sprintf();
    }
	
	return (u16_t)check_sum;
}

int tcp_connect (ipaddr_t dst, int port) 
{
    /* If this is the side initiating the connection, then : 
       The three way handshake is done here to set up the TCP connection. 
       1. Send a packet with just a SYN packet set. 
       2. Send a packet with ACK at the end, after having received a SYN + ACK packet from the other end. */
    tcb.their_ipaddr = dst;
    tcb.their_port = port;
    tcb.our_seqnum = 1000;
    tcb.our_expack = 0;
    tcb.their_seqnum = 0;
    tcb.win_sz = 0;
    tcb.data = NULL; /* The logic behind this is that there is no data transmitted in a SYN packet. */
    tcb.p_data = NULL;
    tcb.data_left = 0;

    u8_t flags = TCP_SYN & TCP_PSH;

    int send_syn = send_tcp_packet (tcb.their_ipaddr, tcb.our_port, tcb.their_port, tcb.our_seqnum, 
            tcb.our_expack, flags, 0, NULL, 0);

    if (send_syn < 0) {
        dprint ("Could not send packet. \n");
        exit (EXIT_FAILURE);
    }

    tcb.our_expack = tcb.our_seqnum;
    tcb.our_seqnum++;

    int recv_synack = recv_tcp_packet (tcb.their_ipaddr, tcb.our_port, tcb.their_port, tcb.our_seqnum,
            tcb.our_expack, flags, 0, NULL, 0);

    if (recv_synack < 0) {
        dprint ("Could not receive the packet. \n");
        exit (EXIT_FAILURE);
    }

    int send_ack = send_tcp_packet (tcb.their_ipaddr, tcb.our_port, tcb.their_port, tcb.our_seqnum, 
            tcb.our_expack, flags, 0, tcb.data, sizeof (tcb.data));

    int (send_ack < 0) {
        dprint ("Could not send the acknowledgement.\n");
        exit (EXIT_FAILURE);
    }

    tcb.state = S_ESTABLISHED;
        /* At the end of a successful connection, both the client and the server are in S_ESTABLISHED state. */
    return 0;
}

int tcp_listen (int port, ipaddr_t *src) 
{
    int their_fd;
    struct sockaddr_in their_addr;
    int their_addr_len;
    
    create_listening_socket ();
    listen (listening_socket, 1);
    
    tcb.our_port = port;
    tcb.our_ipaddr = src;
    tcb.data = "";
    tcb.p_data = NULL;
    tcb.data_left = 0;

    while (1) {
        their_addr_len = sizeof (their_addr);
        their_fd = accept (listening_socket, (struct sockaddr*)&their_addr, &their_addr_len);
        if (their_fd < 0)
            dprint ("Failed to accept connection.\n");
        dprint ("Accepting connection from %s:%d", inet_ntoa (their_addr.sin_addr), ntohs (their_addr.sin_port));
    }
    tcb.state = S_LISTEN;
}

int recv_tcp_packet (ipaddr_t *src, u16_t *src_port, u16_t *dst_port, u32_t *seq_nb, 
        u32_t *ack_nb, u8_t *flags, u16_t *win_sz, char *data, int *data_sz) {
    tcp_header_t header;
    int header_sz = sizeof (tcp_header_t);
    int result;
    char *buf = malloc ();

    if (!tcb.our_ipaddr)
        tcp_init ();

    do {
        header.dst_port = 0;

        /* Receive the packet using ip_receive () */
        result = ip_receive (tcb.their_ipaddr, tcb.our_ipaddr, IP_PROTO_TCP, 0, data);
        if (result < 0)
            return result;
        else 
            continue;
        memcpy (header, buf, header_sz);

    } while (header.dst_port != tcb.our_port);

    /* Decode headers. */
    *src_port = header.src_port;
    *dst_port = header.dst_port;
    *seq_nb = header.seq_nb;
    *ack_nb = header.ack_nb;
    *flags = header.flags;
    *win_sz = header.window;
    memcpy (data, buf + header_sz, sizeof(buf) - header_sz);

    /* Verify the checksum */
    char *encoded = encode (header.src_port, header.dst_port, sizeof(buf) - header_sz);
    u16_t *checksum = (u16_t *)checksum_calculation (encoded);

    if (checksum == header.checksum) {
        dprint ("Packet has been verified.\n");
        return -1;
    }
    else 
        dprint ("Packet has been tampered with.\n");

    return result - header_sz;
}
