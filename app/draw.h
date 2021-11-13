#pragma once
#include <windows.h>

#include "../lib/lib.h"
#include "../lib/out.h"

#include "../lib/fen.h"
#include "../lib/grid_t.h"
#include "../lib/gdi.h"

#include "theme_t.h"


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
