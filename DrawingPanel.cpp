#include "DrawingPanel.h"

/***********************************************************************************
	描画コントロール用
***********************************************************************************/
LRESULT CALLBACK DrawCtrlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HPEN hPen  = NULL;
	static HPEN hPen2 = NULL;
	static int pre_x = 0;
	static int pre_y = 0;
	static HWND hActiveWnd = NULL;
	static HFONT hFont = NULL;

	switch(msg)
	{
	case WM_CREATE:
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)NULL);

		if(NULL == hPen)
		{
			hPen  = CreatePen(PS_SOLID, 3,  RGB(0, 0, 0));
			hPen2 = CreatePen(PS_SOLID, 32, RGB(0, 0, 0));
		}
		if(NULL == hFont)
		{
			hFont = CreateFont(24,
				0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
				"MS GUI Gothic");
		}

		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			DrawingPanel *dp = (DrawingPanel *)GetWindowLong(hWnd, GWL_USERDATA);
			if(dp)
			{
				dp->draw(hDC);

				char text[32];
				sprintf(text, "%d", dp->ID);
				std::string tmp;
				tmp = text + dp->text;

				SelectObject(hDC, hFont);
				SetTextColor(hDC, RGB(0,0,255));
				SetBkMode(hDC, TRANSPARENT);
				TextOut(hDC, 0, 0, tmp.c_str(), tmp.length());
			}
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		{
			pre_x = LOWORD(lParam);
			pre_y = HIWORD(lParam);
			hActiveWnd = NULL;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	// お絵かき機能
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			if(hWnd != hActiveWnd)
			{
				pre_x = x;
				pre_y = y;
				hActiveWnd = hWnd;
			}

			DrawingPanel *dp = (DrawingPanel *)GetWindowLong(hWnd, GWL_USERDATA);
			if(dp && dp->is_user_drawing()
			&& (0 != (wParam & MK_LBUTTON) || 0 != (wParam & MK_RBUTTON)))
			{
				HDC hDC = dp->get_DC();


				if(wParam & MK_LBUTTON)	SelectObject(hDC, hPen);
				else					SelectObject(hDC, hPen2);
				MoveToEx(hDC, pre_x, pre_y, NULL);
				LineTo(hDC, x, y);

				dp->update();
				SendMessage(GetParent(hWnd), WM_APP, 0, 0);	// 再計算要求
			}

			pre_x = x;
			pre_y = y;
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
