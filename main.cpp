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
// FIXME: COLORREF is in 0x00bbggrr format but user will want 0x00rrggbb.
// FIXME: The 3-digit colors need to be stretched.
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
	theme.map(lib::bgr2rgb);
	theme.print();

	lib::log("fen raw: |%s|\n", args.fen);
	char raw[RAW_LEN+1] = {0};
	char raw_col[RAW_LEN+1] = {0};
	if (!args.fen || !*args.fen || !fen_unpack(raw, raw_col, args.fen))
	{
		lib::err("! Was unable to unpack FEN: \"%s\"\n", args.fen);
		return EXIT_FAILURE;
	}
	lib::log("fen unp: |%s|\n", raw);
	if (!args.map || !fen_translate(raw, args.map))
	{
		lib::err("! Was unable to translate FEN \"%s\" with map: \"%s\"\n",
			args.fen, args.map);
		return EXIT_FAILURE;
	}
	lib::log("fen map: |%s|\n", raw);

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
	COLORREF const dp = theme.dp ? theme.dp : RGB(220,160,90);
	COLORREF const lp = theme.lp ? theme.lp : RGB(220,160,90);

	// HPEN pen = CreatePen(PS_SOLID, gap >> 1, RGB(220,100,100));
	// SelectObject(dc, pen);

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(dc, grid.edge, grid.edge, m, lib::CenteringMode::MAX);

	size_t fen_i=0;
	char fen_buf[2] = {'\0', '\0'};

	for (size_t row=0; row<8; ++row)
		for (size_t col=0; col<8; ++col)
		{
			fen_buf[0] = raw[fen_i];

			RECT r;
			grid.get_square_rect(r, col, row);
			SetDCBrushColor(dc, ((row + col) % 2 == 0) ? lt : dk);
			SelectObject(dc, GetStockObject(NULL_PEN));
			Rectangle(dc, r.left, r.top, r.right, r.bottom);

			SetTextColor(dc, raw_col[fen_i] == 'w' ? lp : dp);
			TextOut(dc, r.left + m.cx, r.top + m.cy, fen_buf, 1);

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
