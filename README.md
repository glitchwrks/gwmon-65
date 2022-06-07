GWMON-65
========

GWMON-65 is intended to be a simple ROM-type system monitor for systems utilizing processors that are compatible with the MOS 6502, including (but not limited to) R6501 and R6511 systems. It is being developed and released under the GNU GPLv3 as open source software (see LICENSE in project root for more information).

This is a WIP port of [GWMON-80](https://github.com/glitchwrks/gwmon-80). *It is definitely not ready for any sort of non-experimental use!*

Currently GWMON-65 targets the [Glitch Works R6501Q SBC](https://www.tindie.com/products/glitchwrks/glitch-works-r6501qr6511q-single-board-computer/)

### Vectors

Vectors provide consistent entry points to GWMON-80. Starting with GWMON-80 0.9, vectors are provided at the beginning of the monitor in the form of a jump table. Standard vectors are:

* `CSTART` at `ORG + 0`: cold start routine
* `WSTART` at `ORG + 3`: warm start routine
* `COUT` at `ORG + 6`: output a character to console
* `CIN` at `ORG + 9`: wait for and input a character from console

SM Command Syntax
-----------------

The Small Monitor (`SM`) command syntax is as follows:

    G XXXX          GO starting at address XXXX

`CTRL+C` may be pressed at any text entry point to cancel the current operation. Hexadecimal inputs are validated, entering a non-hex character will cancel the current operation with an `ERROR` message.

The `SM` command processor automatically inserts the spaces after each element. So, to dump memory from `0x0000` to `0x000F` you'd type

    d0000000f

...and you'd get

    >d 0000 000F
    0000 : xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx
    
    >

...where the xx fields are the hex representation of the bytes at those addresses.

No returns or spaces are typed in the commands. This is very similar to the NorthStar ROM monitor, most likely because it's about the simplest way to implement. Input is auto-downcased, so you can type entries in either (or even mixed) case.

### GO Command

`G` prompts for an address and transfers control to that address. The GWMON-65 warm start address is placed on the stack prior to control transfer, so routines may return to GWMON-65 with a `RTS` as long as the stack has been preserved.

Building GWMON-65
-----------------

Typing `make` will build GWMON-65 and convert it into Intel HEX format. This project uses the assembler from the [CC65 package](https://cc65.github.io/).
