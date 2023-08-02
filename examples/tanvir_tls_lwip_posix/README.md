This example is about to tcp server and client, it uses posix socket and lwip stack -> please check Makefile
Please see tcp.c file, it contains server and client

### instruction TCP
1. first run ./network_config.sh, it will create two tap interface and set an ip``fec0:affe::1`` to tapbr0 [for faster debugging the ips are hardcoded]
2. run 2 instances of this example

    [*] first with ``sudo PORT=tap0 make clean all term``
                then    ``ip server`` it will set server ip address to ``fec0:affe::100``
                then    ``tcps`` it will run the server

    [*] second instance with ``sudo PORT=tap1 make all term``
                then ``ip client`` it will set client ip address to ``fec0:affe::99``
                then ``tcpc`` it will run client and try to connect to the server



### state
The current state is that the server and client can connect but when the ``send`` function is called, the instance is crushed and core dumped.


### Note
I don't know where the dumped file is located

### instruction TLS
it is the same as TCP, use ``tlss`` for server and ``tlsc`` for client


### state
The current state is that the server and client can connect but when TLS handshake is called using ``wolfssl_connect`` the programme gets a segmentation fault


### Note
I don't know where the dumped file is located
