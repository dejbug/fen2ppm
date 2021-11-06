#pragma once
#include "../lib/out.h"

struct theme_t
{
	static size_t const count = 5;
	size_t valid = 0;
	COLORREF colors[count] = {0};

	static int xtoi(char c)
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		return -1;
	}

	static int match_color(char const * text, COLORREF & col)
	{
		col = 0;

		if (!text || !*text) return -1;

		size_t cc = 0;

		for (; *text; ++text)
		{
			int const dec = xtoi(*text);
			// lib::log("* hex='%c' dec=%d\n", *text, dec);
			if (dec < 0) break;
			else
			{
				col = (col << 4) + dec;
				++cc;
			}
		}
		if (cc == 3 || cc == 6 || cc == 8)
			return cc;
		return -cc;
	}

	static COLORREF stretch_color(COLORREF col)
	{
		return ((col&0xF00)<< 12) | ((col&0xF00)<< 8) | ((col&0x0F0)<<8) |
			((col&0x0F0)<<4) | ((col&0x00F)<<4) | (col&0x00F);
	}

	void zero()
	{
		for (size_t i=0; i<count; ++i)
			colors[i] = 0;
	}

	int parse(char const * text)
	{
		zero();
		if (!text || !*text) return 0;

		// COLORREF * colors[] = {&ds, &ls, &dp, &lp};
		// int const count = sizeof(colors) / sizeof(*colors);

		valid = 0;
		for (; valid<count; ++valid)
		{
			if (text[0] == '#') ++text;
			int const count = match_color(text, colors[valid]);
			// lib::log("* MATCH%scol=%08X\n", count > 0 ? "ED: " : " FAILED: ",
				// colors[valid]);
			if (count < 0) break;
			text += count + 1;
			if (count == 3) colors[valid] = stretch_color(colors[valid]);
		}
		return valid;
	}

	void map(COLORREF (*f)(COLORREF))
	{
		for (size_t i=0; i<valid; ++i)
			colors[i] = f(colors[i]);
	}

	bool has(size_t i) const
	{
		return i < valid;
	}

	COLORREF get(size_t i, COLORREF def=-1) const
	{
		if (i >= valid) return def;
		return colors[i];
	}

	void print() const
	{
		lib::err("THEME:");
		for (size_t i=0; i<count; ++i)
		{
			if (i < valid) lib::err(" %06x", colors[i]);
			else lib::err(" ------");
		}
		lib::err("\n");
	}
};
