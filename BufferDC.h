#ifndef _BUFFER_DC_HEADER_
#define _BUFFER_DC_HEADER_

#include <windows.h>

#include <wingdi.h>
#pragma comment(lib, "msimg32.lib")

///////////////////////////////////////////////////////////////////////////////

class BufferDC
{
private:
	HBITMAP hBmp;
	HDC hDC;
	POINT size;
public:
	BufferDC() : hBmp(NULL), hDC(NULL) {size.x = 0; size.y = 0;}
	BufferDC(int cx, int cy) : hBmp(NULL), hDC(NULL)
	{
		HDC hDC = ::GetDC(NULL);
		Create(hDC, cx, cy);
		ReleaseDC(NULL, hDC);
	}
	~BufferDC();

	POINT GetSize(){return this->size;}
	HDC GetDC(){return this->hDC;}

	BOOL Create(int cx, int cy)
	{
		HDC hDC = ::GetDC(NULL);
		Create(hDC, cx, cy);
		ReleaseDC(NULL, hDC);
		return TRUE;
	}
	BOOL Create(HDC hCompatibleDC, int cx, int cy);

	void Draw(HDC hDestDC)
	{
		BitBlt(hDestDC, 0, 0, size.x, size.y, hDC, 0, 0, SRCCOPY);
	}
	void Draw(HDC hDestDC, int x, int y)
	{
		BitBlt(hDestDC, x, y, size.x, size.y, hDC, 0, 0, SRCCOPY);
	}
};

#endif