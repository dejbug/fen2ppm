#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>

#include "lib/lib.h"
#include "lib/gdi.h"
#include "lib/argument_parser/argument_parser_t.h"

#include "fen.h"
#include "bitmap.h"
#include "grid_t.h"
#include "chessfont_t.h"

char const fen_0[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int main(int argc, char ** argv)
{
	lib::print_argv(argv);

	lib::argument_parser_t args("c:f:o:s:t:vh");
	if (!args.parse(argc, argv))
	{
		if (args.unknown)
			lib::log("%s: error: unknown option '%c' (\\x%x)\n", argv[0], args.unknown, args.unknown);
		else if (args.missing)
			lib::log("%s: error: missing argument for option '%c' (\\x%x)\n", argv[0], args.missing, args.missing);
		return EXIT_FAILURE;
	}

	args.print();

	size_t const square_size = atoi(args.opt('s'));
	size_t const gap = 0;
	size_t const image_size = (square_size + gap) * 8;

	lib::log("fen raw: |%s|\n", args.arg(0));
	char buf[64+1] = {0};
	fen_unpack(buf, args.arg(0));
	lib::log("fen unp: |%s|\n", buf);
	fen_translate(buf, args.opt('t'));
	lib::log("fen map: |%s|\n", buf);

	context_t ct;
	if (!ct.init()) return 1;
	lib::log("Desktop HWND: %p / DC: %p\n", (void *)ct.dh, (void *)ct.dc);
	HDC mdc = CreateCompatibleDC(ct.dc);
	lib::log("Memory HDC: %p\n", (void *)mdc);
	// ct.free();
	HBITMAP bmp = CreateCompatibleBitmap(ct.dc, image_size, image_size);
	lib::log("Memory HBITMAP: %p\n", (void *)bmp);
	SelectObject(mdc, bmp);

	grid_t grid;
	grid.set_size(8, 8);
	grid.set_square_size(square_size, gap);

	chessfont_t font;
	font.install(args.opt('f'));
	lib::log("Chess font facename is |%s|.\n", font.name);
	font.create(square_size);
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

	if (args.opt('o'))
	{
		lib::log("* WRITING\n");
		FILE * outfile = fopen(args.opt('o'), "wb");
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
