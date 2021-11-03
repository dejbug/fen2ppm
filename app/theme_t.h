#pragma once
#include "../lib/out.h"

struct theme_t
{
	COLORREF ds = 0, ls = 0, dp = 0, lp = 0;
	int valid = 0;
	static int const colors_count = 4;
	COLORREF * const colors[colors_count] = {&ds, &ls, &dp, &lp};

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

		size_t count = 0;

		for (; *text; ++text)
		{
			int const dec = xtoi(*text);
			// lib::log("* hex='%c' dec=%d\n", *text, dec);
			if (dec < 0) break;
			else
			{
				col = (col << 4) + dec;
				++count;
			}
		}
		if (count == 3 || count == 6 || count == 8)
			return count;
		return -count;
	}

	int parse(char const * text)
	{
		if (!text || !*text) return 0;

		// COLORREF * colors[] = {&ds, &ls, &dp, &lp};
		// int const colors_count = sizeof(colors) / sizeof(*colors);

		valid = 0;
		for (; valid<colors_count; ++valid)
		{
			if (text[0] == '#') ++text;
			int const count = match_color(text, *colors[valid]);
			// lib::log("* MATCH%scol=%08X\n", count > 0 ? "ED: " : " FAILED: ", *colors[valid]);
			if (count > 0)
				text += count + 1;
			else
				break;
		}
		return valid;
	}

	void map(COLORREF (*f)(COLORREF))
	{
		for (int i=0; i<valid; ++i)
			*colors[i] = f(*colors[i]);
	}

	void print() const
	{
		lib::err("THEME: ds=%08x, ls=%08x, dp=%08x, lp=%08x\n", ds, ls, dp, lp);
	}
};
