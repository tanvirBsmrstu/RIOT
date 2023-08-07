MODULE := tanvir_tls_gnrc

RIOTBASE=$(CURDIR)/../../..

GNRC_IPV6=1

TLS_PORT=1234


CFLAGS +=  -DTLS_PORT='$(TLS_PORT)'

#SRCFILES += tls_api.c
SRC := tls_api.c 

include $(RIOTBASE)/Makefile.base
