#pragma once
#include "lib/lib.h" // log

#define BLUE(c) ((unsigned char)((c) & 0xFF))
#define GREEN(c) ((unsigned char)(((c) >> 8) & 0xff))
#define RED(c) ((unsigned char)(((c) >> 16) & 0xff))
#define ALPHA(c) ((unsigned char)(((c) >> 24) & 0xff))


struct context_t
{
	HWND dh = nullptr;
	HDC dc = nullptr;

	~context_t()
	{
		free();
	}

	bool init()
	{
		dh = GetDesktopWindow();
		dc = GetDC(dh);
		return dc;
	}

	void free()
	{
		if (dc) DeleteDC(dc);
	}
};


bool get_bitmap_info(HDC dc, HBITMAP bmp, BITMAPINFO & bi)
{
	BITMAPINFOHEADER & ih = bi.bmiHeader;
	memset(&bi, 0, sizeof(bi));
	ih.biSize = sizeof(ih);
	return GetDIBits(dc, bmp, 0, 0, 0, &bi, DIB_RGB_COLORS);
}


bool get_bitmap_data(HDC dc, HBITMAP bmp, BITMAPINFO & bi, char *& data)
{
	BITMAPINFOHEADER & ih = bi.bmiHeader;
	memset(&bi, 0, sizeof(bi));
	ih.biSize = sizeof(ih);

	if (!GetDIBits(dc, bmp, 0, 0, 0, &bi, DIB_RGB_COLORS)) return false;

	data = new char[ih.biSizeImage];
	if (!data) return false;

	if (!GetDIBits(dc, bmp, 0, ih.biHeight, data, &bi, DIB_RGB_COLORS)) return false;
	return true;
}


size_t calc_bitmap_size(BITMAPINFOHEADER const & ih)
{
	// FIXME: Test this code or find a better reference on bitmaps.

	LONG const height = ih.biHeight < 0 ? -ih.biHeight : ih.biHeight;
	DWORD const pixels = (DWORD)ih.biWidth * (DWORD)height;

	switch (ih.biBitCount)
	{
		case 1: return pixels; break;
		case 4: return pixels; break;
		case 8: return pixels; break;
		case 16: return pixels * 2; break;
		case 24: return pixels * 4; break;
		case 32: return pixels * 4; break;
		default: break;
	}
	return 0;
}


size_t calc_bitmap_size_32(BITMAPINFOHEADER const & ih)
{
	if (ih.biBitCount != 32) return 0;
	return calc_bitmap_size(ih);
}


bool dump_bitmap_data(HDC dc, HBITMAP bmp, FILE * file=stdout)
{
	BITMAPINFO bi;
	BITMAPINFOHEADER & ih = bi.bmiHeader;
	if (!get_bitmap_info(dc, bmp, bi)) return false;
	lib::log("%ldx%ld@%d (%ld)\n", ih.biWidth, ih.biHeight, ih.biBitCount, ih.biSizeImage);

	char * data = nullptr;
	if (!get_bitmap_data(dc, bmp, bi, data)) return false;
	lib::log("Bitmap Data PTR: %p\n", data);
	if (!data) return false;
	if (ih.biBitCount != 32 || ih.biSizeImage != calc_bitmap_size_32(ih))
	{
		if (data) delete[] data;
		return false;
	}

	LONG const height = ih.biHeight < 0 ? -ih.biHeight : ih.biHeight;
	fprintf(file, "P3\n# <https://github.com/dejbug/fen2ppm>\n%ld %ld\n255\n", ih.biWidth, height);

	uint32_t * dwords = (uint32_t *)data;
	// size_t const dwords_count = ih.biSizeImage >> 3;

	// for (size_t i=0; i<dwords_count; ++i)
	// {
	// 	DWORD const & c = dwords[i];
	// 	printf("%d %d %d ", RED(c), GREEN(c), BLUE(c));
	// 	if (i > 0 && (i % 8) == 0) printf("\n");
	// }
	size_t index = 0;
	if (ih.biWidth >= 0)
	{
		for (long j=(long)ih.biHeight-1; j>=0; --j)
		{
			for (long i=0; i<ih.biWidth; ++i)
			{
				DWORD const & c = dwords[i + j*ih.biWidth];
				fprintf(file, "%d %d %d ", RED(c), GREEN(c), BLUE(c));
				if ((++index & 7) == 0) fprintf(file, "\n");
			}
		}
	}

	if (data) delete[] data;
	return true;
}
