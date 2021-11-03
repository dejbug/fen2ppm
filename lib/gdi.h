#pragma once
#include <windows.h>
// Needs: -lgdi32

namespace lib {

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
	// printf("FONT |%s| was %s.\n", name, fe.found ? "found" : "NOT found");

	ReleaseDC(h, dc);
	return fe.found;
}

} // lib
