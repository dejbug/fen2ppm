#pragma once
#include <windows.h>
#include "lib.h"
// Needs: -lgdi32

namespace lib {

typedef enum {MAX=0, AVG} CenteringMode;

bool calc_text_centering_margins(HDC dc, int edge, SIZE & m, int mode=CenteringMode::MAX)
{
	TEXTMETRIC tm;
	if (!GetTextMetrics(dc, &tm)) return false;
	LONG const width = mode == AVG ? tm.tmAveCharWidth : tm.tmMaxCharWidth;
	m.cx = ((edge - width) >> 1);
	m.cy = ((edge - tm.tmHeight) >> 1);
	return true;
}

bool calc_text_centering_margins(HDC dc, int edge, SIZE & m, char const * text, size_t len)
{
	SIZE s;
	GetTextExtentPoint32(dc, text, len, &s);
	m.cx = ((edge - s.cx) >> 1);
	m.cy = ((edge - s.cy) >> 1);
	return true;
}

struct font_exists_struct_t
{
	char name[MAX_PATH] = {0};
	BOOL found = FALSE;
};

int CALLBACK font_exists_callback(ENUMLOGFONT FAR * elf, NEWTEXTMETRIC FAR * ntm, int ft, LPARAM lp)
{
	font_exists_struct_t & fe = *(font_exists_struct_t *)lp;
	// printf("name: |%s|\n", elf->elfFullName);
	if (!strcmp((char const *)elf->elfFullName, fe.name))
	{
		fe.found = TRUE;
		return 0;
	}
	return 1;
}

bool font_exists(char const * name)
{
	HWND h = GetDesktopWindow();
	HDC dc = GetDC(h);
	// printf("HDC :%p\n", (void *)dc);
	font_exists_struct_t fe;
	strncpy(fe.name, name, MAX_PATH);

	EnumFontFamilies(dc, NULL, (FONTENUMPROCA)font_exists_callback, (LPARAM)&fe);
	printf("FONT %s: |%s|.\n", fe.found ? "found" : "NOT found", name);

	ReleaseDC(h, dc);
	return fe.found;
}

void draw_char(HDC dc, int x, int y, char c, COLORREF col)
{
	// SelectObject(dc, GetStockObject(DC_BRUSH));
	// SetBkMode(dc, TRANSPARENT);

	char buf[2] {c, '\0'};
	SetTextColor(dc, col);
	TextOut(dc, x, y, buf, 1);
}

} // lib
