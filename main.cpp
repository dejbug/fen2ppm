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

// TODO: Start using fen2_t (which allows for different board geometries).

// FIXME: Add a USAGE and HELP message.
// FIXME:bitmap_t.h: Test calc_bitmap_size() or find a better reference on bitmaps.

// TODO: Add ROPs.
// TODO:theme_t: Move the static methods into lib ?

void draw_board_themed(HDC dc, fen2_t & fen, HFONT font, char const * map, theme_t & theme, grid_t & grid, float border_factor=0.3)
{
	lib::log("mode: THEMED\n");

	SelectObject(dc, font);
	SetBkMode(dc, TRANSPARENT);
	SelectObject(dc, GetStockObject(NULL_PEN));
	SelectObject(dc, GetStockObject(DC_BRUSH));

	COLORREF const background = RGB(255,255,255);
	// COLORREF const background = theme.get(0, RGB(255,255,255));
	COLORREF const ds = theme.get(0, RGB(209,209,209));	// dark square
	COLORREF const ls = theme.get(1, RGB(253,183,183));	// light square
	COLORREF const dp = theme.get(2, RGB(150,120,140));	// dark piece
	COLORREF const lp = theme.get(3, RGB(220,200,200));	// light piece
	COLORREF const black = RGB(0, 0, 0);
	COLORREF const white = RGB(255, 255, 255);
	COLORREF const dp_lighter = lib::linterpol(dp, white, border_factor);
	COLORREF const lp_darker = lib::linterpol(lp, black, border_factor);
	COLORREF const db = theme.get(4, dp_lighter);	// dark (piece) border
	COLORREF const lb = theme.get(5, lp_darker);	// light (piece) border

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(dc, grid.edge, m, lib::CenteringMode::MAX);

	RECT r;
	grid.get_bounds(r);

	SetDCBrushColor(dc, background);
	Rectangle(dc, r.left, r.top, r.right + 1, r.bottom + 1);

	size_t i = 0;
	for (size_t row=0; row<grid.rows; ++row)
		for (size_t col=0; col<grid.cols; ++col)
		{
			grid.get_square_rect(r, col, row);
			SetDCBrushColor(dc, ((row + col) % 2 == 0) ? ls : ds);
			Rectangle(dc, r.left, r.top, r.right + 1, r.bottom + 1);

			int const x = r.left + m.cx;
			int const y = r.top + m.cy;

			if (fen.side(i) == Side::Light)
			{
				lib::draw_char(dc, x, y, fen.get_inv(i, map), lp);
				lib::draw_char(dc, x, y, fen.get(i, map), lb);
			}
			else if (fen.side(i) == Side::Dark)
			{
				lib::draw_char(dc, x, y, fen.get(i, map), dp);
				lib::draw_char(dc, x, y, fen.get_inv(i, map), db);
			}

			++i;
		}
}

void draw_board_simple(HDC dc, fen2_t & fen, HFONT font, char const * map, theme_t & theme, grid_t & grid)
{
	lib::log("mode: NEWSPAPER\n");

	SelectObject(dc, font);
	SetBkMode(dc, TRANSPARENT);
	SelectObject(dc, GetStockObject(NULL_PEN));
	SelectObject(dc, GetStockObject(DC_BRUSH));

	COLORREF const background = theme.get(0, RGB(255, 255, 255));
	COLORREF const foreground = theme.get(1, RGB(0, 0, 0));

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(dc, grid.edge, m, lib::CenteringMode::MAX);


	RECT r;
	grid.get_bounds(r);

	SetDCBrushColor(dc, background);
	Rectangle(dc, r.left, r.top, r.right + 1, r.bottom + 1);

	size_t i = 0;
	for (size_t row=0; row<grid.rows; ++row)
		for (size_t col=0; col<grid.cols; ++col)
		{
			grid.get_square_rect(r, col, row);
			int const x = r.left + m.cx;
			int const y = r.top + m.cy;

			char const c = fen.get(i, map, col, row);
			lib::draw_char(dc, x, y, c, foreground);

			++i;
		}
}

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

	lib::chessfont_t font;
	if (!font.create(args.font, args.square_size))
	{
		lib::err("! Was unable to create font \"%s\" (with size %d).",
			args.font, args.square_size);
		return EXIT_FAILURE;
	}

	lib::log("font: %p\n", font.handle);

	fen2_t fen;
	if (!fen.parse(args.fen) && !fen.rectangle)
		return EXIT_FAILURE;

	grid_t grid;
	grid.set_size(fen.geom.right, fen.geom.bottom);
	grid.set_square_size(args.square_size, args.gap);

	memory_dc_t mdc;
	if (!mdc.create(grid.width(), grid.height()))
	{
		lib::err("! Was unable to create DC.\n");
		return EXIT_FAILURE;
	}
	HDC const dc = mdc.dc;
	HBITMAP const bmp = mdc.bmp;

	lib::err("args.map_length: %d\n", args.map_length);

	if (args.map_length == 12)
		draw_board_themed(dc, fen, font.handle, args.map, theme, grid);
	else if (args.map_length == 26)
		draw_board_simple(dc, fen, font.handle, args.map, theme, grid);

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
