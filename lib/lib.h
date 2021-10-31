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

} // lib
