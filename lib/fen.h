#pragma once
#include <assert.h>
#include "lib.h"

#define FEN_LEN_MAX 64+8	// A board with a piece on every square.
#define FEN_LEN_MIN 8+7		// An empty board.
#define RAW_LEN 64			// Length of "unpacked" FEN (without the '\0').
#define MAP_LEN 12			// Length of translation table (without the '\0').
#define MAP_LEN_2 26		// Length of extended translation table (without the '\0').

typedef enum { None='\0', Dark='d', Light='l' } Side;

bool fen_unpack(char (&raw)[RAW_LEN+1], char (&col)[RAW_LEN+1], char const * fen)
{
	// ASSUME: fen is a non-empty string.
	if (!fen || !*fen) return false;

	// ASSUME: fen is zero-terminated.
	size_t const fen_len = strlen(fen);

	// ASSUME: fen is long enough to represent the shortest fen.
	if (fen_len < FEN_LEN_MIN) return false;

	// ASSUME: fen has no leading whitespace.
	if (isspace(fen[0])) return false;

	memset(col, Side::None, RAW_LEN);
	col[RAW_LEN] = '\0';

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
			col[raw_len] = Side::Dark;
			raw[raw_len] = *fen;
			++raw_len;
		}
		else if (strchr("QKRBNP", *fen))
		{
			if (raw_len >= RAW_LEN) return false;
			col[raw_len] = Side::Light;
			raw[raw_len] = *fen;
			++raw_len;
		}
		else return false;
	}
	raw[RAW_LEN] = '\0';
	if (raw_len != RAW_LEN) return false;
	return true;
}

char fen_toggle(char c)
{
	switch (c)
	{
		case 'p': return 'P';
		case 'P': return 'p';
		case 'n': return 'N';
		case 'N': return 'n';
		case 'b': return 'B';
		case 'B': return 'b';
		case 'r': return 'R';
		case 'R': return 'r';
		case 'q': return 'Q';
		case 'Q': return 'q';
		case 'k': return 'K';
		case 'K': return 'k';
	}
	return c;
}

char fen2font(char c, char const * map)
{
	assert(!(!map || !*map));
	assert(strlen(map) >= MAP_LEN);

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
	}
	return '\0';
}

char fen2font(char c, char const * map, int col, int row)
{
	assert(!(!map || !*map));
	assert(strlen(map) >= MAP_LEN_2);

	bool const is_light_square = ((row + col) % 2 == 0);
	int const i = is_light_square ? 0 : 1;

	switch (c)
	{
		case 'P': return map[2*0+i];
		case 'p': return map[2*1+i];
		case 'N': return map[2*2+i];
		case 'n': return map[2*3+i];
		case 'B': return map[2*4+i];
		case 'b': return map[2*5+i];
		case 'R': return map[2*6+i];
		case 'r': return map[2*7+i];
		case 'Q': return map[2*8+i];
		case 'q': return map[2*9+i];
		case 'K': return map[2*10+i];
		case 'k': return map[2*11+i];
		default : return map[2*12+i];
	}
	return '\0';
}

struct fen_t
{
	char raw[RAW_LEN+1] = {0};	// As parsed.
	char fen[RAW_LEN+1] = {0};	// After mapping.
	char col[RAW_LEN+1] = {0};
	char const * map = nullptr;

	bool parse(char const * text)
	{
		raw[RAW_LEN] = '\0';
		fen[RAW_LEN] = '\0';
		col[RAW_LEN] = '\0';

		lib::log("fen raw: |%s|\n", text);
		if (!text || !*text || !fen_unpack(raw, col, text))
		{
			lib::err("! Was unable to unpack FEN: \"%s\"\n", text);
			return false;
		}
		lib::log("fen unp: |%s|\n", raw);
		return true;
	}

	Side get_side(size_t i) const
	{
		if (!isalpha(raw[i])) return Side::None;
		return islower(raw[i]) ? Side::Dark : Side::Light;
	}

	char get(size_t i) const
	{
		if (i >= RAW_LEN) return '\0';
		return raw[i];
	}

	char get(size_t i, char const * map) const
	{
		if (i >= RAW_LEN) return '\0';
		return fen2font(raw[i], map);
	}

	char get(size_t i, char const * map, int col, int row) const
	{
		if (i >= RAW_LEN) return '\0';
		return fen2font(raw[i], map, col, row);
	}

	char get_inv(size_t i, char const * map) const
	{
		if (i >= RAW_LEN) return '\0';
		if (col[i] == Side::None) return get(i, map);
		return fen2font(fen_toggle(raw[i]), map);
	}
};
