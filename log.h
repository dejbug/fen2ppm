#pragma once
#include <stdarg.h>

void log(char const * format, ...)
{
#ifndef NDEBUG
	va_list list;
	va_start(list, format);
	vfprintf(stderr, format, list);
	va_end(list);
#endif
}
