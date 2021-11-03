#pragma once
#include <stdio.h>
#include <stdarg.h>

// lib::log and prosper.

namespace lib {

bool file_exists(char const * path)
{
	FILE * f = fopen(path, "rb");
	if (!f) return false;
	fclose(f);
	return true;
}

bool find_text_in_file(FILE * file, char const * text)
{
	if (!file) return false;
	if (!text || !*text) return false;

	int const len = strlen(text);
	int pos = 0;
	int c;
	size_t const mark = ftell(file);
	size_t start = mark;

	while (EOF != (c = fgetc(file)))
	{
		if (c == text[pos]) ++pos;

		else if (pos)
		{
			fseek(file, ++start, SEEK_SET);
			pos = 0;
		}

		if (pos >= len) return true;
	}

	fseek(file, mark, SEEK_SET);
	return false;
}

#define BGR_RED(c)  ((c)&0xFF)
#define BGR_GREEN() (((c)>>8)&0xFF)
#define BGR_BLUE()  (((c)>>16)&0xFF)

COLORREF bgr2rgb(COLORREF bgr)
{
	COLORREF rgb = bgr & 0xFF;
	rgb <<= 8; bgr >>= 8;
	rgb += bgr & 0xFF;
	rgb <<= 8; bgr >>= 8;
	rgb += bgr & 0xFF;
	return rgb;
}

} // lib
