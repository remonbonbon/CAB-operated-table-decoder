#include "CommandButton.h"

/***********************************************************************************
	CommandButtonóp
***********************************************************************************/
LRESULT CALLBACK CommandButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HPEN hBlackPen = NULL;
	static HPEN hSelectPen = NULL;
	static HPEN hThinSelectPen = NULL;
	static HFONT hFont = NULL;
	static HFONT hCmdFont = NULL;
	static HBRUSH bSelect = NULL;
	static HBRUSH bFill = NULL;

	switch(msg)
	{
	case WM_CREATE:
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)NULL);

		if(NULL == hBlackPen)
		{
			hBlackPen      = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
			hSelectPen     = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
			hThinSelectPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		}
		if(NULL == hFont)
		{
			hFont = CreateFont(21,
				0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
				"MS GUI Gothic");
			hCmdFont = CreateFont(32,
				0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
				"MS GUI Gothic");
		}
		if(NULL == bSelect)
		{
			bSelect = (HBRUSH)CreateSolidBrush(RGB(255,220,220));
			bFill = (HBRUSH)CreateSolidBrush(RGB(0,0,255));
		}


		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			CommandButton *cb = (CommandButton *)GetWindowLong(hWnd, GWL_USERDATA);
			if(cb)
			{
				HDC hBuf = cb->get_DC();

				HBRUSH bNormal = (HBRUSH)GetStockObject(WHITE_BRUSH);

				SetBkMode(hBuf, TRANSPARENT);
				char text[32];

				RECT rc = {0};
				rc.bottom = cb->get_height();
				const int w = cb->get_width();
				if(false == cb->is_type_sort())
				{
					const int c = 7;
					for(int i=0; i<c; i++)
					{
						rc.left  = (w * i)     / c;
						rc.right = (w * (i+1)) / c + (i<c-1 ? 1 : 0);

						SelectObject(hBuf, GetStockObject(BLACK_PEN));
						SelectObject(hBuf, GetStockObject(NULL_BRUSH));
						FillRect(hBuf, &rc,
							(cb->selected_cmd == i+CMD_FLIP_UD) ? bSelect : bNormal);
						Rectangle(hBuf, rc.left, rc.top, rc.right, rc.bottom);

						switch(i+CMD_FLIP_UD)
						{
						case CMD_FLIP_UD:	strcpy(text, "è„â∫\nãtÇ≥Ç‹");	break;
						case CMD_FLIP_LR:	strcpy(text, "ç∂âE\nãtÇ≥Ç‹");	break;
						case CMD_ERASE_U:	strcpy(text, "ëO ê}å`\nè¡Ç∑");	break;
						case CMD_ERASE_D:	strcpy(text, "éü ê}å`\nè¡Ç∑");	break;
						case CMD_EXCHANGE:	strcpy(text, "ëOÇ∆\nì¸ÇÍÇ©Ç¶");	break;
						case CMD_CANCEL_U:	strcpy(text, "ëO ñΩóﬂ\néÊè¡");	break;
						case CMD_CANCEL_D:	strcpy(text, "éü ñΩóﬂ\néÊè¡");	break;
						default:			strcpy(text, "???");			break;
						}

						if(cb->selected_cmd == i+CMD_FLIP_UD)	SetTextColor(hBuf, RGB(0,0,255));
						else									SetTextColor(hBuf, RGB(0,0,0));

						rc.top = 20;
						SelectObject(hBuf, hFont);
						DrawText(hBuf, text, strlen(text), &rc, DT_CENTER);
						rc.top = 0;

						RECT fig = {rc.left+10, rc.bottom-10-(rc.right-rc.left-20), rc.right-10, rc.bottom-10};
						const int cx = fig.left + (fig.right - fig.left)/2;
						const int cy = fig.top  + (fig.bottom - fig.top)/2;
						SelectObject(hBuf, (cb->selected_cmd == i+CMD_FLIP_UD) ? hThinSelectPen : GetStockObject(BLACK_PEN));
						Ellipse(hBuf, fig.left, fig.top, fig.right, fig.bottom);
						SelectObject(hBuf, (cb->selected_cmd == i+CMD_FLIP_UD) ? hSelectPen : hBlackPen);
						switch(i+CMD_FLIP_UD)
						{
						case CMD_FLIP_UD:
							MoveToEx(hBuf, cx, fig.top+10, NULL);
							LineTo(hBuf,   cx, fig.bottom-10);
							LineTo(hBuf,   cx-10, cy);
							MoveToEx(hBuf, cx, fig.bottom-10, NULL);
							LineTo(hBuf,   cx+10, cy);
							break;
						case CMD_FLIP_LR:
							MoveToEx(hBuf, fig.right-10, cy, NULL);
							LineTo(hBuf,   fig.left+10,  cy);
							LineTo(hBuf,   cx, cy-10);
							MoveToEx(hBuf,   fig.left+10,  cy, NULL);
							LineTo(hBuf,   cx, cy+10);
							break;
						case CMD_ERASE_U:
							MoveToEx(hBuf, cx, fig.top, NULL);
							LineTo(hBuf,   cx, fig.bottom);
							Ellipse(hBuf, cx-10, cy-10, cx+10, cy+10);
							break;
						case CMD_ERASE_D:
							MoveToEx(hBuf, fig.right, cy, NULL);
							LineTo(hBuf,   fig.left,  cy);
							Ellipse(hBuf, cx-10, cy-10, cx+10, cy+10);
							break;
						case CMD_EXCHANGE:
							SelectObject(hBuf, hCmdFont);
							DrawText(hBuf, "G", 1, &fig, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case CMD_CANCEL_U:
							SelectObject(hBuf, (cb->selected_cmd == i+CMD_FLIP_UD) ? bFill : GetStockObject(BLACK_BRUSH));
							Ellipse(hBuf, cx-10, cy-10, cx+10, cy+10);
							break;
						case CMD_CANCEL_D:
							Ellipse(hBuf, cx-10, cy-10, cx+10, cy+10);
							break;
						}
					}
				}
				else
				{
					const int c = 4;
					const int cmd_list[c] = {CMD_NONE, CMD_SORT_4321, CMD_SORT_3412, CMD_SORT_2143};
					for(int i=0; i<c; i++)
					{
						rc.left  = (w * i)     / c;
						rc.right = (w * (i+1)) / c + (i<c-1 ? 1 : 0);

						SelectObject(hBuf, GetStockObject(BLACK_PEN));
						SelectObject(hBuf, GetStockObject(NULL_BRUSH));
						FillRect(hBuf, &rc,
							(cb->selected_cmd == cmd_list[i]) ? bSelect : bNormal);
						Rectangle(hBuf, rc.left, rc.top, rc.right, rc.bottom);

						switch(cmd_list[i])
						{
						case CMD_NONE:		strcpy(text, "ï¿ë÷\nÇµÇ»Ç¢");	break;
						case CMD_SORT_4321:	strcpy(text, "ï¿ë÷\n4321");		break;
						case CMD_SORT_3412:	strcpy(text, "ï¿ë÷\n3412");		break;
						case CMD_SORT_2143:	strcpy(text, "ï¿ë÷\n2143");		break;
						default:			strcpy(text, "???");			break;
						}

						if(cb->selected_cmd == cmd_list[i])	SetTextColor(hBuf, RGB(0,0,255));
						else								SetTextColor(hBuf, RGB(0,0,0));

						rc.top = 20;
						SelectObject(hBuf, hFont);
						DrawText(hBuf, text, strlen(text), &rc, DT_CENTER);
						rc.top = 0;

						RECT fig = {rc.left+10, rc.bottom-10-(rc.right-rc.left-20), rc.right-10, rc.bottom-10};
						const int cx = fig.left + (fig.right - fig.left)/2;
						const int cy = fig.top  + (fig.bottom - fig.top)/2;
						SelectObject(hBuf, (cb->selected_cmd == cmd_list[i]) ? hThinSelectPen : GetStockObject(BLACK_PEN));
						if(CMD_NONE != cmd_list[i]) Ellipse(hBuf, fig.left, fig.top, fig.right, fig.bottom);
						SelectObject(hBuf, (cb->selected_cmd == cmd_list[i]) ? hSelectPen : hBlackPen);
						switch(cmd_list[i])
						{
						case CMD_NONE:
							break;
						case CMD_SORT_4321:
							SelectObject(hBuf, hCmdFont);
							DrawText(hBuf, "R", 1, &fig, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case CMD_SORT_3412:
							SelectObject(hBuf, hCmdFont);
							DrawText(hBuf, "S", 1, &fig, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case CMD_SORT_2143:
							SelectObject(hBuf, hCmdFont);
							DrawText(hBuf, "J", 1, &fig, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						}
					}
				}


				cb->draw(hDC);
			}
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_LBUTTONDOWN:
		{
			CommandButton *cb = (CommandButton *)GetWindowLong(hWnd, GWL_USERDATA);
			if(cb)
			{
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);

				if(x < 0) x = 0;
				if(cb->get_width() <= x) x = cb->get_width()-1;

				cb->selected_cmd = CMD_NONE;
				if(false == cb->is_type_sort())
				{
					int cmd = (x * 7) / cb->get_width();
					if(0 <= cmd && cmd < 7)
					{
						cb->selected_cmd = CMD_FLIP_UD + cmd;
					}
				}
				else
				{
					int cmd = (x * 4) / cb->get_width();
					const int cmd_list[4] = {CMD_NONE, CMD_SORT_4321, CMD_SORT_3412, CMD_SORT_2143};
					if(0 <= cmd && cmd < 4)
					{
						cb->selected_cmd = cmd_list[cmd];
					}
				}

				InvalidateRect(hWnd, NULL, FALSE);
				SendMessage(GetParent(hWnd), WM_APP, 0, 0);	// çƒåvéZóvãÅ
			}
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
