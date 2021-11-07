#include <stdio.h>
#include "../lib/lib.h"
#include "../lib/argument_parser_t.h"

struct args_t
{
	// We keep the parser around so we can test whether the values of the
	// parameters were passed as such by user or were the result of defaulting.
	lib::argument_parser_t parser;

	char const * colors = nullptr;
	char const * fen = nullptr;
	char const * font = nullptr;
	char const * map = nullptr;
	char const * out_path = nullptr;
	int gap = 0;
	int square_size = 0;

	int map_length = 0;

	args_t() : parser("c:f:g:m:o:s:vh") {}

	bool parse(int argc, char ** argv)
	{
		if (!parser.parse(argc, argv))
		{
			if (parser.unknown)
			{
				lib::err("%s: error: unknown option '%c' (\\x%x)\n",
					argv[0], parser.unknown, parser.unknown);
				return false;
			}
			else if (parser.missing)
			{
				lib::err("%s: error: missing argument for option '%c' (\\x%x)\n",
					argv[0], parser.missing, parser.missing);
				return false;
			}
			return false;
		}
		// parser.print();

		// TODO: I wanted to keep this class lean but we might want to
		// aid modularisation by providing all the parameters with defaults.
		// If external logic needs to dstinguish between user-passed and
		// defaulted values it can query the retained parser object.

		colors = parser.opt('c');
		if (!colors || !*colors) colors = DEFAULT_COLORS;

		fen = parser.arg(0);
		if (!fen || !*fen)
		{
			lib::log("* No FEN supplied; using FEN of standard starting position.\n");
			fen = DEFAULT_FEN;
		}

		font = parser.opt('f');
		if (!font || !*font)
		{
			lib::log("* No font supplied; using built in font.\n");
			font = DEFAULT_FONT;
		}

		gap = DEFAULT_GAP;
		if (parser.opt('g'))
			gap = atoi(parser.opt('g'));
		if (gap < 0) gap = -gap;

		// It is difficult to provide a default map. One would have
		// to try and derive a map from the inspecting the sizes of
		// the glyphs of the font; an expensive operation likely to
		// fail anyway.
		map = parser.opt('m');
		if (map && !*map) map = nullptr;

		map_length = map ? strlen(map) : 0;

		// Three kinds of map can be passed, distinguishable by length.
		if (map_length != 12 && map_length != 26)
		{
			// A map was passed.
			if (map_length != 0)
			{
				int const index = atoi(map);
				if (index >= 0 && index < MAP_INTERNAL_COUNT)
				{
					map = map_internal_strings[index];
					map_length = map_internal_lengths[index];
				}
				else
				{
					lib::log("* The map is neither a string of length 12 or 26, nor is it a valid integer index into the internal table of predefined/common maps.\n", map_length);
					return false;
				}
			}
			// If user passed neither font nor map, then the font will
			// have defaulted, so we know to use the default map, too.
			else if (lib::is_empty(parser.opt('f')))
			{
				// The default font mode is newspaper mode, so we use
				// the 26-digit map.
				map = MAP_INTERNAL_26;
				map_length = 26;
			}
		}

		// No out_path just means to print to STDOUT.
		out_path = parser.opt('o');
		if (out_path && !*out_path) out_path = nullptr;

		square_size = DEFAULT_SQUARE_SIZE;
		if (parser.opt('s'))
			square_size = atoi(parser.opt('s'));

		if (square_size < MIN_SQUARE_SIZE)
		{
			lib::log("* A square size of %d is impractically small. Defaulting to %d.\n", square_size, MIN_SQUARE_SIZE);
			square_size = MIN_SQUARE_SIZE;
		}
		return true;
	}

	void print() const
	{
		fprintf(stderr, "args_t{colors=\"%s\", fen=\"%s\", font=\"%s\", gap=%d, map=\"%s\", out_path=\"%s\", square_size=%d}\n", colors, fen, font, gap, map, out_path, square_size);
	}

};
