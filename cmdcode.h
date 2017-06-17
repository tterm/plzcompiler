/***************************************************************************
                          cmdcode.h  -  description
                             -------------------
    begin                : Sun Jan 11 2004
    copyright            : (C) 2004 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VARI 1
#define CONS 2
#define FUNC 3
#define IFCL 4

#define  NUL    0
#define  ADD    1
#define  SUB    2
#define  MPY    3
#define  DVD    4
#define  NEG    5
#define  ODD    6
#define  EQL    7
#define  NEQ    8
#define  LSS    9
#define  GTR   10
#define  LEQ   11
#define  GEQ   12
#define  ILIT   13
#define  RET   14
#define  CALL   15
#define  ILOD   16
#define  STO   17
#define  IINT   18
#define  JPU   19
#define  JPC   20
#define  INP   21
#define  PRN   22
#define  ASC   23
#define  PRS   24
#define  PRL   25
#define  FINT  26
#define  FLIT  27
#define  ISTO  28
#define  LOD  29
#define  IPRN  30
#define  FPRN  31
#define  SOUT  32
#define  SPC   33
#define  PDBG  34
#define  STDB  35



char *codebefehle[] =
{
"NUL",	/* 00 */
"ADD",	/* 01 */
"SUB",	/* 02 */
"MPY",	/* 03 */
"DVD",	/* 04 */
"NEG",	/* 05 */
"ODD",	/* 06 */
"EQL",	/* 07 */
"NEQ",	/* 08 */
"LSS",		/* 09 */
"GTR",	/* 10 */
"LEQ",	/* 11 */
"GEQ",	/* 12 */
"ILIT",	/* 13 */
"RET",	/* 14 */
"CALL",	/* 15 */
"ILOD",	/* 16 */
"STO",	/* 17 */
"IINT",	/* 18 */
"JPU",		/* 19 */
"JPC",		/* 20 */
"INP",		/* 21 */
"PRN",	/* 22 */
"ASC",	/* 23 */
"PRS",		/* 24 */
"PRL",	/* 25 */
"FINT",	/* 26 */
"FLIT",	/* 27 */
"ISTO",	/* 28 */
"LOD",	/* 29 */
"IPRN",	/* 30 */
"FPRN",	/* 31 */
"SOUT",	/* 32 */
"SPC",	/* 33 */
"PDBG",	/* 34 */
"STDB"	/* 35 */
};