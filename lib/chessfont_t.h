#pragma once
#include <algorithm> // std::min
#include "lib.h" // log, find_text_in_file
#include "gdi.h" // font_exists

#define FOT_PATH "FEN2PPM.TEMP.FOT"
#define FOT_NAME "FEN2PPM.FOT"


struct chessfont_t
{
	HFONT handle = nullptr;
	char name[MAX_PATH+1] = {0};
	char path[MAX_PATH+1] = {0};
	bool from_file = false;
	bool installed = false;
	char * fot_path = nullptr;

	~chessfont_t() { uninstall(); free(); }

	// operator HFONT() const { return handle; }

	void set_name(char const * text)
	{
		if (!text || !*text) name[0] = '\0';
		else strncpy_s(name, MAX_PATH, text, MAX_PATH);
		name[MAX_PATH] = '\0';
	}

	void set_path(char const * text)
	{
		if (!text || !*text) path[0] = '\0';
		else strncpy_s(path, MAX_PATH, text, MAX_PATH);
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

		if (lib::find_text_in_file(file, "FONTRES:"))
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
		uninstall();
		free();

		set_path(path);
		from_file = lib::file_exists(path);
		if (from_file)
		{
			if (fot_path) delete[] fot_path;
			fot_path = lib::make_temp_path(FOT_NAME);

			DeleteFile(fot_path);

			if (!CreateScalableFontResource(1,fot_path,path,0)) return false;
			if (0 == AddFontResource(path)) return false;

			if (!extract_name(fot_path, name, sizeof(name))) return false;
		}
		else
		{
			if (!lib::font_exists(path)) return false;

			set_path(nullptr);
			set_name(path); // TODO: set_name(notdir(path));

		}
		return installed = true;
	}

	void uninstall()
	{
		if (!installed) return;
		RemoveFontResource(path);
		if (fot_path) { DeleteFile(fot_path); delete[] fot_path; fot_path = nullptr; }
		set_path(NULL);
		installed = false;
		lib::log("* free: Chessfont (uninstall)\n");
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

	void free()
	{
		if (!handle) return;
		DeleteObject(handle), handle = 0;
		lib::log("* free: Chessfont (uncreate)\n");
	}
};
