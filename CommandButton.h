#ifndef _COMMAND_BUTTON_
#define _COMMAND_BUTTON_

#include <windows.h>
#include <stdio.h>
#include <string>
#include "BufferDC.h"

#define CMD_NONE		0	// ���~���ꂽ����
#define CMD_FLIP_UD		1	// �㉺�𔽓]
#define CMD_FLIP_LR		2	// ���E�𔽓]
#define CMD_ERASE_U		3	// ��̐}�`������
#define CMD_ERASE_D		4	// ���̐}�`������
#define CMD_EXCHANGE	5	// ��̐}�`�ƌ�������
#define CMD_CANCEL_U	6	// ��̖��߂𒆎~
#define CMD_CANCEL_D	7	// ���̖��߂𒆎~
#define CMD_SORT_4321	8	// 4321�̏��ɕ��ёւ�
#define CMD_SORT_3412	9	// 3412�̏��ɕ��ёւ�
#define CMD_SORT_2143	10	// 2143�̏��ɕ��ёւ�

LRESULT CALLBACK CommandButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CommandButton
{
private:
	BufferDC bufDC;
	HWND hWnd;

public:
	int selected_cmd;	// �I������Ă��閽�߁DCMD_*
	bool type_sort;		// ���ёւ��p�Ȃ�true

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
	// CommandButton�쐬
	void CreateCommandButton(const char *WindowName, int x, int y, int cx, int cy, HWND hParent)
	{
		hWnd = CreateWindowEx(	0 //WS_EX_CLIENTEDGE
							,	"MyCommandButton"	//�N���X��
							,	WindowName	//�E�B���h�E��
							,	WS_CHILD | WS_VISIBLE	//�E�B���h�E�X�^�C��
							,	x, y	//���W
							,	cx, cy	//���E����
							,	hParent, NULL, GetModuleHandle(NULL), NULL	//hParentWnd�AhMenu�AhInst�A�E�B���h�E�쐬�f�[�^
							);

		SetWindowLong(hWnd, GWL_USERDATA, (LONG)(this));
	}
};

#endif
