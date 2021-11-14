#pragma once
#include <windows.h>
#include "../res/resource.h"

#include "../lib/out.h"
#include "../lib/memory_dc_t.h"
#include "../lib/grid_t.h"
#include "../app/args_t.h"

#define WINDOW_TITLE_MAIN "fen2ppm"

extern args_t args;
extern memory_dc_t mdc;
extern memory_dc_t mdc;
extern grid_t grid;
extern bool update_board();
extern bool draw_board();

namespace app {

LRESULT CALLBACK MainFrame(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
		case WM_CREATE:
		{
			SetWindowText(h, WINDOW_TITLE_MAIN);
			break;
		}

		case WM_PAINT:
		{
			lib::log("* WM_PAINT\n");
			PAINTSTRUCT ps;
			HDC const dc = BeginPaint(h, &ps);

			RECT r;
			grid.get_bounds(r);
			BitBlt(dc, 0, 0, r.right, r.bottom, mdc.dc, 0, 0, SRCCOPY);

			EndPaint(h, &ps);
			break;
		}

		case WM_SIZE:
		{
			lib::log("* WM_SIZE\n");
			RECT r;
			GetClientRect(h, &r);
			long const side = std::min(r.right-r.left, r.bottom-r.top);
			long const edge = side >> 3;
			args.square_size = edge;
			update_board();
			draw_board();
			InvalidateRect(h, NULL, TRUE);
			UpdateWindow(h);
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
