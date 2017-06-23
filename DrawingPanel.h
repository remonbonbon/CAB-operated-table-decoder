#ifndef _DRAWING_PANEL_
#define _DRAWING_PANEL_

#include <windows.h>
#include <stdio.h>
#include <string>
#include "BufferDC.h"

LRESULT CALLBACK DrawCtrlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////

class DrawingPanel
{
private:
	BufferDC bufDC;

	bool user_drawing;	// お絵かきをするならtrue

	HWND hWnd;

public:
	int ID;
	std::string text;

public:
	DrawingPanel() : user_drawing(true), hWnd(NULL)
	{
		HINSTANCE hInst = GetModuleHandle(NULL);
		WNDCLASSEX WndClass = {0};
			WndClass.cbSize = sizeof(WNDCLASSEX);
			WndClass.hInstance = hInst;
			WndClass.lpszMenuName = NULL;
			WndClass.hIcon   = LoadIcon(NULL, IDI_APPLICATION);
			WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

			WndClass.style = CS_PARENTDC;
			WndClass.lpszClassName = "MyDrawControl";
			WndClass.lpfnWndProc = DrawCtrlProc;
			WndClass.hbrBackground = NULL;
		RegisterClassEx(&WndClass);
	}

public:
	void create(int id, HWND hParent, int pos_x, int pos_y, int size_x, int size_y, bool a_user_drawing = true)
	{
		ID = id;
		user_drawing = a_user_drawing;
		bufDC.Create(size_x, size_y);
		clear();
		CreateDrawControl("", pos_x, pos_y, size_x, size_y, hParent);
	}

	HDC get_DC()
	{
		return bufDC.GetDC();
	}

	int get_width()  {return bufDC.GetSize().x;}
	int get_height() {return bufDC.GetSize().y;}

	bool is_user_drawing() const {return user_drawing;}

	void clear()
	{
		RECT rc = {0, 0, bufDC.GetSize().x, bufDC.GetSize().y};
		HDC hDC = bufDC.GetDC();
		FillRect(hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		update();
	}

	void update()
	{
		InvalidateRect(hWnd, NULL, FALSE);
	}

	void draw(HDC hDC)
	{
		bufDC.Draw(hDC);
	}

private:
	// 描画コントロール作成
	void CreateDrawControl(const char *WindowName, int x, int y, int cx, int cy, HWND hParent)
	{
		hWnd = CreateWindowEx(	0 //WS_EX_CLIENTEDGE
							,	"MyDrawControl"	//クラス名
							,	WindowName	//ウィンドウ名
							,	WS_CHILD | WS_VISIBLE	//ウィンドウスタイル
							,	x, y	//座標
							,	cx, cy	//幅・高さ
							,	hParent, NULL, GetModuleHandle(NULL), NULL	//hParentWnd、hMenu、hInst、ウィンドウ作成データ
							);

		SetWindowLong(hWnd, GWL_USERDATA, (LONG)(this));
	}
};

#endif
