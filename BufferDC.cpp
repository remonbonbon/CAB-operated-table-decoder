#include "BufferDC.h"

///////////////////////////////////////////////////////////////////////////////

/***********************************************************************************
	デストラクタ
***********************************************************************************/
BufferDC::~BufferDC()
{
	if(this->hDC)
	{
		DeleteDC(this->hDC);
		this->hDC = 0;
	}
	if(this->hBmp)
	{
		DeleteObject(this->hBmp);
		this->hBmp = 0;
	}
}

/***********************************************************************************
	バッファを作る。
***********************************************************************************/
BOOL BufferDC::Create(HDC hCompatibleDC, int cx, int cy)
{
	this->~BufferDC();

	this->hDC = CreateCompatibleDC(hCompatibleDC);
	if(this->hDC)
	{
		this->hBmp = CreateCompatibleBitmap(hCompatibleDC, cx, cy);
		if(this->hBmp)
		{
			SelectObject(this->hDC, this->hBmp);
			this->size.x = cx;
			this->size.y = cy;
			return TRUE;
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
