#pragma once
#include <stdio.h>
#include <stdarg.h>

#include "out.h"

// lib::log and prosper.

#define COLORREF_ALPHA(c) ((UCHAR)( ((DWORD)(c)>>24u) & 0xFFu ))
#define COLORREF_RED(c)   ((UCHAR)( ((DWORD)(c))      & 0xFFu )) // GetRValue(c)
#define COLORREF_GREEN(c) ((UCHAR)( ((DWORD)(c)>>8u)  & 0xFFu )) // GetGValue(c)
#define COLORREF_BLUE(c)  ((UCHAR)( ((DWORD)(c)>>16u) & 0xFFu )) // GetBValue(c)

#define HTML_ALPHA(c) ((UCHAR)( ((DWORD)(c)>>24u) & 0xFFu ))
#define HTML_RED(c)   ((UCHAR)( ((DWORD)(c)>>16u) & 0xFFu ))
#define HTML_GREEN(c) ((UCHAR)( ((DWORD)(c)>>8u)  & 0xFFu ))
#define HTML_BLUE(c)  ((UCHAR)( ((DWORD)(c))      & 0xFFu ))

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

COLORREF html2colorref(COLORREF rgb) // HTML(=0xrrggbb) to COLORREF(=0xbbggrr)
{
	return RGB(HTML_RED(rgb), HTML_GREEN(rgb), HTML_BLUE(rgb));
}

COLORREF linterpol(COLORREF a, COLORREF b, float f)
{
	return RGB(
		(COLORREF_RED(b)-COLORREF_RED(a))*f+COLORREF_RED(a),
		(COLORREF_GREEN(b)-COLORREF_GREEN(a))*f+COLORREF_GREEN(a),
		(COLORREF_BLUE(b)-COLORREF_BLUE(a))*f+COLORREF_BLUE(a)
	);
}

char * make_temp_path(char const * name)
{
	DWORD const temp_path_len = GetTempPath(0, nullptr);
	if (!temp_path_len)
	{
		lib::err("! Was unable to determine TEMP path length.\n");
		return nullptr;
	}
	DWORD const name_len = strlen(name);
	DWORD const path_size = temp_path_len + name_len + 1;
	char * path = new char[path_size];
	if (!path)
	{
		lib::err("! Was unable to allocate memory for TEMP path.\n");
		return nullptr;
	}
	DWORD const path_len = GetTempPath(path_size, path);
	if (!path_len)
	{
		lib::err("! Was unable to determine TEMP path.\n");
		return nullptr;
	}
	strncat(path, name, name_len);
	// for (size_t i=0; i<path_size; ++i)
		// lib::err("%02x ", path[i]);
	// lib::err("\n");
	// for (size_t i=0; i<path_size; ++i)
		// lib::err("[%c]", path[i]);
	// lib::err("\n");
	return path;
}

} // lib
