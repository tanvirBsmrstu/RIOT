# Example Applications for PSA Crypto
This example application is supposed to show two things:
    1. How to use basic functions of the PSA Crypto API
    2. How to configure the implementation with Kconfig dependency resolution vs.
        Make dependency resolution

## Basic usage of PSA Crypto
There are three example operations:
- AES 128 CBC
- HMAC SHA256
- ECDSA with a P256 curve

Each comes in its own sourcefile called `example_<operation>.c`. To see which functions to call to perform each operations, please read the code.

## Configuration of the API
There are two ways to configure the API: module selection via Kconfig and module selection
via Makefiles.

To see which modules should be chosen for each configuration, please read the Makefile and
the `app.config.test` files.

### Kconfig
When building the application with the `TEST_KCONFIG=1` option, the first config file used is
`app.config.test`. This chooses the PSA Crypto module, the operations you want to use and other
modules our application needs (e.g. ztimer). If you need cryptographic keys, you can specify the number of key slots needed for key storage (the default is set to 5).

If you only build this without specifying anything else, PSA Crypto will automatically choose a default crypto backend depending on the platform you're building for.
For example when your platform is `native`, software implementations are built.
When you specify `BOARD=nrf52840dk`, the hardware accelerator of the board will be built.

If you want to force a custom backend, you can specify that in the Kconfig file. This application already contains the configuration for a custom backend (see `app.config.test.custom`), which can be added to the build by defining `CUSTOM_BACKEND=1`.

Instead of or in addition to the default and custom implementations you can use a secure element as a backend. The configuration can be found in `app.config.test.se` and the application can be built with `SECURE_ELEMENT=1`.

Note that in this application all the configurations are in separate app.config files for demonstration purposes. You can also write all configs into one file or choose them via `menuconfig`.
Also the build options `CUSTOM_BACKENDS` and `SECURE_ELEMENT` only apply to this specific application.

### Make
All the configurations in the Kconfig files can also be applied using Make dependency resolution. The Makefile contains all the modules that must be selected when building the different configurations.
They can all be built as described above, but *without* the `TEST_KCONFIG=1` setting.

When building this application multiple times with different backends, it is best to remove the `bin` directory in between builds.

## A thing about Secure Elements
Currently this implementation only supports Microchip ATECCX08A devices. Those devices need to be configured and locked to be able to correctly use them.
This implementation assumes that you have done that and that you know your device's configuration.

In `main.c` you can find a `slot_config` list. Here you need to specify for each connected secure element what kind of key can be stored in which slot, and whether it is already occupied. The list contains an example configuration, but the values can be changed.
This is required, so the implementation can allocate free key slots and can generate and import new keys.

Please make sure that your key slots are writable.

The current implementation does not support the use of keys that are already stored on the device.

## Note
If you need more information about the build options, please refer to the API documentation.
