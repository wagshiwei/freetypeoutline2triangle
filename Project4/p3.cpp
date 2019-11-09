
#include <windows.h>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftstroke.h>
#include <ftbitmap.h>

#ifdef GDIPLUS
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
#endif

#if 0
void GDIDrawLine(HDC hdc, int x, int y, int x2, int y2, COLORREF color) {
	float scale = 0.05;
	y += 400;
	y2 += 400;
	x = (x*scale);
	y = (y*scale);
	x2 = (x2*scale);
	y2 = (y2*scale);

	HPEN hPen, hPenOld;
	hPen = CreatePen(PS_SOLID, 3, color);
	hPenOld = (HPEN)::SelectObject(hdc, hPen); //往DC放画笔对象
	//::MoveToEx(hdc, x, y, NULL);
	/*Ellipse*/(hdc, x, y, x + 5, y + 5);
	MoveToEx(hdc, x, y, NULL);
	LineTo(hdc, x2, y2);
	::SelectObject(hdc, hPenOld); //这是个好习惯
	::DeleteObject(hPen); //释放画笔
}
#endif

static std::vector<std::vector<FT_Vector>> pointsList;
static std::vector<int> contours;
FT_Face face;



void DrawBitmap(HDC hdc, FT_Bitmap& bitmap) {

	BITMAPINFO  PBMINOW;
	memset(&PBMINOW, 0, sizeof(BITMAPINFO));
	//BITMAPFILEHEADER Header;
	//PBMINOW.bmiHeader.biBitCount;
	//PBMINOW.bmiHeader.biHeight;
	//PBMINOW.bmiHeader.biPlanes;
	//PBMINOW.bmiHeader.biSize;
	//PBMINOW.bmiHeader.biWidth;
	//PBMINOW.bmiColors
	PBMINOW.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	PBMINOW.bmiHeader.biWidth = bitmap.width;
	PBMINOW.bmiHeader.biHeight = bitmap.rows;
	PBMINOW.bmiHeader.biPlanes = 1;
	PBMINOW.bmiHeader.biBitCount = 32;
	PBMINOW.bmiHeader.biCompression = BI_RGB;
	PBMINOW.bmiHeader.biSizeImage = (bitmap.width) * bitmap.rows*(PBMINOW.bmiHeader.biBitCount / 8);
	PBMINOW.bmiHeader.biXPelsPerMeter = 0;
	PBMINOW.bmiHeader.biYPelsPerMeter = 0;
	PBMINOW.bmiHeader.biClrUsed = 0;
	PBMINOW.bmiHeader.biClrImportant = 0;
	int count = bitmap.width*bitmap.rows * sizeof(int);

	//CreateDIBSection
	unsigned char *pvBits;// = new  unsigned char[count];
	//HBITMAP hbitmap = CreateDIBSection(NULL, &PBMINOW, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);
	/*BITMAP bmp;
	HBITMAP hbitmap = CreateBitmapIndirect(&bmp);*/
#if 1
	pvBits = new  unsigned char[count];
	int pitch = bitmap.width;
	unsigned int* intByte = (unsigned int *)pvBits;
	for (int i = 0; i < bitmap.rows; i++) {
		for (int j = 0; j < bitmap.width; j++) {
			unsigned char c = bitmap.buffer[i*bitmap.width + j];
			//*(pvBits+i*bitmap.width + j) = c;
			//memcpy(pvBits + i * pitch, bitmap.buffer + i*pitch, pitch);
			unsigned int cc = (((unsigned char)0xff << 24) | (c << 16) | (c << 8) | c);
			int index = i * bitmap.width + j;
			intByte[index] = cc;
			//pvBits[i * bitmap.width + j]
				//unsigned int* intByte = (unsigned int *)(pvBits+ index*4);
				//intByte[index] = cc;
		}
	}
#endif
	HBITMAP hbitmap = CreateDIBitmap(hdc, (BITMAPINFOHEADER*)&(PBMINOW.bmiHeader), CBM_INIT, pvBits, &PBMINOW, DIB_RGB_COLORS);


#ifdef GDIPLUS
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Bitmap * bia = Gdiplus::Bitmap::FromHBITMAP(hbitmap, NULL);
	graphics.DrawImage(bia, Gdiplus::Point(0, 0));

#else
	HDC memDC = CreateCompatibleDC(hdc);
	HGDIOBJ  pOldBitmap = SelectObject(memDC, hbitmap);
	//BITMAP bmp;
	//SetBitmapBits(hbitmap, bitmap.pitch*bitmap.rows,bitmap.buffer);
	//bitmap.GetBitmap(&bmp);
	StretchBlt(hdc, 10, bitmap.rows, bitmap.width, -(signed)bitmap.rows,
		memDC, 0, 0, bitmap.width, bitmap.rows, SRCCOPY);
	SelectObject(memDC, pOldBitmap);
#endif


	//delete pvBits;
}

void RefreshView(HWND hWnd, HDC hdc) {
	//GDIDrawDot(hdc,0, 0,RGB(255, 0, 0));

	

	DrawBitmap(hdc, face->glyph->bitmap);

}


int main(int argc, const char* argv[]) {
	HWND hwnd = ::GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(NULL);
	ShowWindow(hwnd, 0);

#ifdef GDIPLUS
	//initCircle();
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
#endif

	FT_Library library;                         //库的句柄

	int error = FT_Init_FreeType(&library);
	if (error)
	{
		//初始化失败
	}

	error = FT_New_Face(library,
		"../msyh.ttc",    //字形文件
		0,
		&face);

	//error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0);
	error = FT_Set_Pixel_Sizes(face, 0, 40);

	FT_ULong code;
	unsigned char *p = (unsigned char*)&code;
	p[0] = 0x84; //DE 56  84 76
	p[1] = 0x76;
	p[2] = 0;
	p[3] = 0;
	//p[2] = FF FE 11 62;
	//p[3] = FF FE 11 62;
	int glyph_index = FT_Get_Char_Index(face, code);

	error = FT_Load_Glyph(
		face, /* face对象的句柄 */
		glyph_index, /* 字形索引 */
		FT_LOAD_DEFAULT);

	if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {

	}

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);


	wWinMain(instance, NULL, NULL, 1);

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	ShowWindow(hwnd, 1);
}