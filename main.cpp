#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>

#include "log.h"
#include "fen.h"
#include "bitmap.h"

#include "lib/argument_parser_t.h"
#include "grid_t.h"
#include "chessfont_t.h"

typedef enum {MAX=0, AVG} CenteringMode;
bool calc_text_centering_margins(HDC dc, int cx, int cy, SIZE & m, int mode=CenteringMode::MAX)
{
	TEXTMETRIC tm;
	if (!GetTextMetrics(dc, &tm)) return false;
	LONG const width = mode == AVG ? tm.tmAveCharWidth : tm.tmMaxCharWidth;
	m.cx = ((cx - width) >> 1);
	m.cy = ((cy - tm.tmHeight) >> 1);
	return true;
}

char const fen_0[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

char const map_Merida[] = "opmnvbtrwqlk";
char const map_Alpha[] = "opjhnbtrwqlk";

void print_argv(char ** argv)
{
	if (argv)
		for (size_t i=0; argv[i]; ++i)
			log("%3d %p |%s|\n", i, argv[i], argv[i]);
}

int main(int argc, char ** argv)
{
	// print_argv(argv);

	lib::argument_parser_t args(":f:t:o:s:vh");
	if (!args.parse(argc, argv))
	{
		if (args.unknown)
			log("%s: error: unknown option '%c' (\\x%x)\n", argv[0], args.unknown, args.unknown);
		else if (args.missing)
			log("%s: error: missing argument for option '%c' (\\x%x)\n", argv[0], args.missing, args.missing);
		return EXIT_FAILURE;
	}

	args.print();

	size_t const square_size = atoi(args.opt('s'));
	size_t const gap = 0;
	size_t const image_size = (square_size + gap) * 8;

	log("fen raw: |%s|\n", args.arg(0));
	char buf[64+1] = {0};
	fen_unpack(buf, args.arg(0));
	log("fen unp: |%s|\n", buf);
	fen_translate(buf, args.opt('t'));
	log("fen map: |%s|\n", buf);

	context_t ct;
	if (!ct.init()) return 1;
	log("Desktop HWND: %p / DC: %p\n", (void *)ct.dh, (void *)ct.dc);
	HDC mdc = CreateCompatibleDC(ct.dc);
	log("Memory HDC: %p\n", (void *)mdc);
	// ct.free();
	HBITMAP bmp = CreateCompatibleBitmap(ct.dc, image_size, image_size);
	log("Memory HBITMAP: %p\n", (void *)bmp);
	SelectObject(mdc, bmp);

	grid_t grid;
	grid.set_size(8, 8);
	grid.set_square_size(square_size, gap);

	chessfont_t font;
	font.install(args.opt('f'));
	log("Chess font facename is |%s|.\n", font.name);
	font.create(square_size);
	log("Chess font handle is %p\n", (void *)font.handle);

	SelectObject(mdc, font.handle);
	SelectObject(mdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(mdc, RGB(209,209,209));
	SetBkMode(mdc, TRANSPARENT);

	COLORREF const dk = RGB(283,183,183);
	COLORREF const lt = RGB(209,209,209);

	// HPEN pen = CreatePen(PS_SOLID, gap >> 1, RGB(220,100,100));
	// SelectObject(mdc, pen);

	SIZE m = {0, 0};
	calc_text_centering_margins(mdc, grid.edge, grid.edge, m, CenteringMode::MAX);

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
		log("* WRITING\n");
		FILE * outfile = fopen(args.opt('o'), "wb");
		dump_bitmap_data(mdc, bmp, outfile);
		fclose(outfile);
	}
	else
	{
		log("* PRINTING\n");
		dump_bitmap_data(mdc, bmp, stdout);
	}

	font.free();
	font.uninstall();
	if (bmp) DeleteObject(bmp);
	if (mdc) DeleteDC(mdc);

	return EXIT_SUCCESS;
}
