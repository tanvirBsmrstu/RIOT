This example is about to tcp server and client, it uses posix socket and gnrc stack -> please check Makefile
Please see tcp.c file, it contains server and client

### instruction TCP
1. first run ./network_config.sh, it will create two tap interface and set an ip``fec0:affe::1`` to tapbr0 [for faster debugging the ips are hardcoded]
2. run 2 instances of this example

    [*] first with ``sudo PORT=tap0 make clean all term``
                then check the interface number with ``ifconfig``
                then set ip with ``ifconfig 6 add fec0:affe::100`` please change the interface 6 to the one got from 
                now ``tlss`` this will start the tls server using the x509 certificate at ``./certs/certificates``

    [*] second instance with ``sudo PORT=tap1 make all term``
                then check the interface number with ``ifconfig``
                then set ip with ``ifconfig 6 add fec0:affe::99`` please change the interface 6 to the one got from previous step
                now ``tlsc`` this will start the tls server using the x509 certificate at ``./certs/certificates``

    ### NOTE
    for faster development client will always request to ``fec0:affe::100`` and this is hardcoded for now.


### state
    ## server and client can successfully communicate using x509 certificate


### instruction TLS
it is the same as TCP, use ``tlss`` for server and ``tlsc`` for client

ifconfig 6 add fec0:affe::100
ifconfig 6 add 192.168.1.100
