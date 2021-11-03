#include <stdio.h>
#include "../lib/argument_parser_t.h"

#define DEFAULT_COLORS "ff0000/0000ff/ffaaaa/aaaaff"
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define DEFAULT_FONT "MERIFONTNEW.TTF"
#define DEFAULT_GAP 0
#define DEFAULT_MAP "opmnvbtrwqlk"
#define DEFAULT_OUT_PATH NULL
#define DEFAULT_SQUARE_SIZE 64

struct args_t
{
	char const * colors = nullptr;
	char const * fen = nullptr;
	char const * font = nullptr;
	char const * map = nullptr;
	char const * out_path = nullptr;
	int gap = 0;
	int square_size = 0;
	int image_size = 0;
	bool ok = false;

	bool parse(int argc, char ** argv)
	{
		ok = false;

		lib::argument_parser_t parser("c:f:g:o:s:t:vh");
		if (!parser.parse(argc, argv))
		{
			if (parser.unknown)
			{
				lib::log("%s: error: unknown option '%c' (\\x%x)\n",
					argv[0], parser.unknown, parser.unknown);
				return false;
			}
			else if (parser.missing)
			{
				lib::log("%s: error: missing argument for option '%c' (\\x%x)\n",
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
		if (parser.opt('t')) map = parser.opt('t');

		out_path = DEFAULT_OUT_PATH;
		if (parser.opt('o')) out_path = parser.opt('o');

		square_size = DEFAULT_SQUARE_SIZE;
		if (parser.opt('s'))
			square_size = atoi(parser.opt('s'));

		image_size = (square_size + gap) << 3;

		return true;
	}

	void print() const
	{
		fprintf(stderr, "args_t{colors=\"%s\", fen=\"%s\", font=\"%s\", gap=%d, map=\"%s\", out_path=\"%s\", square_size=%d, image_size=%d}\n", colors, fen, font, gap, map, out_path, square_size, image_size);
	}

};
