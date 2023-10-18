//
// Copyright (c) Yeditepe University Computer Engineering Department, 2005.
// This file is part of HIPPY
//
// Hippy is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Hippy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hippy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "codes.h"

// clang-foramt off
const char *const g_hexConvTable[256] =
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
    "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
    "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

#define INVAL 0x00
const InstDesc g_instOpCodes[107] =
{
    //          rel     imm,    dir,    ind,    ext,    inher
    /*ASL*/   { INVAL,  INVAL,  INVAL,  0x68,   0x78,   INVAL },
    /*BMI*/   { 0x2b,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*ASR*/   { INVAL,  INVAL,  INVAL,  0x67,   0x77,   INVAL },
    /*BLS*/   { 0x23,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*BNE*/   { 0x26,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*SEC*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x0d },
    /*BLT*/   { 0x2d,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*DAA*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x19 },
    /*SEI*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x0f },
    /*EORA*/  { INVAL,  0x88,   0x98,   0xa8,   0xb8,   INVAL },
    /*EORB*/  { INVAL,  0xc8,   0xd8,   0xe8,   0xf8,   INVAL },
    /*NEG*/   { INVAL,  INVAL,  INVAL,  0x60,   0x70,   INVAL },
    /*SEV*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x0b },
    /*TAB*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x16 },
    /*BPL*/   { 0x2a,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*BRA*/   { 0x20,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*ANDA*/  { INVAL,  0x84,   0x94,   0xa4,   0xb4,   INVAL },
    /*ANDB*/  { INVAL,  0xc4,   0xd4,   0xe4,   0xf4,   INVAL },
    /*LSR*/   { INVAL,  INVAL,  INVAL,  0x64,   0x74,   INVAL },
    /*TBA*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x17 },
    /*ROL*/   { INVAL,  INVAL,  INVAL,  0x69,   0x79,   INVAL },
    /*CLC*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x0c },
    /*DEC*/   { INVAL,  INVAL,  INVAL,  0x6a,   0x7a,   INVAL },
    /*TAP*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x06 },
    /*ROR*/   { INVAL,  INVAL,  INVAL,  0x66,   0x76,   INVAL },
    /*CLI*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x0e },
    /*ADCA*/  { INVAL,  0x89,   0x99,   0xa9,   0xb9,   INVAL },
    /*ADCB*/  { INVAL,  0xc9,   0xd9,   0xe9,   0xf9,   INVAL },
    /*LSRA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x44 },
    /*LSRB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x54 },
    /*ADDA*/  { INVAL,  0x8b,   0x9b,   0xab,   0xbb,   INVAL },
    /*ADDB*/  { INVAL,  0xcb,   0xdb,   0xeb,   0xfb,   INVAL },
    /*BSR*/   { 0x8d,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*CLR*/   { INVAL,  INVAL,  INVAL,  0x6f,   0x7f,   INVAL },
    /*DES*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x34 },
    /*CLV*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x0a },
    /*BVC*/   { 0x28,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*DEX*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x09 },
    /*COMA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x43 },
    /*COMB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x53 },
    /*RTI*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x3B },
    /*COM*/   { INVAL,  INVAL,  INVAL,  0x63,   0x73,   INVAL },
    /*PSHA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x36 },
    /*PSHB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x37 },
    /*ASLA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x48 },
    /*BVS*/   { 0x29,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*ASLB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x58 },
    /*RTS*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x39 },
    /*STAA*/  { INVAL,  INVAL,  0x97,   0xa7,   0xb7,   INVAL },
    /*ROLA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x49 },
    /*STAB*/  { INVAL,  INVAL,  0xd7,   0xe7,   0xf7,   INVAL },
    /*ROLB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x59 },
    /*CPX*/   { INVAL,  0x8c,   0x9c,   0xac,   0xbc,   INVAL },
    /*INC*/   { INVAL,  INVAL,  INVAL,  0x6c,   0x7c,   INVAL },
    /*DECA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x4a },
    /*NOP*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x01 },
    /*DECB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x5a },
    /*INS*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x31 },
    /*INX*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x08 },
    /*ASRA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x47 },
    /*ASRB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x57 },
    /*RORA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x46 },
    /*RORB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x56 },
    /*ABA*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x1b },
    /*STS*/   { INVAL,  INVAL,  0x9f,   0xaf,   0xbf,   INVAL },
    /*TPA*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x07 },
    /*STX*/   { INVAL,  INVAL,  0xdf,   0xef,   0xff,   INVAL },
    /*SUBA*/  { INVAL,  0x80,   0x90,   0xa0,   0xb0,   INVAL },
    /*SUBB*/  { INVAL,  0xc0,   0xd0,   0xe0,   0xf0,   INVAL },
    /*SWI*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x3f },
    /*ORAA*/  { INVAL,  0x8a,   0x9a,   0xaa,   0xba,   INVAL },
    /*ORAB*/  { INVAL,  0xca,   0xda,   0xea,   0xfa,   INVAL },
    /*JMP*/   { INVAL,  INVAL,  INVAL,  0x6e,   0x7e,   INVAL },
    /*TST*/   { INVAL,  INVAL,  INVAL,  0x6d,   0x7d,   INVAL },
    /*TSX*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x30 },
    /*WAI*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x3e },
    /*BCC*/   { 0x24,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*LDS*/   { INVAL,  0x8e,   0x9e,   0xae,   0xbe,   INVAL },
    /*LDX*/   { INVAL,  0xce,   0xde,   0xee,   0xfe,   INVAL },
    /*JSR*/   { INVAL,  INVAL,  INVAL,  0xad,   0xbd,   INVAL },
    /*BCS*/   { 0x25,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*PULA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x32 },
    /*PULB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x33 },
    /*TXS*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x35 },
    /*CLRA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x4f },
    /*CLRB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x5f },
    /*INCA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x4c },
    /*INCB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x5c },
    /*BEQ*/   { 0x27,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*BGE*/   { 0x2c,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*LDAA*/  { INVAL,  0x86,   0x96,   0xa6,   0xb6,   INVAL },
    /*LDAB*/  { INVAL,  0xc6,   0xd6,   0xe6,   0xf6,   INVAL },
    /*CBA*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x11 },
    /*BHI*/   { 0x22,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*BGT*/   { 0x2e,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*NEGA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x40 },
    /*NEGB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x50 },
    /*SBCA*/  { INVAL,  0x82,   0x92,   0xa2,   0xb2,   INVAL },
    /*SBCB*/  { INVAL,  0xc2,   0xd2,   0xe2,   0xf2,   INVAL },
    /*SBA*/   { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x10 },
    /*CMPA*/  { INVAL,  0x81,   0x91,   0xa1,   0xb1,   INVAL },
    /*BITA*/  { INVAL,  0x85,   0x95,   0xa5,   0xb5,   INVAL },
    /*CMPB*/  { INVAL,  0xc1,   0xd1,   0xe1,   0xf1,   INVAL },
    /*BITB*/  { INVAL,  0xc5,   0xd5,   0xe5,   0xf5,   INVAL },
    /*BLE*/   { 0x2f,   INVAL,  INVAL,  INVAL,  INVAL,  INVAL },
    /*TSTA*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x4d },
    /*TSTB*/  { INVAL,  INVAL,  INVAL,  INVAL,  INVAL,  0x5d }
};
#undef INVAL

/*
 * INSTRUCTION DESCRIPTION TABLE (IDT)
 * HIGHER ORDER 4BITS --> CLOCK CYCLES
 * LOWER  ORDER 4BITS --> NUMBER OF BYTES
 */

const unsigned char g_instDescTable[0x100] =
{
    0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x41, 0x41, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x00, 0x21, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x51, 0x00, 0xA1, 0x00, 0x00, 0x91, 0xC1,
    0x21, 0x00, 0x00, 0x21, 0x21, 0x00, 0x21, 0x21, 0x21, 0x21, 0x21, 0x00, 0x21, 0x21, 0x00, 0x21,
    0x21, 0x00, 0x00, 0x21, 0x21, 0x00, 0x21, 0x21, 0x21, 0x21, 0x21, 0x00, 0x21, 0x21, 0x00, 0x21,
    0x72, 0x00, 0x00, 0x72, 0x72, 0x00, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x72, 0x72, 0x42, 0x72,
    0x63, 0x00, 0x00, 0x63, 0x63, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x00, 0x63, 0x63, 0x33, 0x63,
    0x22, 0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x22, 0x33, 0x82, 0x33, 0x00,
    0x32, 0x32, 0x32, 0x00, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x42, 0x00, 0x42, 0x52,
    0x52, 0x52, 0x52, 0x00, 0x52, 0x52, 0x52, 0x62, 0x52, 0x52, 0x52, 0x52, 0x62, 0x82, 0x62, 0x72,
    0x43, 0x43, 0x43, 0x00, 0x43, 0x43, 0x43, 0x53, 0x43, 0x43, 0x43, 0x43, 0x53, 0x93, 0x53, 0x63,
    0x22, 0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x33, 0x00,
    0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x00, 0x00, 0x42, 0x52,
    0x52, 0x52, 0x52, 0x00, 0x52, 0x52, 0x52, 0x62, 0x52, 0x52, 0x52, 0x52, 0x00, 0x00, 0x62, 0x72,
    0x43, 0x43, 0x43, 0x00, 0x43, 0x43, 0x43, 0x53, 0x43, 0x43, 0x43, 0x43, 0x00, 0x00, 0x53, 0x63
};

// clang-format on
