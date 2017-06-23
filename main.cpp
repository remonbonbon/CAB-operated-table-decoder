#include "main.h"

MyCommonStructure mcs;

///////////////////////////////////////////////////////////////////////////////

void cmd_exec(CAB_figure fig[4], CAB_command cmd[4], int i, CAB_command &cur, CAB_command &undo, bool undo_call = false)
{
	switch(cur.cmd)
	{
	case CMD_NONE:
		// 中止された命令
		undo = cur;
		break;
	case CMD_FLIP_UD:
		// 上下反転
		fig[i].stat ^= FIG_STAT_FLIP_UD;
		undo = cur;
		break;
	case CMD_FLIP_LR:
		// 左右反転
		fig[i].stat ^= FIG_STAT_FLIP_LR;
		undo = cur;
		break;
	case CMD_ERASE_U:
		// 上の図形を消去
		if(0 < i)
		{
			if(undo_call)	fig[i-1].stat ^= FIG_STAT_ERASE;	// undo時は消去から戻すので反転させる
			else			fig[i-1].stat |= FIG_STAT_ERASE;	// 通常時は論理和
		}
		undo = cur;
		break;
	case CMD_ERASE_D:
		// 下の図形を消去
		if(i < 3)
		{
			if(undo_call)	fig[i+1].stat ^= FIG_STAT_ERASE;	// undo時は消去から戻すので反転させる
			else			fig[i+1].stat |= FIG_STAT_ERASE;	// 通常時は論理和
		}
		undo = cur;
		break;
	case CMD_CANCEL_U:
		// 前の命令をキャンセル（undo）
		if(0 < i)
		{
			cmd_exec(fig, cmd, undo.figure, undo, undo, true);
		}
		break;
	case CMD_CANCEL_D:
		// 次の命令をキャンセル
		if(i < 3) cmd[i+1].canceled = true;
		break;
	case CMD_EXCHANGE:
		// 上の図形と交換
		if(0 < i)
		{
			std::swap(fig[i-1], fig[i]);
		}
		undo = cur;
		break;
	}
}

/***********************************************************************************
	ウィンドウプロシージャ
***********************************************************************************/
LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			mcs.hMain = hWnd;

			const int sep = 5;
			const int btn = 70;
			const int size = 160;
			const int cmd_size = 80;
			int x = sep, y = sep;
			for(int i=0; i<4; i++)
			{
				x = sep;

				mcs.clear_btn[i] = CreateWindow(TEXT("BUTTON"), TEXT("クリア"),
					WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					x, y+(size-btn)/2, btn, btn,
					hWnd, (HMENU)i+100, mcs.hInst, NULL);
				SET_GUI_FONT(mcs.clear_btn[i]);
				x += sep + btn;

				mcs.drawing[i].create(i+1, hWnd, x, y, size, size);
				x += sep + size;

				mcs.cmd[i].create(hWnd, x, y, cmd_size*7, size);
				x += sep + cmd_size*7;

				mcs.result_disp[i].create(-(i+1), hWnd, x, y, size, size, false);
				x += sep + size;

				y += size+sep;
			}
			x = sep;
			mcs.all_clear_btn = CreateWindow(TEXT("BUTTON"), TEXT("全てクリア"),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				x, y+(size-btn)/2, btn*2, btn,
				hWnd, (HMENU)200, mcs.hInst, NULL);
			SET_GUI_FONT(mcs.all_clear_btn);

			x = sep * 3 + btn + size;
			mcs.sort_cmd.create(hWnd, x, y, cmd_size*4, size, true);

			SendMessage(hWnd, WM_APP, 0, 0);	// 再計算要求
		}
		return 0;
	case WM_APP:
		{
			// 再計算要求
			//1. CommandButtonより命令を取得
			//2. 命令を処理する
			//	a. 命令消去系を処理（undoは面倒なので）
			//	b. 上から順に処理
			//3. 処理後のCAB_figure[4]を作成
			//4. DrawingPanelに元の図形，処理結果フラグを渡して描画

			CAB_figure  fig[4];
			CAB_command cmd[4];
			for(int i=0; i<4; i++)
			{
				cmd[i].cmd      = mcs.cmd[i].selected_cmd;
				cmd[i].figure   = i;
				cmd[i].canceled = false;
				fig[i].figure = i; 
				fig[i].stat = FIG_STAT_NORMAL;
			}

			// 上から順に処理
			CAB_command undo;
			for(int i=0; i<4; i++)
			{
				if(true == cmd[i].canceled) continue;
				cmd_exec(fig, cmd, i, cmd[i], undo);
			}

			// 並び替えを実効
			if(CMD_NONE != mcs.sort_cmd.selected_cmd)
			{
				CAB_figure tmp[4];
				for(int i=0; i<4; i++)
				{
					tmp[i] = fig[i];
				}

				unsigned char order[4] = {1, 2, 3, 4};
				switch(mcs.sort_cmd.selected_cmd)
				{
				case CMD_SORT_4321:
					order[0] = 3;
					order[1] = 2;
					order[2] = 1;
					order[3] = 0;
					break;
				case CMD_SORT_3412:
					order[0] = 2;
					order[1] = 3;
					order[2] = 0;
					order[3] = 1;
					break;
				case CMD_SORT_2143:
					order[0] = 1;
					order[1] = 0;
					order[2] = 3;
					order[3] = 2;
					break;
				}

				for(int i=0; i<4; i++)
				{
					fig[i] = tmp[order[i]];
				}
			}

			// DrawingPanelを再描画
			for(int i=0; i<4; i++)
			{
				HDC hDestDC = mcs.result_disp[i].get_DC();
				HDC hSrcDC  = mcs.drawing[fig[i].figure].get_DC();

				RECT rc = {0, 0, mcs.result_disp[i].get_width(), mcs.result_disp[i].get_height()};

				mcs.result_disp[i].ID = fig[i].figure +1;	// IDを設定
				mcs.result_disp[i].text.clear();			// textをリセット

				if(FIG_STAT_ERASE & fig[i].stat)
				{
					// 消去
					FillRect(hDestDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
					mcs.result_disp[i].text += " 消去";
				}
				else
				{
					POINT pt[3];
					pt[0].x = rc.left;	pt[0].y = rc.top;	// 左上の転送先
					pt[1].x = rc.right;	pt[1].y = rc.top;	// 右上の転送先
					pt[2].x = rc.left;	pt[2].y = rc.bottom;// 左下の転送先
					if(FIG_STAT_FLIP_UD & fig[i].stat)
					{
						// 上下反転
						pt[0].y = rc.bottom;
						pt[1].y = rc.bottom;
						pt[2].y = rc.top;
						mcs.result_disp[i].text += " 上下flip";
					}
					if(FIG_STAT_FLIP_LR & fig[i].stat)
					{
						// 左右反転
						pt[0].x = rc.right;
						pt[1].x = rc.left;
						pt[2].x = rc.right;
						mcs.result_disp[i].text += " 左右flip";
					}
					PlgBlt(hDestDC, pt, hSrcDC, rc.left, rc.top, rc.right, rc.bottom, NULL, 0, 0);
				}

				mcs.result_disp[i].update();
			}
		}
		return 0;
	case WM_COMMAND:
		for(int i=0; i<4; i++)
		{
			if((HWND)lParam == mcs.clear_btn[i])
			{
				mcs.drawing[i].clear();
				SendMessage(hWnd, WM_APP, 0, 0);	// 再計算要求
			}
		}
		if((HWND)lParam == mcs.all_clear_btn)
		{
			for(int i=0; i<4; i++)
			{
				mcs.drawing[i].clear();
				mcs.cmd[i].selected_cmd = CMD_FLIP_UD;	// 命令をリセット
				InvalidateRect(mcs.cmd[i].get_HWND(), NULL, FALSE);	// 再描画
			}
			mcs.sort_cmd.selected_cmd = CMD_NONE;	// ソートをリセット
			InvalidateRect(mcs.sort_cmd.get_HWND(), NULL, FALSE);	// 再描画
			SendMessage(hWnd, WM_APP, 0, 0);	// 再計算要求
		}
		return 0;
	case WM_SIZE:
		{
		}
		return TRUE;
	case WM_NOTIFY:
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/***********************************************************************************
	WinMain
***********************************************************************************/
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	//GDI+初期化
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//コモンコントロール初期化
	INITCOMMONCONTROLSEX ic={0};
	ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
	ic.dwICC = ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES;
	InitCommonControlsEx(&ic); 

	mcs.hInst = hCurInst;

	WNDCLASSEX WndClass = {0};
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.hInstance = hCurInst;
		WndClass.lpszClassName = PROGRAM_NAME;
		WndClass.lpfnWndProc = MainProc;
		WndClass.lpszMenuName = NULL;
		WndClass.hIcon   = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	if(!RegisterClassEx(&WndClass)) return FALSE;

	HWND hWnd = CreateWindow(PROGRAM_NAME	//クラス名
						,	PROGRAM_NAME	//ウィンドウ名
						,	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN	//ウィンドウスタイル
						,	CW_USEDEFAULT, CW_USEDEFAULT	//座標
						,	980, 860	//幅・高さ
						,	NULL, NULL, hCurInst, NULL	//hParentWnd、hMenu、hInst、ウィンドウ作成データ
						);
	if(!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	BOOL bRet;
	while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if(-1 == bRet) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//GDI+終了
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;//(int)msg.wParam;
}
