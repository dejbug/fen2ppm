#pragma once
#include <vector>
#include <assert.h>
#include <stdio.h>

#include "out.h" // print_argv

namespace lib {

struct shlex_t
{
	LPSTR text = NULL;
	size_t length = 0;
	std::vector<size_t> offsets;

	void free()
	{
		if (text) { delete[] text; text = NULL; }
		length = 0;
		offsets.clear();
	}

	bool parse(LPCSTR s)
	{
		return true;
	}

	bool parse(LPCWSTR s)
	{
		free();

		length = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
		// printf("text needs %d\n", length);
		text = new char[length + 1];
		if (!text) return false;
		text[length] = '\0';
		// memset(text, 0, length + 1);
		if (!length) return true;

		int count = 0;
		LPWSTR * const argw = CommandLineToArgvW(s, &count);
		if (!argw || count<=0) { free(); return false; }
		// printf("count: %d\n", count);
		// lib::print_argv(argw);

		size_t off = 0;
		offsets.push_back(off);
		for (int i=0; i<count; ++i)
		{
			size_t const rest = length - off;
			int const len = WideCharToMultiByte(CP_ACP, 0, argw[i], -1, text+off, rest, NULL, NULL);
			off += len;
			offsets.push_back(off);
		}
		// printf("length: %d\n", length);
		// printf("off: %d\n", off);
		assert(off <= length);
		text[off] = '\0';
		text[length] = '\0';
		return true;
	}

	size_t count() const
	{
		return offsets.size() - 1;
	}

	LPSTR get(size_t index) const
	{
		if (index > offsets.size()) return NULL;
		return text + offsets[index];
	}

	void print() const
	{
#ifndef NDEBUG
		printf("shlex_t{text=\"");
		for (size_t off : offsets)
			printf("%s\\0", text+off);
		printf("\", length=%d, count()=%d}\n", length, count());
#endif
	}
};

} // lib
