
#include <windows.h>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftstroke.h>
#include <ftbitmap.h>


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


static std::vector<std::vector<FT_Vector>> pointsList;
static std::vector<int> contours;
FT_Face face;

void RefreshView(HWND hWnd, HDC hdc) {
	//GDIDrawDot(hdc,0, 0,RGB(255, 0, 0));
		int r = 0;
		int g = 0;
		int b = 0;
		for (int i = 0, color = 10; i < pointsList.size(); i++, color += 50) {
			//b += 100;
			if (b >= 255) {
				g += 200;
				b = 0;
			}
			else {
				if (g >= 255) {
					r += 200;
					g = 0;
				}
				else {
					b += 200;
				}
			}
			//DrawPolyTree(hdc, polytree);
			for (int j = 0; j < pointsList[i].size();j++) {
				//int fromIndex = j;
				int toIndex = (j+1)% pointsList[i].size();
				GDIDrawLine(hdc,pointsList[i][j].x, pointsList[i][j].y, pointsList[i][toIndex].x, pointsList[i][toIndex].y, RGB(r, g, b));
			}
		}
}


int main(int argc, const char* argv[]) {
	HWND hwnd = ::GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(NULL);
	ShowWindow(hwnd, 0);

	//initCircle();
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
	//if (face->glyph->outline.flags == FT_OUTLINE_EVEN_ODD_FILL) {
	if (face->glyph->format== FT_GLYPH_FORMAT_OUTLINE) {
		for (int i = 0, begin = 0; i < face->glyph->outline.n_contours; i++) {
			contours.push_back(face->glyph->outline.contours[i]);
			printf("%d  \n", contours[i]);
			
			std::vector<FT_Vector>* posV = new std::vector<FT_Vector>();
			for (int j = begin; j <= face->glyph->outline.contours[i]; j++) {
				
				FT_Vector poi = FT_Vector{ face->glyph->outline.points[j].x, face->glyph->outline.points[j].y };
				(*posV).push_back(poi);
			}
			pointsList.push_back((*posV));
			begin = face->glyph->outline.contours[i] + 1;
		}
	}


	wWinMain(instance, NULL, NULL, 1);

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	ShowWindow(hwnd, 1);
}