#ifndef _COMMAND_BUTTON_
#define _COMMAND_BUTTON_

#include <windows.h>
#include <stdio.h>
#include <string>
#include "BufferDC.h"

#define CMD_NONE		0	// 中止された命令
#define CMD_FLIP_UD		1	// 上下を反転
#define CMD_FLIP_LR		2	// 左右を反転
#define CMD_ERASE_U		3	// 上の図形を消去
#define CMD_ERASE_D		4	// 下の図形を消去
#define CMD_EXCHANGE	5	// 上の図形と交換する
#define CMD_CANCEL_U	6	// 上の命令を中止
#define CMD_CANCEL_D	7	// 下の命令を中止
#define CMD_SORT_4321	8	// 4321の順に並び替え
#define CMD_SORT_3412	9	// 3412の順に並び替え
#define CMD_SORT_2143	10	// 2143の順に並び替え

LRESULT CALLBACK CommandButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CommandButton
{
private:
	BufferDC bufDC;
	HWND hWnd;

public:
	int selected_cmd;	// 選択されている命令．CMD_*
	bool type_sort;		// 並び替え用ならtrue

public:
	CommandButton() : hWnd(NULL), selected_cmd(CMD_FLIP_UD), type_sort(false)
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
			WndClass.lpszClassName = "MyCommandButton";
			WndClass.lpfnWndProc = CommandButtonProc;
			WndClass.hbrBackground = NULL;
		RegisterClassEx(&WndClass);
	}

	void create(HWND hParent, int pos_x, int pos_y, int size_x, int size_y, bool a_type_sort = false)
	{
		type_sort = a_type_sort;
		if(false == type_sort)	selected_cmd = CMD_FLIP_UD;
		else					selected_cmd = CMD_NONE;
		bufDC.Create(size_x, size_y);
		clear();
		CreateCommandButton("", pos_x, pos_y, size_x, size_y, hParent);
	}

	HDC get_DC()
	{
		return bufDC.GetDC();
	}
	HWND get_HWND()
	{
		return hWnd;
	}

	void draw(HDC hDC)
	{
		bufDC.Draw(hDC);
	}

	int get_width()  {return bufDC.GetSize().x;}
	int get_height() {return bufDC.GetSize().y;}

	bool is_type_sort() const {return type_sort;}

	void clear()
	{
		InvalidateRect(hWnd, NULL, FALSE);
	}

private:
	// CommandButton作成
	void CreateCommandButton(const char *WindowName, int x, int y, int cx, int cy, HWND hParent)
	{
		hWnd = CreateWindowEx(	0 //WS_EX_CLIENTEDGE
							,	"MyCommandButton"	//クラス名
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
