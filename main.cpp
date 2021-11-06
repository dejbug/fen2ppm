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

// FIXME:bitmap_t.h: Test calc_bitmap_size() or find a better reference on bitmaps.

// TODO: Add differently sized boards (1x1 and up).
// TODO: Add ROPs.

// TODO:theme_t: Move the static methods into lib ?

#define FONT_INTERNAL_NAME "Leipzig"
#define FONT_INTERNAL_PATH "Leipzig.ttf"

char const * const map_internal_12 = "CAGEKIOMSQWU";
char const * const map_internal_26 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void draw_board_themed(HDC dc, fen_t & fen, chessfont_t & font, theme_t & theme, grid_t & grid, float border_factor=0.3)
{
	SelectObject(dc, font.handle);
	SetBkMode(dc, TRANSPARENT);
	SelectObject(dc, GetStockObject(NULL_PEN));
	SelectObject(dc, GetStockObject(DC_BRUSH));

	COLORREF const dk = theme.get(0, RGB(209,209,209));
	COLORREF const lt = theme.get(1, RGB(253,183,183));
	COLORREF const dp = theme.get(2, RGB(150,120,140));
	COLORREF const lp = theme.get(3, RGB(220,200,200));
	COLORREF const background = theme.get(4, RGB(255,255,255));
	COLORREF const black = RGB(0, 0, 0);
	// COLORREF const white = RGB(255, 255, 255);
	COLORREF const lp_darker = lib::linterpol(lp, black, border_factor);
	COLORREF const dp_darker = lib::linterpol(dp, black, border_factor);

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(dc, grid.edge, m, lib::CenteringMode::MAX);

	RECT r;
	grid.get_bounds(r);

	SetDCBrushColor(dc, background);
	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	size_t fen_i=0;
	for (size_t row=0; row<8; ++row)
		for (size_t col=0; col<8; ++col)
		{
			grid.get_square_rect(r, col, row);
			SetDCBrushColor(dc, ((row + col) % 2 == 0) ? lt : dk);
			Rectangle(dc, r.left, r.top, r.right + 1, r.bottom + 1);

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
}

void draw_board_simple(HDC dc, fen_t & fen, chessfont_t & font, theme_t & theme, grid_t & grid)
{
	SelectObject(dc, font.handle);
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

	size_t fen_i=0;
	for (size_t row=0; row<8; ++row)
		for (size_t col=0; col<8; ++col)
		{
			grid.get_square_rect(r, col, row);
			int const x = r.left + m.cx;
			int const y = r.top + m.cy;

			char const c = fen2font(fen.raw[fen_i], map_internal_26, col, row);
			lib::draw_char(dc, x, y, c, foreground);

			++fen_i;
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

	grid_t grid;
	grid.set_size(8, 8);
	grid.set_square_size(args.square_size, args.gap);

	memory_dc_t mdc;
	if (!mdc.create(grid.get_image_edge()))
	{
		lib::err("! Was unable to create DC.\n");
		return EXIT_FAILURE;
	}
	HDC const dc = mdc.dc;
	HBITMAP const bmp = mdc.bmp;

	chessfont_t font;
	if (!args.font || !*args.font || !strcmp(args.font, "0"))
	{
		args.font = FONT_INTERNAL_NAME;
		args.map = map_internal_12;
		font.create_from_resource(FONT_INTERNAL_PATH, args.font, args.square_size);
	}
	else
	{
		font.create_from_file_or_name(args.font, args.square_size);
	}

	fen_t fen;
	if (!fen.parse(args.fen, args.map))
		return EXIT_FAILURE;

	if (theme.valid)
		draw_board_themed(dc, fen, font, theme, grid);
	else
		draw_board_simple(dc, fen, font, theme, grid);

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
