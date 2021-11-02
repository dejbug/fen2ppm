#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>

#include "lib/lib.h"
#include "lib/gdi.h"
#include "lib/memory_dc_t.h"
#include "app/args_t.h"
#include "app/theme_t.h"

#include "fen.h"
#include "bitmap.h"
#include "grid_t.h"
#include "chessfont_t.h"

// FIXME: Split up lib::log into lib::print and lib::trace. Both printing
//	to stderr with only the latter being conditional on NDEBUG.

// FIXME: The TextOUt logic doesn't have a way of distinguishing yet between
//	dark and light pieces, so it draws them the same way (i.e. theme.dp).

int main(int argc, char ** argv)
{
	// lib::print_argv(argv);

	args_t args;
	if (!parse_args(args, argc, argv))
		return EXIT_FAILURE;
	// print_args(args);

	theme_t theme;
	int const count = theme.parse(args.colors);
	lib::log("THEME: %d COLORS matched\n", count);
	theme.print();

	lib::log("fen raw: |%s|\n", args.fen);
	char raw[64+1] = {0};
	if (!args.fen || !*args.fen || !fen_unpack(raw, args.fen))
	{
		lib::log("! Was unable to unpack FEN: \"%s\"\n", args.fen);
		return EXIT_FAILURE;
	}
	lib::log("fen unp: |%s|\n", raw);
	if (!args.map || !fen_translate(raw, args.map))
	{
		lib::log("! Was unable to translate FEN \"%s\" with map: \"%s\"\n",
			args.fen, args.map);
		return EXIT_FAILURE;
	}
	lib::log("fen map: |%s|\n", raw);

	memory_dc_t mdc;
	if (!mdc.create(args.image_size))
	{
		lib::log("! Was unable to create DC.\n");
		return EXIT_FAILURE;
	}
	HDC const dc = mdc.dc;
	HBITMAP const bmp = mdc.bmp;

	grid_t grid;
	grid.set_size(8, 8);
	grid.set_square_size(args.square_size, args.gap);

	chessfont_t font;
	if (!font.install(args.font))
	{
		lib::log("! \"%s\" is neither a font path nor a font face name.\n", args.font);
		return EXIT_FAILURE;
	}
	if (font.path) lib::log("Chess font path is |%s|.\n", font.path);
	if (font.name) lib::log("Chess font facename is |%s|.\n", font.name);
	if (!font.create(args.square_size))
	{
		lib::log("! Was unable to instantiate font \"%s\".\n", font.name);
		return EXIT_FAILURE;
	}
	lib::log("Chess font handle is %p\n", (void *)font.handle);

	SelectObject(dc, font.handle);
	SelectObject(dc, GetStockObject(DC_BRUSH));
	SetBkMode(dc, TRANSPARENT);

	COLORREF const dk = theme.ds ? theme.ds : RGB(209,209,209);
	COLORREF const lt = theme.ls ? theme.ls : RGB(253,183,183);
	COLORREF const dp = theme.dp ? theme.dp : RGB(220,160,90);
	// COLORREF const lp = theme.lp ? theme.lp : RGB(220,160,90);

	// HPEN pen = CreatePen(PS_SOLID, gap >> 1, RGB(220,100,100));
	// SelectObject(dc, pen);

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(dc, grid.edge, grid.edge, m, lib::CenteringMode::MAX);

	size_t fen_i=0;
	char fen_buf[2] = {'\0', '\0'};

	for (size_t row=0; row<8; ++row)
		for (size_t col=0; col<8; ++col)
		{
			fen_buf[0] = raw[fen_i++];

			RECT r;
			grid.get_square_rect(r, col, row);
			SetDCBrushColor(dc, ((row + col) % 2 == 0) ? lt : dk);
			SelectObject(dc, GetStockObject(NULL_PEN));
			Rectangle(dc, r.left, r.top, r.right, r.bottom);

			SetTextColor(dc, dp);
			TextOut(dc, r.left + m.cx, r.top + m.cy, fen_buf, 1);
		}

	if (args.out_path)
	{
		lib::log("* WRITING\n");
		FILE * outfile = fopen(args.out_path, "wb");
		dump_bitmap_data(dc, bmp, outfile);
		fclose(outfile);
	}
	else
	{
		lib::log("* PRINTING\n");
		dump_bitmap_data(dc, bmp, stdout);
	}

	return EXIT_SUCCESS;
}
