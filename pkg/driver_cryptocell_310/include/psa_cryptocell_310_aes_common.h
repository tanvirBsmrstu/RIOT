/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       Common AES functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 AES APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTOCELL_310_AES_COMMON_H
#define PSA_CRYPTOCELL_310_AES_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

/**
 * @brief   Common setup function for AES operations
 *
 * @param   ctx
 * @param   direction
 * @param   mode
 * @param   padding
 * @param   iv
 * @param   key_buffer
 * @param   key_buffer_size
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_aes_setup(SaSiAesUserContext_t *ctx,
                              SaSiAesEncryptMode_t direction,
                              SaSiAesOperationMode_t mode,
                              SaSiAesPaddingType_t padding,
                              const uint8_t *iv,
                              const uint8_t *key_buffer,
                              size_t key_buffer_size);

/**
 * @brief   Common function for an AES encryption
 *
 * @param   ctx
 * @param   input
 * @param   input_length
 * @param   output
 * @param   output_buffer_size
 * @param   output_length
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_aes_encrypt_decrypt(SaSiAesUserContext_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *output,
                                        size_t output_buffer_size,
                                        size_t *output_length);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTOCELL_310_AES_COMMON_H */
/** @} */
