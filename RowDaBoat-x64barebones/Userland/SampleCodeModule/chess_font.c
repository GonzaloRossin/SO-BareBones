// Created from bdf2c Version 3, (c) 2009, 2010 by Lutz Sammer
//	License AGPLv3: GNU Affero General Public License version 3

#ifndef CHESS_BITMAPS_H
#define CHESS_BITMAPS_H

#include <chess_font.h>

static unsigned char __chess_bitmap__[] = {
//  0 'Black_Pawn'
//	8x8
	________,
	________,
	________,
	___XX___,
	__X__X__,
	___XX___,
	__X__X__,
	__X__X__,
	_XXXXXX_,
	_XXXXXX_,
//  1 'Black_Knight'
//	8x8
    ________,
	____XXX_,
	_XX____X,
	X__XX__X,
	XXX____X,
	___X__X_,
	_XX____X,
	X______X,
	_XXXXXX_,
	XXXXXXXX,
//  2 'Black_Bishop'
//	8x8
    ________,
	___XX___,
	_XX__XX_,
	__X__X__,
	_XXXXXX_,
	__X__X__,
	__X__X__,
	__X__X__,
	_XXXXXX_,
	XXXXXXXX,
//  3 'Black_Rook'
//	8x8
    ________,
	XX_XX_XX,
	XXXXXXXX,
	_X____X_,
	__XXXX__,
	__X__X__,
	__X__X__,
	__X__X__,
	XXXXXXXX,
	XXXXXXXX,
//  4 'Black_Queen'
//	8x8
    ___XX___,
    _XX__XX_,
	X______X,
	_XX__XX_,
	_XXXXXX_,
	__X__X__,
	__X__X__,
	__XXXX__,
	_XXXXXX_,
	XXXXXXXX,
//  5 'Black_King'
//	8x8
    __X__X__,
    ___XX___,
    _XXXXXX_,
	X______X,
	_XXXXXX_,
	__X__X__,
	__X__X__,
	_X____X_,
	_XXXXXX_,
	XXXXXXXX,
//  6 'White_Pawn'
//	8x8
	________,
	________,
	________,
	___XX___,
	__XXXX__,
	___XX___,
	__XXXX__,
	__XXXX__,
	_XXXXXX_,
	_XXXXXX_,
//  7 'White_Knight'
//	8x8
    ________,
	____XXX_,
	_XXXXXXX,
	XXX__XXX,
	XXXXXXXX,
	___XXXX_,
	_XXXXXXX,
	XXXXXXXX,
	_XXXXXX_,
	XXXXXXXX,
//  8 'White_Bishop'
//	8x8
    ________,
	___XX___,
	_XXXXXX_,
	__XXXX__,
	_XXXXXX_,
	__XXXX__,
	__XXXX__,
	__XXXX__,
	_XXXXXX_,
	XXXXXXXX,
//  4 'White_Rook'
//	8x8
    ________,
	XX_XX_XX,
	XXXXXXXX,
	_XXXXXX_,
	__XXXX__,
	__XXXX__,
	__XXXX__,
	__XXXX__,
	XXXXXXXX,
	XXXXXXXX,
//  5 'White_Queen'
//	8x8
    ___XX___,
    _XXXXXX_,
	XXXXXXXX,
	_XXXXXX_,
	_XXXXXX_,
	__XXXX__,
	__XXXX__,
	__XXXX__,
	_XXXXXX_,
	XXXXXXXX,
//  6 'White_King'
//	8x8
    __XXXX__,
    ___XX___,
    _XXXXXX_,
	XXXXXXXX,
	_XXXXXX_,
	__XXXX__,
	__XXXX__,
	_XXXXXX_,
	_XXXXXX_,
	XXXXXXXX,
};

unsigned char * chessBitmap(int color, int piece) {
	return (__chess_bitmap__ + color * 6 * CHESS_PIECE_HEIGHT + piece * CHESS_PIECE_HEIGHT);
}

#endif