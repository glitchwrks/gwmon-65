GWMON-65
========

GWMON-65 is intended to be a simple ROM-type system monitor for systems utilizing processors that are compatible with the MOS 6502, including (but not limited to) R6501 and R6511 systems. It is written in a modular format so that it can be extended for use with specific system hardware with ease. It is being developed and released under the GNU GPLv3 as open source software (see LICENSE in project root for more information).

This is a WIP port of [GWMON-80](https://github.com/glitchwrks/gwmon-80). *It is definitely not ready for any sort of non-experimental use!*

Available Customizations
------------------------

The following customizations are available and can be built using `make` and the contents of the `Make Target` column.

| Make Target | SM | XM | Description                                        |
|-------------|:--:|:--:|----------------------------------------------------|
| `oms6507sbc`| Y  | N  | Omega Micro Systems 6507 Single-Board Computer     |
| `osi1`      | Y  | N  | Ohio Scientific serial systems                     |
| `r65x1qsbc` | Y  | N  | Glitch Works R65X1Q Single-Board Computer          |

Components
----------

GWMON-65 is composed of the following components:

* Jump table
* Core monitor, either `SM` (Small Monitor) or `XM` (eXtended Monitor)
* Command set(s) and handlers
* I/O modules
* Customizations

### Jump Table

The jump table provide consistent entry points to GWMON-65. Vectors are provided at the beginning of the monitor in the form of a jump table. Standard vectors are:

* `CSTART` at `ORG + 0`: cold start routine
* `WSTART` at `ORG + 3`: warm start routine
* `COUT` at `ORG + 6`: output a character to console
* `CIN` at `ORG + 9`: wait for and input a character from console

### Core Monitor

There are two options available for the core monitor: `SM` (Small Monitor) or `XM` (eXtended Monitor). Currently, only `SM` is recommended as `XM` is still under development. `SM` has the advantage of being simpler and much smaller: most customizations using `SM` are 512 bytes or less.

### Command Sets and Handlers

Command sets provide data structures that define commands. The `SM` and `XM` standard command sets (`scmdstd.inc` and `xcmdstd.inc`, respectively) also provide the default command handlers for those core monitors. Command sets are chainable such that additional commands can be added into a given customization. Command sets are terminated by a "NULL command," which is monitor-specific and should be included after all other command sets.

Handlers provide actual implementation of commands. The core command handlers are defined with their command sets, but additional handlers must be defined separately from their command sets to allow command set chaining.

### I/O Modules

I/O modules contain the routines necessary to initialize the console I/O device, receive characters from it, and transmit characters to it. They are generic implementations for a given device; for example, `r6500uc.inc` provides the routines for talking to any system using the built-in serial port on R6500/1 family single-chip microcontrollers.

### Customizations

Customizations tie together vectors, a core monitor, I/O module, and one or more command sets into a machine-specific implementation of GWMON-65. See `smr6501q.a65` for an example of a basic customization of the `SM` for the [Glitch Works R6501Q SBC](https://www.tindie.com/products/glitchwrks/glitch-works-r6501qr6511q-single-board-computer/) with the standard SM command set. See `smosi1.a65` for an example of a customization that requires I/O setup.

Building GWMON-65
-----------------

If a customization for your system already exists, GWMON-65 can be built using the included `Makefile`:

    make r65x1qsbc

...or by assembling the source directly from the command line. Many customizations build to an Intel HEX file, or platform-specific loadable format.

SM Command Syntax
-----------------

The Small Monitor (`SM`) command syntax is as follows:

    D XXXX YYYY     Dump memory from XXXX to YYYY
    E XXXX          Edit memory starting at XXXX (CTRL+C to end)
    G XXXX          GO starting at address 
    L               Load an Intel HEX file into memory

`CTRL+C` may be pressed at any text entry point to cancel the current operation. Hexadecimal inputs are validated, entering a non-hex character will cancel the current operation with an `ERROR` message.

The `SM` command processor automatically inserts the spaces after each element. So, to dump memory from `0x0000` to `0x000F` you'd type

    d0000000f

...and you'd get

    >d 0000 000F
    0000 : xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx
    
    >

...where the xx fields are the hex representation of the bytes at those addresses.

No returns or spaces are typed in the commands. This is very similar to the NorthStar ROM monitor, most likely because it's about the simplest way to implement. Input is auto-downcased, so you can type entries in either (or even mixed) case.

### Edit Memory

The `E` command prompts for a starting address, displays the contents of the memory location, and allows input of a replacement value. Pressing `RETURN` will leave the current value unchanged and move to the next memory address. Pressing `CTRL+C` at any time terminates the `E` command.

### GO Command

`G` prompts for an address and transfers control to that address. The GWMON-65 warm start address is placed on the stack prior to control transfer, so routines may return to GWMON-65 with a `RTS` as long as the stack has been preserved.

### Intel HEX Loader

The Intel HEX loader expects 16-bit addresses. It behaves as an Intel loader should, allowing empty blocks in the middle to be skipped. It will accept either UNIX-style LF endings or DOS/Windows CR/LF endings. The loader will accept an `EOF` (End of File, `0x01`) record or a data record (`0x00`) with zero length as the terminating condition.

After invoking the loader, paste your Intel HEX file into the terminal or do an ASCII upload (depending on your terminal program).

If input or checksum errors are encountered, an asterisk will be printed on the offending line, and `ERROR` will be printed after the hex load completes.

Building GWMON-65
-----------------

Typing `make` will build all GWMON-65 customizations. Many convert into Intel HEX format or other loadable formats. This project uses the assembler from the [CC65 package](https://cc65.github.io/).
