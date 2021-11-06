#include <stdio.h>
#include "../lib/argument_parser_t.h"

#define DEFAULT_COLORS NULL
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define DEFAULT_FONT nullptr
#define DEFAULT_GAP 0
#define DEFAULT_MAP "aAbBcCdDeEfF" // "pPnNbBrRqQkK"
#define DEFAULT_OUT_PATH NULL
#define DEFAULT_SQUARE_SIZE 64

#define MIN_SQUARE_SIZE 8

struct args_t
{
	char const * colors = nullptr;
	char const * fen = nullptr;
	char const * font = nullptr;
	char const * map = nullptr;
	char const * out_path = nullptr;
	int gap = 0;
	int square_size = 0;
	bool ok = false;

	bool parse(int argc, char ** argv)
	{
		ok = false;

		lib::argument_parser_t parser("c:f:g:m:o:s:vh");
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

		colors = parser.opt('c');
		if (!colors) colors = DEFAULT_COLORS;

		fen = DEFAULT_FEN;
		if (parser.arg(0)) fen = parser.arg(0);

		font = DEFAULT_FONT;
		if (parser.opt('f')) font = parser.opt('f');

		gap = DEFAULT_GAP;
		if (parser.opt('g')) gap = atoi(parser.opt('g'));

		map = DEFAULT_MAP;
		if (parser.opt('m')) map = parser.opt('m');

		out_path = DEFAULT_OUT_PATH;
		if (parser.opt('o')) out_path = parser.opt('o');

		square_size = DEFAULT_SQUARE_SIZE;
		if (parser.opt('s'))
			square_size = atoi(parser.opt('s'));

		if (gap < 0) gap = -gap;

		if (square_size < MIN_SQUARE_SIZE)
		{
			lib::err("! The square size of %d is too small to be useful.\n", square_size);
			return false;
		}

		return true;
	}

	void print() const
	{
		fprintf(stderr, "args_t{colors=\"%s\", fen=\"%s\", font=\"%s\", gap=%d, map=\"%s\", out_path=\"%s\", square_size=%d}\n", colors, fen, font, gap, map, out_path, square_size);
	}

};
