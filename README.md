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

Building GWMON-65
-----------------

Typing `make` will build GWMON-65 and convert it into Intel HEX format. This project uses the assembler from the [CC65 package](https://cc65.github.io/).
