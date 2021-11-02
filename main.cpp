#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>

#include "lib/lib.h"
#include "lib/gdi.h"
#include "app/args_t.h"

#include "fen.h"
#include "bitmap.h"
#include "grid_t.h"
#include "chessfont_t.h"

char const fen_0[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

struct memory_dc_t
{
	HDC mdc = NULL;
	HBITMAP bmp = NULL;

	~memory_dc_t()
	{
		if (bmp)
		{
			if (mdc) SelectObject(mdc, (HBITMAP)NULL);
			DeleteObject(bmp);
			bmp = NULL;
		}
		if (mdc)
		{
			DeleteDC(mdc);
			mdc = NULL;
		}
	}
};

int main(int argc, char ** argv)
{
	lib::print_argv(argv);

	args_t args;
	if (!parse_args(args, argc, argv))
		return EXIT_FAILURE;
	print_args(args);

	lib::log("fen raw: |%s|\n", args.fen);
	char buf[64+1] = {0};
	fen_unpack(buf, args.fen);
	lib::log("fen unp: |%s|\n", buf);
	fen_translate(buf, args.map);
	lib::log("fen map: |%s|\n", buf);

	context_t ct;
	if (!ct.init()) return 1;
	lib::log("Desktop HWND: %p / DC: %p\n", (void *)ct.dh, (void *)ct.dc);
	HDC mdc = CreateCompatibleDC(ct.dc);
	lib::log("Memory HDC: %p\n", (void *)mdc);
	// ct.free();
	HBITMAP bmp = CreateCompatibleBitmap(ct.dc, args.image_size, args.image_size);
	lib::log("Memory HBITMAP: %p\n", (void *)bmp);
	SelectObject(mdc, bmp);

	// TODO: Replace the previous block with a single object.
	memory_dc_t dc_auto_deleter;
	dc_auto_deleter.mdc = mdc;
	dc_auto_deleter.bmp = bmp;

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

	SelectObject(mdc, font.handle);
	SelectObject(mdc, GetStockObject(DC_BRUSH));
	SetBkMode(mdc, TRANSPARENT);

	COLORREF const dk = RGB(209,209,209);
	COLORREF const lt = RGB(253,183,183);

	// HPEN pen = CreatePen(PS_SOLID, gap >> 1, RGB(220,100,100));
	// SelectObject(mdc, pen);

	SIZE m = {0, 0};
	lib::calc_text_centering_margins(mdc, grid.edge, grid.edge, m, lib::CenteringMode::MAX);

	size_t fen_i=0;
	char fen_buf[2] = {'\0', '\0'};

	for (size_t row=0; row<8; ++row)
		for (size_t col=0; col<8; ++col)
		{
			fen_buf[0] = buf[fen_i++];

			RECT r;
			grid.get_square_rect(r, col, row);
			SetDCBrushColor(mdc, ((row + col) % 2 == 0) ? lt : dk);
			SelectObject(mdc, GetStockObject(NULL_PEN));
			Rectangle(mdc, r.left, r.top, r.right, r.bottom);

			SetTextColor(mdc, RGB(220,160,90));
			TextOut(mdc, r.left + m.cx, r.top + m.cy, fen_buf, 1);
		}

	if (args.out_path)
	{
		lib::log("* WRITING\n");
		FILE * outfile = fopen(args.out_path, "wb");
		dump_bitmap_data(mdc, bmp, outfile);
		fclose(outfile);
	}
	else
	{
		lib::log("* PRINTING\n");
		dump_bitmap_data(mdc, bmp, stdout);
	}

	font.free();
	font.uninstall();
	if (bmp) DeleteObject(bmp);
	if (mdc) DeleteDC(mdc);

	return EXIT_SUCCESS;
}
