/** \file compiler_defs.h
 *
 * \brief Register/bit definitions for cross-compiler projects on the C8051F93x/2x family.
 *
 * \b COPYRIGHT
 * \n Portions of this file are copyright Maarten Brock
 * \n http://sdcc.sourceforge.net
 * \n Portions of this file are copyright 2010, Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 *
 *
 * <b> GNU LGPL boilerplate: </b>
 * \n This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * \n This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * \n You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 * \n In other words, you are welcome to use, share and improve this program.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them. Help stamp out software-hoarding!
 *
 * <b> Program Description: </b>
 * \note This header file should be included before including
 * a device-specific header file such as C8051F300_defs.h.
 *
 * Macro definitions to accommodate 8051 compiler differences in specifying
 * special function registers and other 8051-specific features such as NOP
 * generation, and locating variables in memory-specific segments.  The
 * compilers are identified by their unique predefined macros. See also:
 * http://predef.sourceforge.net/precomp.html
 *
 * SBIT and SFR define special bit and special function registers at the given
 * address. SFR16 and SFR32 define sfr combinations at adjacent addresses in
 * little-endian format. SFR16E and SFR32E define sfr combinations without
 * prerequisite byte order or adjacency. None of these multi-byte sfr
 * combinations will guarantee the order in which they are accessed when read
 * or written.
 *
 * SFR16X and SFR32X for 16 bit and 32 bit xdata registers are not defined
 * to avoid portability issues because of compiler endianness.
 *
 * Example:
 * \code
 * // my_mcu.c: main 'c' file for my mcu
 * #include <compiler_defs.h>  // this file
 * #include <C8051xxxx_defs.h> // SFR definitions for specific MCU target
 *
 * SBIT  (P0_1, 0x80, 1);      // Port 0 pin 1
 * SFR   (P0, 0x80);           // Port 0
 * SFRX  (CPUCS, 0xE600);      // Cypress FX2 Control and Status register in
 *                             // xdata memory at 0xE600
 * SFR16 (TMR2, 0xCC);         // Timer 2, lsb at 0xCC, msb at 0xCD
 * SFR16E(TMR0, 0x8C8A);       // Timer 0, lsb at 0x8A, msb at 0x8C
 * SFR32 (MAC0ACC, 0x93);      // SiLabs C8051F120 32 bits MAC0 Accumulator,
 *                             // lsb at 0x93, msb at 0x96
 * SFR32E(SUMR, 0xE5E4E3E2);   // TI MSC1210 SUMR 32 bits Summation register,
 *                             // lsb at 0xE2, msb at 0xE5
 * \endcode
 *
 * <b>Target:</b>         C8051xxxx
 * \n <b>Tool chain:</b>     Generic
 * \n <b>Command Line:</b>   None
 *
 * Release 2.3 - 27 MAY 2010 (DM)
 * \n    -Removed 'LOCATED_VARIABLE' pragma from Keil because it is not supported
 * \n Release 2.2 - 06 APR 2010 (ES)
 * \n    -Removed 'PATHINCLUDE' pragma from Raisonance section
 * \n Release 2.1 - 16 JUL 2009 (ES)
 * \n    -Added SEGMENT_POINTER macro definitions for SDCC, Keil, and Raisonance
 * \n    -Added LOCATED_VARIABLE_NO_INIT macro definitions for Raisonance
 * \n Release 2.0 - 19 MAY 2009 (ES)
 * \n    -Added LOCATED_VARIABLE_NO_INIT macro definitions for SDCC and Keil
 * \n Release 1.9 - 23 OCT 2008 (ES)
 * \n    -Updated Hi-Tech INTERRUPT and INTERRUPT_USING macro definitions
 * \n    -Added SFR16 macro defintion for Hi-Tech
 * \n Release 1.8 - 31 JUL 2008 (ES)
 * \n    -Added INTERRUPT_USING and FUNCTION_USING macro's
 * \n    -Added macro's for IAR
 * \n    -Corrected Union definitions for Hi-Tech and added SFR16 macro defintion
 * \n Release 1.7 - 11 SEP 2007 (BW)
 * \n    -Added support for Raisonance EVAL 03.03.42 and Tasking Eval 7.2r1
 * \n Release 1.6 - 27 AUG 2007 (BW)
 * \n    -Updated copyright notice per agreement with Maartin Brock
 * \n    -Added SDCC 2.7.0 "compiler.h" bug fixes
 * \n    -Added memory segment defines (SEG_XDATA, for example)
 * \n Release 1.5 - 24 AUG 2007 (BW)
 * \n    -Added support for NOP () macro
 * \n    -Added support for Hi-Tech ver 9.01
 * \n Release 1.4 - 07 AUG 2007 (PKC)
 * \n    -Removed FID and fixed formatting.
 * \n Release 1.3 - 30 SEP 2007 (TP)
 * \n    -Added INTERRUPT_PROTO_USING to properly support ISR context switching
 *      under SDCC.
 * \n Release 1.2 - (BW)
 * \n    -Added support for U8,U16,U32,S8,S16,S32,UU16,UU32 data types
 * \n Release 1.1 - (BW)
 * \n    -Added support for INTERRUPT, INTERRUPT_USING, INTERRUPT_PROTO,
 *      SEGMENT_VARIABLE, VARIABLE_SEGMENT_POINTER,
 *      SEGMENT_VARIABLE_SEGMENT_POINTER, and LOCATED_VARIABLE
 * \n Release 1.0 - 29 SEP 2006 (PKC)
 * \n    -Initial revision
 */

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef COMPILER_DEFS_H
#define COMPILER_DEFS_H

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

// SDCC - Small Device C Compiler
// http://sdcc.sourceforge.net

#include <stm8s.h>
#include <intrinsics.h>

#ifndef NULL
#define NULL 0
#endif


// used with UU16
# define LSB 0
# define MSB 1

// used with UU32 (b0 is least-significant byte)
# define b0 0
# define b1 1
# define b2 2
# define b3 3

typedef bool BIT;
typedef unsigned char U8;
typedef unsigned int U16;
typedef unsigned long U32;

typedef signed char S8;
typedef signed int S16;
typedef signed long S32;

typedef union UU16
{
    U16 U16;
    S16 S16;
    U8 U8[2];
    S8 S8[2];
} UU16;

typedef union UU32
{
    U32 U32;
    S32 S32;
    UU16 UU16[2];
    U16 U16[2];
    S16 S16[2];
    U8 U8[4];
    S8 S8[4];
} UU32;

#define BITS(bitArray, bitPos)  BIT bitArray ## bitPos
#define WRITE_TO_BIT_ARRAY(bitArray, byte)  bitArray ## 0 = byte & 0x01; \
                                            bitArray ## 1 = byte & 0x02; \
                                            bitArray ## 2 = byte & 0x04; \
                                            bitArray ## 3 = byte & 0x08; \
                                            bitArray ## 4 = byte & 0x10; \
                                            bitArray ## 5 = byte & 0x20; \
                                            bitArray ## 6 = byte & 0x40; \
                                            bitArray ## 7 = byte & 0x80;

#define READ_FROM_BIT_ARRAY(bitArray, byte) byte =  (bitArray ## 0) | \
                                                   ((bitArray ## 1) << 1) | \
                                                   ((bitArray ## 2) << 2) | \
                                                   ((bitArray ## 3) << 3) | \
                                                   ((bitArray ## 4) << 4) | \
                                                   ((bitArray ## 5) << 5) | \
                                                   ((bitArray ## 6) << 6) | \
                                                   ((bitArray ## 7) << 7);

#define NOP() __no_operation();
//#define TRUE          1
//#define FALSE         0
//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define COMPILER_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
