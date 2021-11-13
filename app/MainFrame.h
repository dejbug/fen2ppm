#pragma once
#include <windows.h>
#include "../res/resource.h"

#include "../lib/memory_dc_t.h"

#define WINDOW_TITLE_MAIN "fen2ppm"

#define WMUSER_INIT_PROPS WM_USER + 1

namespace app {

bool ensure_props_available(HWND h, memory_dc_t *& mdc, grid_t *& grid)
{
	if (!mdc) mdc = (memory_dc_t*)GetProp(h, "MDC");
	if (!mdc) return false;
	if (!grid) grid = (grid_t*)GetProp(h, "GRID");
	if (!grid) return false;
	return true;
}

LRESULT CALLBACK MainFrame(HWND h, UINT m, WPARAM w, LPARAM l)
{
	static memory_dc_t * mdc = nullptr;
	static grid_t * grid = nullptr;
	static PAINTSTRUCT ps;

	switch (m)
	{
		case WM_CREATE:
		{
			SetWindowText(h, WINDOW_TITLE_MAIN);
			break;
		}

		case WMUSER_INIT_PROPS:
		{
			printf("WMUSER_INIT_PROPS\n");
			if (!ensure_props_available(h, mdc, grid))
				return 1;
			return 0;
		}

		case WM_PAINT:
		{
			HDC const dc = BeginPaint(h, &ps);

			RECT r;
			grid->get_bounds(r);
			BitBlt(dc, 0, 0, r.right, r.bottom, mdc->dc, 0, 0, SRCCOPY);

			EndPaint(h, &ps);
			break;
		}

		case WM_COMMAND:
		{
			auto const code = HIWORD(w);
			auto const id = LOWORD(w);
			// auto const hctl = (HWND)l;
			bool const is_accelerator = (1 == code);

			if (is_accelerator) switch (id)
			{
				case IDM_ESCAPE:
					SendMessage(h, WM_CLOSE, 0, 0);
					break;

				case IDM_F8:
					break;
			}
			break;
		}

		case WM_CLOSE: DestroyWindow(h); break;
		case WM_DESTROY: PostQuitMessage(0); break;

		default: return CallWindowProc(DefWindowProc, h, m, w, l);
	}
	return 0;
}

} // namespace app
