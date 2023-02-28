/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @brief       Example application for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"
#include "ztimer.h"

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
psa_atca_slot_config_t slot_config[] = {
    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_AES, 0, 0 },
    { PSA_KEY_TYPE_HMAC, 0, 0 },
    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { 0, 1, 1 },
    { 0, 0, 0 },
    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 }
};

psa_se_config_t atca_config_list[] = {
    {
        .slots = slot_config
    }
};
#endif

extern psa_status_t example_cipher_aes_128(void);
extern psa_status_t example_hmac_sha256(void);
extern psa_status_t example_ecdsa_p256(void);

int main(void)
{
    psa_status_t status;

    psa_crypto_init();

    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    status = example_hmac_sha256();
    printf("HMAC SHA256 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        printf("HMAC SHA256 failed: %s\n", psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_cipher_aes_128();
    printf("Cipher AES 128 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        printf("Cipher AES 128 failed: %s\n", psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_ecdsa_p256();
    printf("ECDSA took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        printf("ECDSA failed: %s\n", psa_status_to_humanly_readable(status));
    }

    ztimer_release(ZTIMER_USEC);

    puts("All Done");
    return 0;
}
