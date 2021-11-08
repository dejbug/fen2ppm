#pragma once
#include <algorithm> // max
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>

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

#define DEFAULT_COLORS NULL
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define DEFAULT_FONT "Leipzig.ttf"	// Built-in font's resource ID.
#define DEFAULT_MAP "pPnNbBrRqQkK"	// one-to-one (no-op) map.
#define DEFAULT_GAP 0
#define DEFAULT_SQUARE_SIZE 64

#define MIN_SQUARE_SIZE 8

#define FONT_INTERNAL_PATH "Leipzig.ttf"
#define FONT_INTERNAL_NAME "Leipzig"

#define MAP_INTERNAL_12 "CAGEKIOMSQWU"
#define MAP_INTERNAL_26 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define MAP_INTERNAL_COUNT 5

char const * const map_internal_strings[MAP_INTERNAL_COUNT] = {
	"pPnNbBrRqQkK", // one-to-one (no-op)
	"ABCDEFGHIJKL", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", // internal font themed/newspaper
	"opmnvbtrwqlk", // e.g. Merida, Kingdom, Leipzig.
	"opjhnbtrwqlk", // e.g. Alpha, Berlin.
};

int const map_internal_lengths[MAP_INTERNAL_COUNT] = {
	12, 12, 26, 12, 12,
};

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
	// FIXME: Maybe use tmpnam() for the name?
	// FIXME: Maybe just use GetTempFileName()?
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

bool make_temp_path(char * buffer, size_t buffer_size, char const * name)
{
	// FIXME: Maybe use tmpnam() for the name?
	// FIXME: Maybe just use GetTempFileName()?
	DWORD const temp_path_len = GetTempPath(0, nullptr);
	if (!temp_path_len)
	{
		lib::err("! Was unable to determine TEMP path length.\n");
		return false;
	}
	DWORD const name_len = strlen(name);
	DWORD const path_size = temp_path_len + name_len + 1;
	if (path_size > buffer_size)
	{
		lib::err("! TEMP path is going to be longer than the buffer.\n");
		return false;
	}
	DWORD const path_len = GetTempPath(buffer_size, buffer);
	if (!path_len)
	{
		lib::err("! Was unable to determine TEMP path.\n");
		return false;
	}
	strncat(buffer, name, name_len);
	// for (size_t i=0; i<path_size; ++i)
		// lib::err("%02x ", path[i]);
	// lib::err("\n");
	// for (size_t i=0; i<path_size; ++i)
		// lib::err("[%c]", path[i]);
	// lib::err("\n");
	return true;
}

template <typename T>
T * find_last_path_separator(T * path)
{
	if (!path || !path[0] || !path[1]) return nullptr;
	T * const bsl = strrchr(path, '\\');
	T * const sl = strrchr(path, '/');
	if (bsl && sl) return std::max(bsl, sl);
	if (bsl) return bsl;
	if (sl) return sl;
	return nullptr;
}

char const * notdir(char const * path)
{
	// I remember using GetFullPathName() in the past, but there were
	// 	some major quirks to get rid of edge-cases hence I'm reluctant.
	//  This works fine though.
	char const * lps = find_last_path_separator<char const>(path);
	return lps ? lps+1 : path;
}

struct split_path_t
{
	char * text = nullptr;
	char * path = nullptr;
	char * name = nullptr;
	char * ext = nullptr;

	~split_path_t()
	{
		free();
	}

	void free()
	{
		if (text) { delete[] text; text = nullptr; }
		path = name = ext = nullptr;
	}

	bool parse(char const * full)
	{
		free();

		if (!full) return false;

		text = strdup(full);
		if (!text) return false;
		path = text;

		name = lib::find_last_path_separator<char>(text);
		if (!name)
		{
			name = text;
			path = nullptr;
		}
		else
		{
			*name = '\0';
			++name;
		}

		ext = strrchr(name, '.');
		if (!ext) return true;

		*ext = '\0';
		++ext;

		return true;
	}

	void print()
	{
		lib::err("split_path_t{path=\"%s\", name=\"%s\", ext=\"%s\"}\n", path, name, ext);
	}
};

bool is_empty(char const * s)
{
	return (!s || !*s);
}

bool is_printable(char const * s)
{
	if (!s || !*s) return false;
	while (*s) if(!isspace(*s++)) return true;
	return false;
}

void parse(std::vector<std::string> & words, std::vector<std::string> & separators, char const * text, char const * alphabet)
{
	if (!text) return;

	if (!*text || !alphabet || !*alphabet)
	{
		words.push_back(text);
		return;
	}

	enum { None, InWord, InSeparator } mode = None;
	std::string wacc = "";
	std::string sacc = "";

	for (; *text; ++text)
	{
		if (mode == None)
		{
			if (strchr(alphabet, *text))
			{
				wacc += *text;
				mode = InWord;
			}
			else
			{
				words.push_back("");
				sacc += *text;
				mode = InSeparator;
			}
		}
		else if (mode == InWord)
		{
			if (strchr(alphabet, *text))
				wacc += *text;
			else
			{
				words.push_back(wacc);
				wacc = "";
				sacc += *text;
				mode = InSeparator;
			}
		}
		else if (mode == InSeparator)
		{
			if (!strchr(alphabet, *text))
				sacc += *text;
			else
			{
				separators.push_back(sacc);
				sacc = "";
				wacc += *text;
				mode = InWord;
			}
		}
	}
	if (!wacc.empty())
		words.push_back(wacc);
	if (!sacc.empty())
		separators.push_back(sacc);
}

} // lib
