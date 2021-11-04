#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>

#include "lib/lib.h"
#include "lib/out.h"

#include "lib/fen.h"
#include "lib/grid_t.h"
#include "lib/chessfont_t.h"

#include "lib/gdi.h"
#include "lib/bitmap.h"
#include "lib/memory_dc_t.h"

#include "app/args_t.h"
#include "app/theme_t.h"

// FIXME: Add a USAGE and HELP message.

// theme_t.h
// FIXME: Black is interpreted as a non-color instead of checking theme.valid.
// FIXME: Change the struct's color-indexability into something enum based ?
// FIXME: Move the static methods into lib ?

// bitmap_t.h
// FIXME: Test calc_bitmap_size() or find a better reference on bitmaps.

// chessfont_t.h
// FIXME: Uniquely name FOTs and put them into the system's TEMP.

#define MIN_SQUARE_SIZE 8

int main(int argc, char ** argv)
{
	// lib::print_argv(argv);

	args_t args;
	if (!args.parse(argc, argv))
		return EXIT_FAILURE;
	args.print();

	theme_t theme;
	int const count = theme.parse(args.colors);
	lib::log("THEME: %d COLORS matched\n", count);
	lib::log("RGB: ");
	theme.print();
	// theme.map(lib::html2colorref);
	// lib::log("BGR: ");
	// theme.print();

	fen_t fen;
	if (!fen.parse(args.fen, args.map))
		return EXIT_FAILURE;

	if (args.square_size < MIN_SQUARE_SIZE)
	{
		lib::err("! The square size of %d is too small to be useful.\n",
			args.square_size);
		return EXIT_FAILURE;
	}

	memory_dc_t mdc;
	if (!mdc.create(args.image_size))
	{
		lib::err("! Was unable to create DC.\n");
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
		lib::err("! \"%s\" is neither a font path nor a font face name.\n", args.font);
		return EXIT_FAILURE;
	}
	if (font.path) lib::log("Chess font path is |%s|.\n", font.path);
	if (font.name) lib::log("Chess font facename is |%s|.\n", font.name);
	if (!font.create(args.square_size))
	{
		lib::err("! Was unable to instantiate font \"%s\".\n", font.name);
		return EXIT_FAILURE;
	}
	lib::log("Chess font handle is %p\n", (void *)font.handle);

	SelectObject(dc, font.handle);
	SelectObject(dc, GetStockObject(DC_BRUSH));
	SetBkMode(dc, TRANSPARENT);

	COLORREF const dk = theme.ds ? theme.ds : RGB(209,209,209);
	COLORREF const lt = theme.ls ? theme.ls : RGB(253,183,183);
	COLORREF const dp = theme.dp ? theme.dp : RGB(150,120,140);
	COLORREF const lp = theme.lp ? theme.lp : RGB(220,200,200);
	COLORREF const black = RGB(0, 0, 0);
	// COLORREF const white = RGB(255, 255, 255);
	COLORREF const lp_darker = lib::linterpol(lp, black, 0.3); // RGB(0, 0, 0);
	COLORREF const dp_darker = lib::linterpol(dp, black, 0.3); // RGB(255, 255, 255);

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(dc, grid.edge, grid.edge, m, lib::CenteringMode::MAX);

	size_t fen_i=0;
	for (size_t row=0; row<8; ++row)
		for (size_t col=0; col<8; ++col)
		{
			RECT r;
			grid.get_square_rect(r, col, row);
			SetDCBrushColor(dc, ((row + col) % 2 == 0) ? lt : dk);
			SelectObject(dc, GetStockObject(NULL_PEN));
			Rectangle(dc, r.left, r.top, r.right, r.bottom);

			int const x = r.left + m.cx;
			int const y = r.top + m.cy;

			if (fen.col[fen_i] == Side::Light)
			{
				lib::draw_char(dc, x, y, fen.to_dark(fen_i), lp);
				lib::draw_char(dc, x, y, fen.fen[fen_i], lp_darker);
			}
			else if (fen.col[fen_i] == Side::Dark)
			{
				lib::draw_char(dc, x, y, fen.fen[fen_i], dp);
				lib::draw_char(dc, x, y, fen.to_light(fen_i), dp_darker);
			}

			++fen_i;
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
