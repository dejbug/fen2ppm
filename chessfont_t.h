#pragma once
#include <algorithm>


bool find_text_in_file(FILE * file, char const * text);


struct chessfont_t
{
	HFONT handle = nullptr;
	char name[MAX_PATH+1];
	char path[MAX_PATH+1];

	// operator HFONT() const { return handle; }

	void set_name(char const * text)
	{
		strncpy_s(name, MAX_PATH, text, MAX_PATH);
		name[MAX_PATH] = '\0';
	}

	void set_path(char const * text)
	{
		strncpy_s(path, MAX_PATH, text, MAX_PATH);
		path[MAX_PATH] = '\0';
	}

	static bool extract_name(const char * fotpath, char * name, size_t size)
	{
		if (!fotpath || !*fotpath) return false;
		if (!name) return false;
		if (!size) return true;

		bool result = false;
		FILE * file = fopen(fotpath, "rb");

		LONG const mark = ftell(file);
		fseek(file, 0, SEEK_END);
		LONG const rest = ftell(file) - mark;
		fseek(file, mark, SEEK_SET);

		if (find_text_in_file(file, "FONTRES:"))
		{
			result = true;
			fgets(name, std::min((LONG)MAX_PATH, rest), file);
			name[MAX_PATH] = '\0';
		}
		fclose(file);

		return result;
	}

	bool install(char const * path, bool notify=false)
	{
		set_path(path);
		CreateScalableFontResource(1,"TEMP.FOT",path,0);
		if (0 == AddFontResource(path)) return false;

		// This seems to be causing trouble. But we don't need this.
		// if (notify) SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

		extract_name("TEMP.FOT", name, sizeof(name));
		return true;
	}

	bool uninstall()
	{
		RemoveFontResource(path);
		DeleteFile("TEMP.FOT");
		set_path(0);
		return true;
	}

	bool create(int size)
	{
		return create(name, size);
	}

	bool create(char const * name, int size)
	{
		if (!name || !*name || size < 1) return false;

		set_name(name);

		if (handle) DeleteObject(handle);
		handle = CreateFont(size,0,0,0,0,0,0,0,
			// SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY, FF_DONTCARE, name);
		return !!handle;
	}

	bool resize(int size)
	{
		free();
		return create(name, size);
	}

	bool free()
	{
		if (handle) DeleteObject(handle), handle = 0;
		return true;
	}
};


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
