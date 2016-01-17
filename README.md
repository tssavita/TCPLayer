# TCPLayer

This is a rudimentary implementation of the TCP Layer in an attempt to understand its working better. It was tested using a basic HTTP Server and client available at https://github.com/martijnvermaat/tcp/tree/master/http.

The TCP Library has the following functions :

1.) int tcp_socket ();
2.) int tcp_connect (ipaddr_t dst, int port);
3.) int tcp_listen (int port, ipaddr_t *src);
4.) int tcp_read (char *buf, int maxlen);
5.) int tcp_write (char *buf, int len);
6.) int tcp_close ();
