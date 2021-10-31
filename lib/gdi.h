// Needs: -lgdi32
#pragma once
#include <windows.h>

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

} // lib
