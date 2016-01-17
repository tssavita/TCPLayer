# TCPLayer

This is a rudimentary implementation of the TCP Layer in an attempt to understand its working better. It was tested using a basic HTTP Server and client available at https://github.com/martijnvermaat/tcp/tree/master/http.

The TCP Library has the following functions :

* int tcp_socket ();
* int tcp_connect (ipaddr_t dst, int port);
* int tcp_listen (int port, ipaddr_t *src);
* int tcp_read (char *buf, int maxlen);
* int tcp_write (char *buf, int len);
* int tcp_close ();
