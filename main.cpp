#include "main.h"

MyCommonStructure mcs;

///////////////////////////////////////////////////////////////////////////////

void cmd_exec(CAB_figure fig[4], CAB_command cmd[4], int i, CAB_command &cur, CAB_command &undo, bool undo_call = false)
{
	switch(cur.cmd)
	{
	case CMD_NONE:
		// ���~���ꂽ����
		undo = cur;
		break;
	case CMD_FLIP_UD:
		// �㉺���]
		fig[i].stat ^= FIG_STAT_FLIP_UD;
		undo = cur;
		break;
	case CMD_FLIP_LR:
		// ���E���]
		fig[i].stat ^= FIG_STAT_FLIP_LR;
		undo = cur;
		break;
	case CMD_ERASE_U:
		// ��̐}�`������
		if(0 < i)
		{
			if(undo_call)	fig[i-1].stat ^= FIG_STAT_ERASE;	// undo���͏�������߂��̂Ŕ��]������
			else			fig[i-1].stat |= FIG_STAT_ERASE;	// �ʏ펞�͘_���a
		}
		undo = cur;
		break;
	case CMD_ERASE_D:
		// ���̐}�`������
		if(i < 3)
		{
			if(undo_call)	fig[i+1].stat ^= FIG_STAT_ERASE;	// undo���͏�������߂��̂Ŕ��]������
			else			fig[i+1].stat |= FIG_STAT_ERASE;	// �ʏ펞�͘_���a
		}
		undo = cur;
		break;
	case CMD_CANCEL_U:
		// �O�̖��߂��L�����Z���iundo�j
		if(0 < i)
		{
			cmd_exec(fig, cmd, undo.figure, undo, undo, true);
		}
		break;
	case CMD_CANCEL_D:
		// ���̖��߂��L�����Z��
		if(i < 3) cmd[i+1].canceled = true;
		break;
	case CMD_EXCHANGE:
		// ��̐}�`�ƌ���
		if(0 < i)
		{
			std::swap(fig[i-1], fig[i]);
		}
		undo = cur;
		break;
	}
}

/***********************************************************************************
	�E�B���h�E�v���V�[�W��
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

				mcs.clear_btn[i] = CreateWindow(TEXT("BUTTON"), TEXT("�N���A"),
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
			mcs.all_clear_btn = CreateWindow(TEXT("BUTTON"), TEXT("�S�ăN���A"),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				x, y+(size-btn)/2, btn*2, btn,
				hWnd, (HMENU)200, mcs.hInst, NULL);
			SET_GUI_FONT(mcs.all_clear_btn);

			x = sep * 3 + btn + size;
			mcs.sort_cmd.create(hWnd, x, y, cmd_size*4, size, true);

			SendMessage(hWnd, WM_APP, 0, 0);	// �Čv�Z�v��
		}
		return 0;
	case WM_APP:
		{
			// �Čv�Z�v��
			//1. CommandButton��薽�߂��擾
			//2. ���߂���������
			//	a. ���ߏ����n�������iundo�͖ʓ|�Ȃ̂Łj
			//	b. �ォ�珇�ɏ���
			//3. �������CAB_figure[4]���쐬
			//4. DrawingPanel�Ɍ��̐}�`�C�������ʃt���O��n���ĕ`��

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

			// �ォ�珇�ɏ���
			CAB_command undo;
			for(int i=0; i<4; i++)
			{
				if(true == cmd[i].canceled) continue;
				cmd_exec(fig, cmd, i, cmd[i], undo);
			}

			// ���ёւ�������
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

			// DrawingPanel���ĕ`��
			for(int i=0; i<4; i++)
			{
				HDC hDestDC = mcs.result_disp[i].get_DC();
				HDC hSrcDC  = mcs.drawing[fig[i].figure].get_DC();

				RECT rc = {0, 0, mcs.result_disp[i].get_width(), mcs.result_disp[i].get_height()};

				mcs.result_disp[i].ID = fig[i].figure +1;	// ID��ݒ�
				mcs.result_disp[i].text.clear();			// text�����Z�b�g

				if(FIG_STAT_ERASE & fig[i].stat)
				{
					// ����
					FillRect(hDestDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
					mcs.result_disp[i].text += " ����";
				}
				else
				{
					POINT pt[3];
					pt[0].x = rc.left;	pt[0].y = rc.top;	// ����̓]����
					pt[1].x = rc.right;	pt[1].y = rc.top;	// �E��̓]����
					pt[2].x = rc.left;	pt[2].y = rc.bottom;// �����̓]����
					if(FIG_STAT_FLIP_UD & fig[i].stat)
					{
						// �㉺���]
						pt[0].y = rc.bottom;
						pt[1].y = rc.bottom;
						pt[2].y = rc.top;
						mcs.result_disp[i].text += " �㉺flip";
					}
					if(FIG_STAT_FLIP_LR & fig[i].stat)
					{
						// ���E���]
						pt[0].x = rc.right;
						pt[1].x = rc.left;
						pt[2].x = rc.right;
						mcs.result_disp[i].text += " ���Eflip";
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
				SendMessage(hWnd, WM_APP, 0, 0);	// �Čv�Z�v��
			}
		}
		if((HWND)lParam == mcs.all_clear_btn)
		{
			for(int i=0; i<4; i++)
			{
				mcs.drawing[i].clear();
				mcs.cmd[i].selected_cmd = CMD_FLIP_UD;	// ���߂����Z�b�g
				InvalidateRect(mcs.cmd[i].get_HWND(), NULL, FALSE);	// �ĕ`��
			}
			mcs.sort_cmd.selected_cmd = CMD_NONE;	// �\�[�g�����Z�b�g
			InvalidateRect(mcs.sort_cmd.get_HWND(), NULL, FALSE);	// �ĕ`��
			SendMessage(hWnd, WM_APP, 0, 0);	// �Čv�Z�v��
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
	//GDI+������
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//�R�����R���g���[��������
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

	HWND hWnd = CreateWindow(PROGRAM_NAME	//�N���X��
						,	PROGRAM_NAME	//�E�B���h�E��
						,	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN	//�E�B���h�E�X�^�C��
						,	CW_USEDEFAULT, CW_USEDEFAULT	//���W
						,	980, 860	//���E����
						,	NULL, NULL, hCurInst, NULL	//hParentWnd�AhMenu�AhInst�A�E�B���h�E�쐬�f�[�^
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

	//GDI+�I��
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;//(int)msg.wParam;
}
