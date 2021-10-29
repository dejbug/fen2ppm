#pragma once
#include <algorithm> // std::min


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

	static size_t read_zstring_be(FILE * file, char * buf, size_t bsize)
	{
		DWORD const offset = ftell(file);
		fseek(file, 0, SEEK_END);
		DWORD const fsize = ftell(file);
		fseek(file, offset, SEEK_SET);

		if (offset >= fsize) return 0;

		DWORD len = fgetc(file) << 24;
		len |= (fgetc(file) << 16);
		len |= (fgetc(file) << 8);
		len |= (fgetc(file) << 0);

		if (len > (fsize-offset)) return 0;

		return fread(buf, sizeof(char), std::min(bsize, (size_t)len), file);
	}

	static bool extract_name(const char * fotpath, char * name, size_t size)
	{
		if (extract_name_by_offset(fotpath, name, size)) return true;
		if (extract_name_by_search(fotpath, name, size)) return true;
		return false;
	}

	static bool extract_name_by_offset(const char * fotpath, char * name, size_t size)
	{
		if (!fotpath || !*fotpath) return false;
		if (!name) return false;
		if (!size) return true;

		memset(name, 0, size);

		FILE * file = fopen(fotpath, "rb");
		fseek(file, 0, SEEK_END);
		DWORD const file_size = ftell(file);
		fseek(file, 13, SEEK_SET);

		// Seems this offset is little-endian but the others big-endian.
		// TTF is an Apple technology but isn't FOT from Microsoft? Maybe
		// they simply copied the strings over.
		WORD offset = 0;
		fread((char *)&offset, sizeof(WORD), 1, file);
		if (offset >= file_size) { fclose(file); return false; }

		fseek(file, offset, SEEK_SET);
		// The first string is the (double-zero terminated) the file name.
		if (!read_zstring_be(file, name, size)) return false;
		fseek(file, 1, SEEK_CUR);
		// This is what we're looking for.
		size_t const len = read_zstring_be(file, name, size);
		if (!len) return false;

		fclose(file);

		if (strncmp(name, "FONTRES:", 8)) return false;
		if (strncpy_s(name, size, name+8, len-8)) return false;

		return true;
	}

	static bool extract_name_by_search(const char * fotpath, char * name, size_t size)
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

		// This seems to be causing trouble. Perhaps we need to copy
		// the font file into the windows\font folder first? We don't need this.
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
