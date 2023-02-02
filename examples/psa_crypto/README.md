# Example Applications for PSA Crypto
This is a showcase for the use of the PSA Crypto for cryptographic operations.
It shows the following operations:
- AES 128 CBC
- HMAC SHA256
- ECDSA with a P256 curve

When building this application multiple times with different backends, in is best to remove the `bin` directory in between builds.

## Available backends
Several backends can be configured using build time options, depending on the target platform.

### Hardware
If a target platform has a cryptographic hardware accelerator, this application will automatically be built with the hardware backend.
Currently only the CryptoCell accelerator on the NRF52840dk is supported by PSA Crypto.

### Software
When building for `native` and boards without hardware accelerators, this application builds the following software backends:
- AES 128 CBC: RIOT Cipher Module
- HMAC SHA256: RIOT Hash Module
- ECDSA: Micro-ECC Package

On platforms with an accelerator, the use of software backends can be enforced by building with `SOFTWARE=1`.

### Secure Elements
When a secure element is available and correctly configured, this application can be built with the `SECURE_ELEMENT=1` option to perform all operations on an SE.
Up to two SEs are supported. Two can be chosen by passing `SECURE_ELEMENT=2`

## Constraints
Boards with very small memory may be able to run the application when using a secure element.
