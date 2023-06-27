#ifndef PSA_CRYPTO_SE_CONFIG_H
#define PSA_CRYPTO_SE_CONFIG_H

#include "psa/crypto_types.h"

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
/**
 * @brief   Structure containing information about which type of key
 *          can be held by a specific key slot on an ATECCX08 device.
 *
 *          If a slot should be ignored by the implementation,
 *          just mark it as persistent and occupied.
 */
typedef struct {
    /* Type of key permitted for slot */
    psa_key_type_t key_type_allowed;
    /* Specify if key should be stored persistently or can be overwritten */
    uint8_t key_persistent;
    /* Specify whether slot is already occupied */
    uint8_t slot_occupied;
} psa_atca_slot_config_t;
#endif

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
/**
 * @brief   Structure containing device specific configuration data.
 *
 *          This will be stored in the driver's persistent data to
 *          manage the device.
 */
typedef union {
    #if (IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A))
        psa_atca_slot_config_t * slots;
    #endif
} psa_se_config_t;
#endif

#endif /* PSA_CRYPTO_SE_CONFIG_H */
