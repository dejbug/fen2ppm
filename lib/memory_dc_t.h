#pragma once
#include <windows.h>

struct memory_dc_context_t
{
	HWND dh = nullptr;
	HDC dc = nullptr;

	~memory_dc_context_t() { free(); }

	bool create()
	{
		dh = GetDesktopWindow();
		dc = GetDC(dh);
		lib::log("Context DC: %p / HWND: %p\n", (void *)dc, (void *)dh);
		return dc;
	}

	void free()
	{
		if (!dc) return;
		ReleaseDC(dh, dc);
		dh = NULL;
		dc = NULL;
		lib::log("* free: Context DC\n");
	}
};

struct memory_dc_t
{
	memory_dc_context_t ctx;
	HDC dc = NULL;
	HBITMAP bmp = NULL;

	~memory_dc_t() { free(); }

	bool create(int image_size)
	{
		free();
		if (!ctx.create()) return false;
		lib::log("Desktop HWND: %p / DC: %p\n", (void *)ctx.dh, (void *)ctx.dc);
		dc = CreateCompatibleDC(ctx.dc);
		if (!dc) return false;
		lib::log("Memory HDC: %p\n", (void *)dc);
		bmp = CreateCompatibleBitmap(ctx.dc, image_size, image_size);
		if (!bmp) return false;
		lib::log("Memory HBITMAP: %p\n", (void *)bmp);
		SelectObject(dc, bmp);
		return true;
	}

	void free()
	{
		ctx.free();
		if (bmp)
		{
			if (dc) SelectObject(dc, (HBITMAP)NULL);
			DeleteObject(bmp);
			bmp = NULL;
			lib::log("* free: Memory HBITMAP\n");
		}
		if (dc)
		{
			DeleteDC(dc);
			dc = NULL;
			lib::log("* free: Memory DC\n");
		}
	}
};
