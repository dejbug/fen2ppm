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
#include "app/draw.h"
#include "app/gui.h"
#include "app/MainFrame.h"

// TODO: Start using fen2_t (which allows for different board geometries).

// FIXME: Add a USAGE and HELP message.
// FIXME:bitmap_t.h: Test calc_bitmap_size() or find a better reference on bitmaps.

// TODO: Add ROPs.
// TODO:theme_t: Move the static methods into lib ?

args_t args;
theme_t theme;
lib::chessfont_t font;
fen2_t fen;
grid_t grid;
memory_dc_t mdc;

bool init_board()
{
	int const count = theme.parse(args.colors);
	lib::log("THEME: %d COLORS matched\n", count);
	lib::log("RGB: ");
	theme.print();
	// theme.map(lib::html2colorref);
	// lib::log("BGR: ");
	// theme.print();

	if (!fen.parse(args.fen) && !fen.rectangle)
		return false;

	return true;
}

bool update_board()
{
	if (!font.create(args.font, args.square_size))
	{
		lib::err("! Was unable to resize font \"%s\" (with size %d)\n.",
			args.font, args.square_size);
		return false;
	}

	lib::log("font: %p\n", font.handle);

	grid.set_size(fen.geom.right, fen.geom.bottom);
	grid.set_square_size(args.square_size, args.gap);

	if (!mdc.create(grid.width(), grid.height()))
	{
		lib::err("! Was unable to create DC.\n");
		return false;
	}

	lib::err("args.map_length: %d\n", args.map_length);

	return true;
}

bool draw_board()
{
	if (args.map_length == 12)
		draw_board_themed(mdc.dc, fen, font.handle, args.map, theme, grid);
	else if (args.map_length == 26)
		draw_board_simple(mdc.dc, fen, font.handle, args.map, theme, grid);

	return true;
}

int main(int argc, char ** argv)
{
	// lib::print_argv(argv);

	if (!args.parse(argc, argv))
		return EXIT_FAILURE;
	args.print();

	if (!init_board() || !update_board() || !draw_board())
		return EXIT_FAILURE;

	if (args.out_path)
	{
		lib::log("* WRITING\n");
		FILE * outfile = fopen(args.out_path, "wb");
		dump_bitmap_data(mdc.dc, mdc.bmp, outfile);
		fclose(outfile);
	}
	// else
	// {
		// lib::log("* PRINTING\n");
		// dump_bitmap_data(mdc.dc, mdc.bmp, stdout);
	// }
	else
	{
		lib::log("* DRAWING (output to GUI).\n");

		WNDCLASSEX wc;
		app::init(wc);
		wc.lpfnWndProc = app::MainFrame;
		if (!app::install(wc)) return 1;

		HWND const h = app::create(wc);
		if (!h) return 2;

		SetWindowPos(h, NULL, 0, 0, 800, 600, SWP_NOZORDER|SWP_NOMOVE);
		app::center(h);
		UpdateWindow(h);
		ShowWindow(h, SW_SHOW);

		HACCEL const a = LoadAccelerators(wc.hInstance, "accel_main");

		MSG m;
		return app::run(m, h, a);
	}

	return EXIT_SUCCESS;
}
