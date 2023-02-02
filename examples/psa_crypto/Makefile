# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

APPLICATION = example_psa_crypto

BOARD ?= native

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto
USEMODULE += psa_hash
USEMODULE += psa_hash_sha_256

USEMODULE += psa_cipher
USEMODULE += psa_cipher_aes_128_cbc

USEMODULE += psa_mac
USEMODULE += psa_mac_hmac_sha_256

USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_p256r1

ifeq (1, $(SECURE_ELEMENT))
  CFLAGS += -DSECURE_ELEMENT
  USEMODULE += psa_secure_element
  USEMODULE += psa_secure_element_ateccx08a
  USEMODULE += psa_secure_element_ateccx08a_ecc
  ifeq (1, $(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.test.se
  else
    CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=4
    CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1
  endif
else ifeq (2, $(SECURE_ELEMENT))
  CFLAGS += -DMULTIPLE_SE
  CFLAGS += -DSECURE_ELEMENT
  CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=8
  CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=2
  USEMODULE += psa_secure_element
  USEMODULE += psa_secure_element_multiple
  USEMODULE += psa_secure_element_ateccx08a
  USEMODULE += psa_secure_element_ateccx08a_ecc
  ifeq (1, $(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.test.se.multi
  else
    CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=8
    CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=2
  endif
else ifdef SOFTWARE
  USEMODULE += psa_cipher_aes_128_cbc_custom_backend
  USEMODULE += psa_cipher_aes_128_cbc_backend_riot # force software backend

  USEMODULE += psa_mac_hmac_sha_256_custom_backend
  USEMODULE += psa_mac_hmac_sha_256_backend_riot # force software backend

  USEMODULE += psa_hash_sha_256_custom_backend
  USEMODULE += psa_hash_sha_256_backend_riot

  USEMODULE += psa_asymmetric_ecc_p256r1_custom_backend
  USEMODULE += psa_asymmetric_ecc_p256r1_backend_microecc # force software backend

  ifeq (1, $(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.test.sw
  else
    CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
    CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=3
  endif
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(12*THREAD_STACKSIZE_DEFAULT\)
endif

SHOULD_RUN_KCONFIG :=

include $(RIOTBASE)/Makefile.include
