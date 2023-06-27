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
 * @brief       Common hash functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 hash APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */
#ifndef PSA_CRYPTOCELL_310_HASHES_COMMON_H
#define PSA_CRYPTOCELL_310_HASHES_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "crys_hash.h"

/**
 * @brief   Common hash setup function
 *
 * @param   ctx
 * @param   mode
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_hash_setup(CRYS_HASHUserContext_t *ctx,
                               CRYS_HASH_OperationMode_t mode);

/**
 * @brief   Common hash update function
 *
 * @param   ctx
 * @param   input
 * @param   input_length
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_hash_update(CRYS_HASHUserContext_t *ctx,
                                const uint8_t *input,
                                size_t input_length);

/**
 * @brief   Common hash finish function
 *
 * @param   ctx
 * @param   hash
 * @param   hash_size
 * @param   hash_length
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_hash_finish(CRYS_HASHUserContext_t *ctx,
                                uint8_t *hash,
                                size_t hash_size,
                                size_t *hash_length);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTOCELL_310_HASHES_COMMON_H */
/** @} */
