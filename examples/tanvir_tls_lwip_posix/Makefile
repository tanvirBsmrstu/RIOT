APPLICATION = tanvir-example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1


# Optionally include remoteDNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
# USEMODULE += sock_dns              # include DNS client
# USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

ifneq (,$(DEFAULT_MQTT_CLIENT_ID))
  CFLAGS += -DDEFAULT_MQTT_CLIENT_ID=\"$(DEFAULT_MQTT_CLIENT_ID)\"
endif
ifneq (,$(DEFAULT_MQTT_USER))
  CFLAGS += -DDEFAULT_MQTT_USER=\"$(DEFAULT_MQTT_USER)\"
endif
ifneq (,$(DEFAULT_MQTT_PWD))
  CFLAGS += -DDEFAULT_MQTT_PWD=\"$(DEFAULT_MQTT_PWD)\"
endif

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEPKG += paho-mqtt
# The names of libraries to be built
CFLAGS += -DMQTT_EMBED_LIB_C=paho-embed-mqtt3cs

CFLAGS += -Wno-unused-but-set-variable
# CFLAGS += -Wno-error=unused-but-set-variable


LWIP_IPV6 ?= 1



###### IP Stack ####
USEMODULE += netdev_default
#CFLAGS += -DTHREAD_STACKSIZE_MAIN=20000

### LWIP ##
ifneq (0, $(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6_autoconfig
endif
USEMODULE += lwip_ipv6
USEMODULE += lwip_netdev
USEMODULE += netdev_eth
USEMODULE += auto_init_lwip


USEMODULE += lwip_tcp 
USEMODULE += netdev_default
USEMODULE += inet_csum
USEMODULE += lwip_sock_tcp
ifeq ($(BOARD),native)
  USEMODULE += lwip_ethernet
endif

USEMODULE += sock_tcp
USEMODULE += sock_ip

# Add POSIX modules
USEMODULE += posix_select
USEMODULE += posix_sockets
USEMODULE += posix_inet

CFLAGS += -DSO_REUSE
CFLAGS += -DLWIP_SO_RCVTIMEO
CFLAGS += -DLWIP_SOCK_TCP_ACCEPT_TIMEOUT=500
CFLAGS += -DLWIP_ICMP6=1
# CFLAGS += -DLWIP_NETIF_LOOPBACK=1
# CFLAGS += -DLWIP_HAVE_LOOPIF=1

USEMODULE += posix_sockets


#########wolf ssl
USEPKG += wolfssl
USEMODULE+=wolfssl_socket
USEMODULE+=wolfcrypt_rsa
#USEMODULE+=wolfcrypt_ecc
USEMODULE+=wolfcrypt_aes
USEMODULE+=wolfcrypt_asn
USEMODULE+=wolfcrypt_hmac
USEMODULE+=wolfcrypt_md5
USEMODULE+=wolfcrypt_sha
USEMODULE+=wolfcrypt_random
USEMODULE+=wolfssl_internal
USEMODULE+=wolfssl_wolfio
USEMODULE+=wolfssl_keys
USEMODULE+=wolfssl_ssl
USEMODULE+=wolfssl_tls
USEMODULE+=wolfcrypt_aes_gcm

CFLAGS+=-DHAVE_AESGCM -DWC_RSA_PSS -DHAVE_HKDF

CFLAGS += -DWOLFSSL_SP_MATH_ALL

#CFLAGS += -DWOLFSSL_LWIP
#CFLAGS += -DOPENSSL_EXTRA

#################
#CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT) -DDTLS_WOLFSSL -Wno-unused-parameter -Wno-unused-variable
CFLAGS += -Wno-unused-parameter -Wno-unused-variable


include Makefile.lwip
#include Makefile.gnrc

#USEMODULE += sock_async_event
#USEMODULE += sock_ip
USEMODULE += sock_udp
#USEMODULE += sock_tcp

USEMODULE += ztimer
USEMODULE += ztimer_msec

####

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DWIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DWIFI_PASS=\"$(WIFI_PASS)\"
endif
CFLAGS +=  -Wno-unused-parameter -Wno-unused-variable
-Werror=maybe-uninitialized
