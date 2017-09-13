ELF Injector
============

Based on PicoFlamingo's [ELFun File Injector](https://0x00sec.org/t/elfun-file-injector/410).

## Usage

1. Build everything, including dummy target binary and payload.

        make

2. Try out the dummy target

        build/dummy

3. Inject payload

        build/elfinjector build/dummy build/payload

4. Try out the infected target again

        build/dummy

5. Profit :squirrel:


## Disclaimer

For educational purposes.
