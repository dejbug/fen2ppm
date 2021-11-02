#include <stdio.h>
#include "../lib/argument_parser/argument_parser_t.h"

#define DEFAULT_COLORS "ff0000/0000ff/ffaaaa/aaaaff"
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define DEFAULT_FONT "MERIFONTNEW.TTF"
#define DEFAULT_GAP 0
#define DEFAULT_MAP "opmnvbtrwqlk"
#define DEFAULT_OUT_PATH "fen.ppm"
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
};

bool parse_args(args_t & args, int argc, char ** argv)
{
	args.ok = false;

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
	parser.print();

	args.colors = parser.opt('c');
	if (!args.colors) args.colors = DEFAULT_COLORS;

	args.fen = DEFAULT_FEN;
	if (parser.arg(0)) args.fen = parser.arg(0);

	args.font = DEFAULT_FONT;
	if (parser.opt('f')) args.font = parser.opt('f');

	args.gap = DEFAULT_GAP;
	if (parser.opt('g')) args.gap = atoi(parser.opt('g'));

	args.map = DEFAULT_MAP;
	if (parser.opt('t')) args.map = parser.opt('t');

	args.out_path = DEFAULT_OUT_PATH;
	if (parser.opt('o')) args.out_path = parser.opt('o');

	args.square_size = DEFAULT_SQUARE_SIZE;
	if (parser.opt('s'))
		args.square_size = atoi(parser.opt('s'));

	args.image_size = (args.square_size + args.gap) << 3;

	return true;
}

void print_args(args_t const & args)
{
	fprintf(stderr, "args_t{colors=\"%s\", fen=\"%s\", font=\"%s\", gap=%d, map=\"%s\", out_path=\"%s\", square_size=%d, image_size=%d}\n", args.colors, args.fen, args.font, args.gap, args.map, args.out_path, args.square_size, args.image_size);
}
