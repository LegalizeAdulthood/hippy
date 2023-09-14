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
#include "disassembler.h"

/* Converts the given array of bytes into a string containing their hexadecimal
 * representations. assumes that the buffer, pcStr is twice as much as the requested
 * number of bytes to decode (szByteArray) returns the number of bytes written.
 */
int HexDumper::ByteArrayToHexArray(BYTE * pbByteArray, int szByteArray, char * pcStr){
	char * p = pcStr;
	while(szByteArray){
		pcStr[0] = HexTable[*pbByteArray / 16];
		pcStr[1] = HexTable[*pbByteArray % 16];
		pcStr+=2;
		pbByteArray++;
		szByteArray--;
	}

	pcStr[0] = 0;
	return (int) (pcStr - p);
}

/* Converts the given array of bytes into a string containing their hexadecimal
 * representations. assumes that the buffer, pcStr is twice more than the requested
 * number of bytes to decode (szByteArray). This function also inserts blank characters
 * after each byte.
 */
void HexDumper::ByteArrayToHexArrayEx(BYTE * pbByteArray, int szByteArray, char * pcStr){
	while(szByteArray){
		pcStr[0] = HexTable[*pbByteArray / 16];
		pcStr[1] = HexTable[*pbByteArray % 16];
		pcStr[2] = ' ';
		pcStr+=3;
		pbByteArray++;
		szByteArray--;
	}

	pcStr[0] = 0;
}

/* Simply outputs the hexadecimal representaion of "b" into pcStr. Which should be 
 * 3 chars (minimum). Null character at the end is also put.
 */
void HexDumper::ByteToHex(BYTE b, char * pcStr){
	pcStr[0] = HexTable[b / 16];
	pcStr[1] = HexTable[b % 16];
	pcStr[2] = 0;
}

/*
 * Disassembles given byte array os size szByteArray. Writes the out put to
 * pcSTr. Only Disassembles one instruction and returns the length (in bytes)
 * of that instruction. if left most byte is not a val,d instruction or
 * the length of that instruction is larger than szByteArray then returns -1.
 * in that case INVALID is coppied to the pcStr.
 */
int Disassembler::Dasm(BYTE * pbByteArray, int szByteArray, char * pcStr){
	int opcode  = *pbByteArray;
	int numread = InstDescTbl[opcode] & 0x0F;
	char pcAddr[5];

	if(numread>szByteArray || !opcode){
		strcpy(pcStr, "INVALID");
		return INVALID_CODE;
	}

	if(numread == 2) hexer.ByteToHex(pbByteArray[1], pcAddr);
	else if(numread == 3) hexer.ByteArrayToHexArray(&pbByteArray[1], 2, pcAddr);

	switch(InstModes[opcode]){
	case amNon:
			strcpy(pcStr, "INVALID");
			return INVALID_CODE;
		break;
	case amRel:
	case amDir:
	case amExt:
		    if(strlen(codes[opcode])==3) sprintf(pcStr, "%s      $%s",codes[opcode], pcAddr);
			else sprintf(pcStr, "%s     $%s",codes[opcode], pcAddr);
		break;
	case amImm:
		if(strlen(codes[opcode])==3) sprintf(pcStr,"%s      #$%s",codes[opcode], pcAddr);
		else sprintf(pcStr, "%s     #$%s",codes[opcode], pcAddr);
		break;
	case amInd:
		sprintf(pcStr, "%s     $%s, X",codes[opcode], pcAddr);
		break;
	case amImp:
			strcpy(pcStr, codes[opcode]);
		break;
	}

	return numread;
}