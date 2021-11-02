#pragma once
#include <windows.h>
#include <stdarg.h>

namespace lib {

template<typename T> void log(T const * format, ...);
template<> void log(char const * format, ...)
{
#ifndef NDEBUG
	va_list list;
	va_start(list, format);
	vfprintf(stderr, format, list);
	va_end(list);
#endif
}

template<> void log(wchar_t const * format, ...)
{
#ifndef NDEBUG
	va_list list;
	va_start(list, format);
	vfwprintf(stderr, format, list);
	va_end(list);
#endif
}

template<typename T> void print_argv(T ** argv);
template<> void print_argv<>(char ** argv)
{
	if (argv)
		for (size_t i=0; argv[i]; ++i)
			lib::log<>("%3d %p |%s|\n", i, argv[i], argv[i]);
}

template<> void print_argv<>(wchar_t ** argv)
{
	if (argv)
		for (size_t i=0; argv[i]; ++i)
			lib::log<>(L"%3d %p |%S|\n", i, argv[i], argv[i]);
}

bool file_exists(char const * path)
{
	FILE * f = fopen(path, "rb");
	if (!f) return false;
	fclose(f);
	return true;
}

struct font_exists_struct_t
{
	char name[MAX_PATH] = {0};
	BOOL found = FALSE;
};

// int CALLBACK font_exists_callback(LOGFONT const * lf, TEXTMETRIC const * tm, DWORD ft, unsigned int l)
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

} // lib
