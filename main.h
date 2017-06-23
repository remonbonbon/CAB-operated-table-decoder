#ifndef _COMMON_HEADER_
#define _COMMON_HEADER_

///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <list>
#include <sstream>
#include "BufferDC.h"

#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "DrawingPanel.h"
#include "CommandButton.h"
#include "CAB_command.h"

///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////

//ウィンドウ位置、サイズを変更
#define SET_WND_POSIZE(hWnd, x, y, cx, cy)	SetWindowPos(hWnd, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOREDRAW)

//コントロールにGUIデフォルトフォントを設定
#define SET_GUI_FONT(hWnd)	SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE)

//WM_NOTIFYを解読
//#define WM_NOTIFY_ID(lParam)	(((LPNMHDR)lParam)->idFrom)
//#define WM_NOTIFY_HWND(lParam)	(((LPNMHDR)lParam)->hwndFrom)
#define WM_NOTIFY_CODE(lParam)	(((LPNMHDR)lParam)->code)

///////////////////////////////////////////////////////////////////////////////

#define PROGRAM_NAME	"Yellow Taxi"

///////////////////////////////////////////////////////////////////////////////

struct MyCommonStructure
{
	HINSTANCE hInst;
	HWND hMain;

	HWND			clear_btn[4];
	DrawingPanel	drawing[4];
	CommandButton	cmd[4];
	DrawingPanel	result_disp[4];
	CommandButton	sort_cmd;

	HWND all_clear_btn;
};

///////////////////////////////////////////////////////////////////////////////

#endif
