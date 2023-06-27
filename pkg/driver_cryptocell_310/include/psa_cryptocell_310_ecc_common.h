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
 * @brief       Common ECC functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 ECC APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTOCELL_310_ECC_COMMON_H
#define PSA_CRYPTOCELL_310_ECC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

#include "sns_silib.h"
#include "crys_ecpki_build.h"
#include "crys_ecpki_ecdsa.h"
#include "crys_ecpki_kg.h"
#include "crys_ecpki_domain.h"

/**
 * @brief   Map PSA hash encodings to CryptoCell specific hash encodings
 *
 * @param   hash psa_algorithm_t
 */
#define MAP_PSA_HASH_TO_CRYS_HASH(hash) \
    ((hash == PSA_ALG_SHA_1) ? CRYS_ECPKI_AFTER_HASH_SHA1_mode : \
     (hash == PSA_ALG_SHA_224) ? CRYS_ECPKI_AFTER_HASH_SHA224_mode : \
     (hash == PSA_ALG_SHA_256) ? CRYS_ECPKI_AFTER_HASH_SHA256_mode : \
     (hash == PSA_ALG_SHA_384) ? CRYS_ECPKI_AFTER_HASH_SHA384_mode : \
     (hash == PSA_ALG_SHA_512) ? CRYS_ECPKI_AFTER_HASH_SHA512_mode : \
     0)

/**
 * @brief   Common ECC key generation function
 *
 * @param   priv_key_buffer
 * @param   pub_key_buffer
 * @param   priv_key_buffer_length
 * @param   pub_key_buffer_length
 * @param   domain
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_ecc_generate_key_pair(uint8_t *priv_key_buffer,
                                                 uint8_t *pub_key_buffer,
                                                 uint32_t *priv_key_buffer_length,
                                                 uint32_t *pub_key_buffer_length,
                                                 CRYS_ECPKI_DomainID_t domain);

/**
 * @brief   Common ECC hash signature function
 *
 * @param   priv_key
 * @param   priv_key_size
 * @param   hash
 * @param   hash_length
 * @param   signature
 * @param   signature_length
 * @param   hash_mode
 * @param   domain
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_ecc_sign_hash(const uint8_t *priv_key,
                                         uint32_t priv_key_size,
                                         const uint8_t *hash,
                                         size_t hash_length,
                                         uint8_t *signature,
                                         size_t *signature_length,
                                         CRYS_ECPKI_HASH_OpMode_t hash_mode,
                                         CRYS_ECPKI_DomainID_t domain);

/**
 * @brief   Common ECC hash verification function
 *
 * @param   pub_key
 * @param   pub_key_size
 * @param   hash
 * @param   hash_length
 * @param   signature
 * @param   signature_length
 * @param   hash_mode
 * @param   domain
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_ecc_verify_hash(const uint8_t *pub_key,
                                           size_t pub_key_size,
                                           const uint8_t *hash,
                                           size_t hash_length,
                                           const uint8_t *signature,
                                           size_t signature_length,
                                           CRYS_ECPKI_HASH_OpMode_t hash_mode,
                                           CRYS_ECPKI_DomainID_t domain);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTOCELL_310_ECC_COMMON_H */
/** @} */
