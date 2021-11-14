#pragma once
#include <algorithm> // min
#include <memory> // unique_ptr
#include "lib.h" // log, find_text_in_file
#include "gdi.h" // font_exists

#define FOT_PATH "FEN2PPM.TEMP.FOT"
#define FOT_NAME "FEN2PPM.FOT"

namespace lib {

size_t read_zstring_be(FILE * file, char * buf, size_t bsize)
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

bool extract_name_by_offset(const char * fotpath, char * name, size_t size)
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

bool extract_name_by_search(const char * fotpath, char * name, size_t size)
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
		fgets(name, std::min((LONG)size, rest), file);
		name[size] = '\0';
	}

	fclose(file);
	return result;
}

bool extract_name(const char * fotpath, char * name, size_t size)
{
	if (extract_name_by_offset(fotpath, name, size)) return true;
	if (extract_name_by_search(fotpath, name, size)) return true;
	return false;
}

typedef enum { All=0, Enumerable } CreateFontMode;

HFONT create_font_from_name(char const * name, int size, int mode=CreateFontMode::All)
{
	lib::log("* Loading from name |%s|.\n", name);

	if (!name || !*name) return nullptr;
	if (mode == CreateFontMode::Enumerable && !lib::font_exists(name)) return nullptr;
	return CreateFont(size,0,0,0,0,0,0,0,
		// SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FF_DONTCARE, name);
}

bool install_font_from_resource(char (&name)[MAX_PATH+1], char const * path)
{
	lib::split_path_t sp;

	if (!path || !*path || !sp.parse(path))
	{
		path = FONT_INTERNAL_PATH;
		strncpy(name, FONT_INTERNAL_NAME, sizeof(name));
	}
	else
	{
		strncpy(name, sp.name, sizeof(name));
	}
	sp.print();

	lib::log("* Loading from resource |%s| (%s).\n", path, name);

	if (!*name)
	{
		lib::err("! Couldn't derive font name from resource \"%s\".\n", path);
		return false;
	}

	HINSTANCE const instance = GetModuleHandle(NULL);
	HRSRC const res_find = FindResource(instance, path, MAKEINTRESOURCE(4711));
	if (!res_find)
	{
		lib::err("! Was unable to find internal chessfont.\n");
		return false;
	}
	DWORD const res_size = SizeofResource(instance, res_find);
	lib::log("res_size: %d\n", res_size);
	HGLOBAL const res_load = LoadResource(instance, res_find);
	if (!res_load)
	{
		lib::err("! Was unable to load internal chessfont resource.\n");
		return false;
	}
	LPVOID res_lock = LockResource(res_load);
	if (!res_lock)
	{
		lib::err("! Was unable to lock internal chessfont resource.\n");
		return false;
	}
	DWORD res_loaded_count = 0;
	HANDLE const res_add = AddFontMemResourceEx(res_lock, res_size, 0, &res_loaded_count);
	if (!res_add)
	{
		lib::err("! Was unable to add internal chessfont.\n");
		return false;
	}
	lib::log("res_loaded_count: %d\n", res_loaded_count);
	lib::log("res_add: %p\n", res_add);

	return true;
}

// void fot_path_delete(char *& fot_path)
// {
// 	lib::log("* deleting fot_path...");
// 	delete[] fot_path;
// 	fot_path = nullptr;
// 	lib::log("DONE\n");
// }

bool install_font_from_path(char (&name)[MAX_PATH+1], char const * path, char (&fot_path)[MAX_PATH+1])
{
	lib::log("* Loading from path |%s|.\n", path);

	if (!path || !*path) return false;
	if (isdigit(path[0]) && path[1] == '\0') return false;
	if (!lib::file_exists(path)) return false;

	memset(fot_path, 0, sizeof(fot_path));
	lib::make_temp_path(fot_path, MAX_PATH, FOT_NAME);
	// char * fot_path = lib::make_temp_path(FOT_NAME);
	// if (!fot_path) return false;
	// std::unique_ptr<char,decltype(&fot_path_delete)> fot_path_deleter(fot_path, &fot_path_delete);

	DeleteFile(fot_path);

	if (!CreateScalableFontResource(1,fot_path,path,0)) return false;
	if (0 == AddFontResource(fot_path)) return false;

	memset(name, 0, sizeof(name));
	if (!extract_name(fot_path, name, sizeof(name))) return false;

	if (!*name) return false;

	return true;
}

// HFONT create_font(char const * path_or_name, int size)
// {
	// char fot[MAX_PATH+1] = {0};
	// HFONT font = nullptr;
	// if ((font = install_font_from_path(path_or_name, size, fot))) return font;
	// if ((font = create_font_from_name(path_or_name, size, CreateFontMode::Enumerable))) return font;
	// if ((font = install_font_from_resource(path_or_name, size))) return font;
	// return nullptr;
// }

struct chessfont_t
{
	HFONT handle = nullptr;
	char name[MAX_PATH+1] = {0};
	char fot_path[MAX_PATH+1] = {0};
	bool installed = false;

	~chessfont_t() { uninstall(); free(); }

	// operator HFONT() const { return handle; }

	bool create(char const * path_or_name, int size)
	{
		free();

		if (*name)
		{
			lib::log("* Loading already installed font.\n");
			handle = create_font_from_name(name, size, CreateFontMode::All);
			if (handle) return true;
		}

		lib::log("* Reinstalling font.\n");

		uninstall();

		if (install_font_from_path(name, path_or_name, fot_path))
		{
			handle = create_font_from_name(name, size, CreateFontMode::All);
			if (handle) { installed = true; return true; }
		}

		strncpy(name, path_or_name, sizeof(name));
		handle = create_font_from_name(name, size, CreateFontMode::Enumerable);
		if (handle) { installed = false; return true; }

		if (install_font_from_resource(name, path_or_name))
		{
			handle = create_font_from_name(name, size, CreateFontMode::All);
			if (handle) { installed = false; return true; }
		}

		return false;
	}

	void uninstall()
	{
		if (!installed) return;
		RemoveFontResource(fot_path);
		installed = false;
		if (fot_path) { DeleteFile(fot_path); fot_path[0] = '\0'; }
		lib::log("* free: Chessfont (uninstall)\n");
	}

	void free()
	{
		if (!handle) return;
		DeleteObject(handle);
		handle = 0;
		lib::log("* free: Chessfont (uncreate)\n");
	}
};

} // lib
