#pragma once
#include "header.h"

VOID Draw_Image(HDC hdc, char *Image_name,int xpos, int ypos)
{
	wchar_t buf[100];
	int lens;
	lens = swprintf(buf, 100, L"%hs", image_file_name); // %hs �������
	
	Image image(buf);
	Graphics graphics(hdc);
	graphics.DrawImage(&image, xpos, ypos);
}
