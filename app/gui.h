#pragma once
#include <windows.h>

#define WINDOW_CLASSNAME_MAIN "fen2ppm-WC"

namespace app {

void init(WNDCLASSEX & wc)
{
	// memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASSNAME_MAIN;
	wc.lpfnWndProc = DefWindowProc;
}

ATOM install(WNDCLASSEX const & wc)
{
	return RegisterClassEx(&wc);
}

HWND create(WNDCLASSEX const & wc)
{
	return CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW|WS_EX_ACCEPTFILES|
		WS_EX_CONTEXTHELP|WS_EX_CONTROLPARENT,
		wc.lpszClassName,
		"",
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		wc.hInstance,
		NULL);
}

int run(MSG & msg, HWND main=NULL, HACCEL haccel=NULL)
{
	while (1)
	{
		int const res = GetMessage(&msg, nullptr, 0, 0);
		if (0 == res) break; // WM_QUIT
		else if (-1 == res) return -1; // errors!
		if (main && haccel) TranslateAccelerator(main, haccel, &msg);
		if (main && IsDialogMessage(main, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void center(HWND handle)
{
	RECT rp, rc;

	HWND parent = GetParent(handle);
	if (parent) GetWindowRect(parent, &rp);
	else rp = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

	GetWindowRect(handle, &rc);

	int const rpw = (rp.right - rp.left);
	int const rph = (rp.bottom - rp.top);
	int const rcw = (rc.right - rc.left);
	int const rch = (rc.bottom - rc.top);

	int const dx = (rpw - rcw) >> 1;
	int const dy = (rph - rch) >> 1;

	SetWindowPos(handle, nullptr, rp.left + dx, rp.top + dy, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

} // namespace app
