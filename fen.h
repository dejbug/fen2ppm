#pragma once
#include "lib/lib.h"

#define FEN_LEN_MAX 64+8	// A board with a piece on every square.
#define FEN_LEN_MIN 8+7		// An empty board.
#define RAW_LEN 64			// Length of "unpacked" FEN (without the '\0').
#define MAP_LEN 12			// Length of translation table (without the '\0').

typedef enum { None='\0', Black='b', White='w' } Side;

bool fen_unpack(char raw[RAW_LEN+1], char col[RAW_LEN+1], char const * fen)
{
	memset(col, Side::None, RAW_LEN);
	col[RAW_LEN] = '\0';

	// ASSUME: fen is a non-empty string.
	if (!fen || !*fen) return false;

	// ASSUME: fen is zero-terminated.
	size_t const fen_len = strlen(fen);

	// ASSUME: fen is long enough to represent the shortest fen.
	if (fen_len < FEN_LEN_MIN) return false;

	// ASSUME: fen has no leading whitespace.
	if (isspace(fen[0])) return false;

	size_t raw_len = 0;
	for (size_t k=0; k<FEN_LEN_MAX && *fen; ++k, ++fen)
	{
		// lib::log("(k=%d, raw_len=%d, *fen=%c)\n", k, raw_len, *fen);
		if(isspace(*fen))
			break;
		else if (*fen == '/')
		{
			if (raw_len % 8 != 0)
				return false;
		}
		else if (isdigit(*fen))
		{
			int const d = (*fen - '0');
			if (d < 1 || d > 8) return false;
			for (int i=0; i<d; ++i)
			{
				if (raw_len >= RAW_LEN) return false;
				raw[raw_len++] = '-';
			}
		}
		else if (strchr("qkrbnp", *fen))
		{
			if (raw_len >= RAW_LEN) return false;
			col[raw_len] = Side::Black;
			raw[raw_len] = *fen;
			++raw_len;
		}
		else if (strchr("QKRBNP", *fen))
		{
			if (raw_len >= RAW_LEN) return false;
			col[raw_len] = Side::White;
			raw[raw_len] = *fen;
			++raw_len;
		}
		else return false;
	}
	raw[RAW_LEN] = '\0';
	if (raw_len != RAW_LEN) return false;
	return true;
}

char fen_translate(char c, char const map[MAP_LEN+1])
{
	switch (c)
	{
		case 'p': return map[0];
		case 'P': return map[1];
		case 'n': return map[2];
		case 'N': return map[3];
		case 'b': return map[4];
		case 'B': return map[5];
		case 'r': return map[6];
		case 'R': return map[7];
		case 'q': return map[8];
		case 'Q': return map[9];
		case 'k': return map[10];
		case 'K': return map[11];
		default: return ' ';
	}
}

bool fen_translate(char raw[RAW_LEN+1], char const map[MAP_LEN+1])
{
	for (size_t i=0; i<RAW_LEN; ++i)
	{
		raw[i] = fen_translate(raw[i], map);
	}
	return true;
}
