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

WIFI_SSID ?= "A book of ice and fire"
WIFI_PASS ?= "sorry@214"

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
###################  IP STACK ###############
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo    # For testing we also include the ping6 command and some stats
# Activate ICMPv6 error messages
#USEMODULE += gnrc_icmpv6_error
#USEMODULE += gnrc_tcp

USEPKG += paho-mqtt
#CFLAGS += -Dpaho-mqtt3cs
# paho-mqtt depends on TCP support, choose which stacks you want
GNRC_IPV6 ?= 1
LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 0
######
TLS_PORT ?= 1234 


###### IP Stack ####


### LWIP ##
# ifneq (0, $(LWIP_IPV6))
#   USEMODULE += ipv6_addr
#   USEMODULE += lwip_ipv6_autoconfig
# endif
# USEMODULE += lwip_ipv6
# USEMODULE += lwip_netdev
# USEMODULE += auto_init_lwip
# USEMODULE += sock_tcp
# USEMODULE += sock_ip

# CFLAGS += -DSO_REUSE
# CFLAGS += -DLWIP_SO_RCVTIMEO
# CFLAGS += -DLWIP_SOCK_TCP_ACCEPT_TIMEOUT=500
# CFLAGS += -DLWIP_NETIF_LOOPBACK=1
# CFLAGS += -DLWIP_HAVE_LOOPIF=1


###### IP Stack #####

#########wolf ssl
USEPKG += wolfssl
#USEMODULE+=wolfssl_socket
USEMODULE+=wolfcrypt
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
USEMODULE+=wolfssl_gnrc
USEMODULE+=wolfssl_dtls


CFLAGS+=-DHAVE_AESGCM -DWC_RSA_PSS -DHAVE_HKDF -DWOLFSSL_SP_MATH_ALL

# CFLAGS += -DOPENSSL_EXTRA
#  CFLAGS += -DOPENSSL_ALL
#CFLAGS += -DWOLFSSL_GNRC

#################
#CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT) -DDTLS_WOLFSSL -Wno-unused-parameter -Wno-unused-variable
CFLAGS += -Wno-unused-parameter -Wno-unused-variable


include Makefile.lwip
include Makefile.gnrc

USEMODULE += sock_async_event
#USEMODULE += sock_ip
USEMODULE += sock_udp
USEMODULE += sock_tcp


USEMODULE += ztimer
USEMODULE += ztimer_msec

####

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DWIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DWIFI_PASS=\"$(WIFI_PASS)\"
endif
CFLAGS +=  -Wno-unused-parameter -Wno-unused-variable
