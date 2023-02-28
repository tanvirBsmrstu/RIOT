/**
 * @cond
 * @defgroup    sys_psa_crypto    PSA Cryptographic API
 * @ingroup     sys
 * @brief       Implements the PSA Crypto API specification.
 * @see         https://armmbed.github.io/mbed-crypto/html/
 *
 * @note    This implementation is not complete.
 *          See [Implementation Status](#implementation-status) to see which
 *          algorithms and backends are supported.
 *
 * About {#About}
 * =====
 * This module implements the PSA Cryptography API Version 1.1 as specified
 * [here](https://armmbed.github.io/mbed-crypto/html/).
 * It provides an OS level access to cryptographic operations and supports software and hardware
 * backends as well as the use of secure elements.
 * The API automatically builds a hardware backend for an operation, if there's one available,
 * otherwise it falls back to software. Specific backends can be configured, if needed.
 * For configuration options see [Configuration](#configuration).
 *
 * PSA has an integrated key management module, which stores keys internally without exposing them
 * to applications. To learn how to use keys with PSA, read [Key Management](#key-management).
 *
 * A basic usage and configuration example can be found in `examples/psa_crypto`.
 * For more usage instructions, please read the documentation.
 *
 * If you want to add operations and backends, see [Porting Guide](#porting-guide).
 *
 * Key Management {#key-management}
 * ===
 * PSA can only operate on keys, that are stored in the internal key storage module. This means you
 * need to either generate keys with PSA or import an existing one.
 * For this purpose, there are a number of
 * [key management functions](https://armmbed.github.io/mbed-crypto/html/api/keys/management.html).
 *
 * ## Key Attributes
 * To create a key for PSA, you need to set key attributes first.
 * Attributes describe the key type, size, lifetime and usage policies (see
 * [Key Attributes](https://armmbed.github.io/mbed-crypto/html/api/keys/attributes.html)).
 *
 * The example below defines attributes for an AES-128 key, which can be used for CBC encryption
 * and decryption and will be stored in local volatile memory.
 * @code
 * psa_key_attributes_t attributes = psa_key_attributes_init();
 * psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
 * psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
 * psa_set_key_bits(&attributes, 128);
 * psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
 * psa_set_key_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
 * @endcode
 *
 * After setting the attributes, an exiting key can be imported:
 * @code
 * uint8_t AES_KEY[] = { ... };
 * psa_key_id_t key_id = 0;
 * psa_status_t status = psa_import_key(&attributes, AES_KEY, sizeof(AES_KEY), &key_id)
 * @endcode
 * The identifier returned by the import operation can then be used for operations with this
 * specific key.
 * @code
 * status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING, PLAINTEXT, sizeof(PLAINTEXT),
 * output_buffer, sizeof(output_buffer, &output_length));
 * @endcode
 *
 * ### Lifetime
 * The lifetime value actually consists of two values: persistence and location. The location
 * defines the actual memory location of the key. Currently, only the persistence level
 * @ref PSA_KEY_PERSISTENCE_VOLATILE is supported.
 * Supported locations are @ref PSA_KEY_LOCATION_LOCAL_STORAGE, as well as secure element locations
 * (see [Secure Elements](#secure-elements).
 * The actual lifetime can be constructed with the macro
 * `PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location)`.
 *
 * Configuration {#configuration}
 * ===
 * To use PSA with a default backend, simply select the PSA modules and the operations needed. The
 * build system will then automatically build the available backends. It will default to hardware
 * accelerated crypto, if it is present and fall back to a software backend, if not.
 * @code
 * USEMODULE += psa_crypto
 * USEMODULE += psa_cipher
 * USEMODULE += psa_cipher_aes_128_cbc
 * @endcode
 *
 * If desired, you can choose a specific backend at compile time:
 * @code
 * USEMODULE += psa_cipher_aes_128_cbc_custom_backend
 * USEMODULE += psa_cipher_aes_128_cbc_backend_riot
 * @endcode
 *
 * Implementation Status and Available Backend Modules {#implementation-status}
 * =====================
 * ## Base Module
 * - `psa_crypto` (required)
 *
 * ## Hashes
 * | Algorithms | Software Backends | Hardware Backends | Note |
 * |------------|-------------------|-------------------|------|
 * | MD5        | RIOT Hashes       | -                 | Insecure, only for legacy usage |
 * | SHA-1      | RIOT Hashes       | nRF52 CryptoCell  | Insecure, only for legacy usage |
 * | SHA-224    | RIOT Hashes       | nRF52 CryptoCell  |-|
 * | SHA-256    | RIOT Hashes       | nRF52 CryptoCell  |-|
 * | SHA-512    | -                 | nRF52 CryptoCell  |-|
 *
 * ### Modules
 * - `psa_hash` (required)
 * - `psa_hash_md5`
 * - `psa_hash_sha_1`
 * - `psa_hash_sha_224`
 * - `psa_hash_sha_256`
 * - `psa_hash_sha_512`
 *
 * ### Backend Modules
 * - `psa_hash_sha_<size>_custom_backend`
 * - `psa_hash_sha_<size>_backend_periph`
 * - `psa_hash_sha_<size>_backend_riot`
 *
 * ## Ciphers
 * | Algorithms | Software Backends | Hardware Backends | Note |
 * |------------|-------------------|-------------------|------|
 * | AES-128 ECB| -       | ATECC608A | Insecure, only for legacy usage |
 * | AES-128 CBC| RIOT Cipher       | nRF52 CryptoCell, ATECC608A |-|
 * | AES-192 CBC| RIOT Cipher       | - |-|
 * | AES-256 CBC| RIOT Cipher       | - |-|
 *
 * ### Modules
 * - `psa_cipher` (required)
 * - `psa_cipher_aes_128_ecb`
 * - `psa_cipher_aes_128_cbc`
 * - `psa_cipher_aes_192_cbc`
 * - `psa_cipher_aes_256_cbc`
 *
 * ### Backend Modules
 * - `psa_cipher_aes_<size>_<mode>_custom_backend`
 * - `psa_cipher_aes_<size>_<mode>_backend_periph`
 * - `psa_cipher_aes_<size>_<mode>_backend_riot`
 *
 * ## MAC
 * | Algorithms | Software Backends | Hardware Backends | Note |
 * |------------|-------------------|-------------------|------|
 * | HMAC-SHA256| RIOT Hashes       | nRF52 CryptoCell, ATECC608A|-|
 *
 * ### Modules
 * - `psa_mac` (required)
 * - `psa_mac_hmac_sha_256`
 *
 * ### Backend Modules
 * - `psa_mac_hmac_sha_256_custom_backend`
 * - `psa_mac_hmac_sha_256_backend_periph`
 * - `psa_mac_hmac_sha_256_backend_riot`
 *
 * ## Elliptic Curves
 * ### Key Generation
 * | Curves | Software Backends | Hardware Backends | Note |
 * |--------|-------------------|-------------------|------|
 * | SECP-192| Micro-ECC        | nRF52 CryptoCell  |-|
 * | SECP-256| Micro-ECC        | nRF52 CryptoCell, ATECC608A |-|
 *
 * ### ECDSA
 * | Curves | Software Backends | Hardware Backends | Note |
 * |--------|-------------------|-------------------|------|
 * | SECP-192| Micro-ECC        | nRF52 CryptoCell  |-|
 * | SECP-256| Micro-ECC        | nRF52 CryptoCell, ATECC608A |-|
 *
 * ### Modules
 * - `psa_asymmetric` (required)
 * - `psa_asymmetric_ecc_p192r1`
 * - `psa_asymmetric_ecc_p256r1`
 *
 * ### Backend Modules
 * - `psa_asymmetric_ecc_<curve>_custom_backend`
 * - `psa_asymmetric_ecc_<curve>_backend_periph`
 * - `psa_asymmetric_ecc_<curve>_backend_microecc`
 *
 * ## RNG
 * Currently uses RIOT Random Module as backend. See the documentation for configuration options.
 *
 * Secure Elements {#secure-elements}
 * ===
 * This implementation supports the use of multiple secure elements (SE). For this, you need to
 * assign a static location value in the range of PSA_KEY_LOCATION_SE_MIN and
 * PSA_KEY_LOCATION_SE_MAX to each connected SE (e.g.
 * `pkg/cryptoauthlib/include/atca_params.h`). At startup, the SEs are registered with PSA with
 * their assigned location value and can be accessed by simply creating keys with this location.
 *
 * To use SEs, the appropriate modules must be chosen in the Makefile:
 * @code
 * USEMODULE += psa_secure_element
 * USEMODULE += psa_secure_element_ateccx08a
 * USEMODULE += psa_secure_element_ateccx08a_ecc
 * @endcode
 *
 * When using more than one SE, add `USEMODULE += psa_secure_element_multiple`.
 *
 * Porting Guide {#porting-guide}
 * ===
 * ## General Information
 * To add functionality, carefully read the
 * [API specification](https://armmbed.github.io/mbed-crypto/html/index.html).
 * Make sure, your operations return the correct values (including error values).
 * If your crypto driver or library specifies its own status values, please map them to the
 * corresponding PSA status as good as possible.
 *
 * For many operations, PSA specifies supporting macros to determine correct buffer sizes,
 * construct values from multiple inputs (e.g. key lifetime) or check the validity of input
 * parameters. Make sure, that the macros also support your added algorithms and operations and
 * return the correct values.
 *
 * Read the following description of the PSA layers and which functionality should be added to
 * which layer.
 *
 * ![PSA Structure](riot-psa-structure.svg)
 *
 * PSA is structured in layers. Operations are handed from the user facing API down to the
 * executing driver or library, while passing several dispatchers.
 * The first layer is implemented in `sys/psa_crypto/psa_crypto.c`. Here, input parameters
 * and output buffer sizes are validated. This layer also gets keys from storage and checks,
 * if the operation complies with key usage policies (see existing implementations).
 *
 * After validation, each call is passed to the location dispatcher, implemented in
 * `sys/psa_crypto/psa_crypto_location_dispatch.c`. Here, the call must be passed on to either the
 * secure element dispatcher (if a key is stored on an SE), or an algorithm dispatcher (for locally
 * stored keys).
 *
 * ### Secure Element Integration
 * We provide a generic SE interface (see [SE Driver Wrapper](#sys_psa_crypto_se_driver)).
 * Your SE driver must implement
 * this API and provide pointers to the available methods. If some functions are not supported by
 * your device, set the pointer to `NULL` (for an example, see
 * `pkg/cryptoauthlib/psa_se_driver/psa_atca_se_driver.c`).
 *
 * Each connected device must be registered with PSA and a device unique location value
 * (see `pkg/cryptoauthlib/include/atca_params.h`). This should happen automatically in
 * the [Auto Init](#sys_auto_init) module (see `sys/auto_init/security/auto_init_atca.c`).
 *
 * ### Other Hardware and Software Backend Integration
 * Operations that are not executed on SEs, are passed to an algorithm dispatcher. Here, the key
 * type, size and algorithm must be mapped to an algorithm specific API.
 * The naming should follow the schema `psa_<algorithm>_<keytype>_<keysize>_<operation>` (e.g.
 * `psa_cipher_cbc_aes_128_encrypt` or `psa_ecc_p192r1_verify_hash`).
 * This specific API must be implemented by the backend, that should perform the operation
 * (for an example see `sys/crypto/psa_riot_cipher/` or for third party packages
 * `pkg/driver_cryptocell_310`).
 * @endcond
 */
