/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 1999-2001 Christian Höltje

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/* CUBES
 * Coordinates and sizes for drawing the dice
 * TL means Top-Left: ie TLx is the Top Left x coordinate
 */

#define CUBETLx		2 
#define CUBETLy		19
#define CUBESpace	2  // Spacing between cubes
#define CUBESize	20 // Size, x and y

/* UI constants */
#define idDie0		9000
#define idDie1		9001
#define idDie2		9002
#define idDie3		9003
#define idDie4		9004

#define fldNGname0	9010
#define fldNGname1	9011
#define fldNGname2	9012
#define fldNGname3	9013
#define fldNGname4	9014
#define fldNGname5	9015
#define fldNGname6	9016
#define fldNGname7	9017

/* PLAYER
 * Coodinates and extents for drawing the player names and scores
 * TL means TopLeft
 * EX means Extent
 */
#define SYMBOLx		36

#define PLAYERTLx	50
#define PLAYERTLy	52
#define PLAYEREXx	74
#define PLAYEREXy	12

#define SCOREx		130

#define PLAYERMaxName   10


/* NewGame Positions
 */

#define NewGameUser	5
#define NewGameUserSize 11
#define NewGameTop	15
#define NewGameName	(NewGameUser + NewGameUserSize + 2)
#define NewGameNames	80
#define NewGameRight	(NewGameUser + NewGameUserSize + NewGameNames + 5)

