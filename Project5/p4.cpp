
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

#if 1
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

static std::vector<std::vector<FT_Vector*>*> pointsList;
std::vector<FT_Vector*> *fpolyline;
static std::vector<int> contours;
FT_Face face;



void RefreshView(HWND hWnd, HDC hdc) {
	//GDIDrawDot(hdc,0, 0,RGB(255, 0, 0));

	//DrawBitmap(hdc, face->glyph->bitmap);

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
		for (int j = 0; j < pointsList[i]->size(); j++) {
			//int fromIndex = j;
			int toIndex = (j + 1) % pointsList[i]->size();
			std::vector<FT_Vector *> plist = *(pointsList[i]);
			GDIDrawLine(hdc, ((plist)[j])->x, plist[j]->y, plist[toIndex]->x, plist[toIndex]->y, RGB(r, g, b));
		}
	}
}

static FT_Vector Bezier(float t, FT_Vector p0, FT_Vector p1, FT_Vector p2) {
	FT_Vector  point;// = new FT_Vector();
	float temp = 1 - t;
	(point).x = temp * temp * p0.x + 2 * t * temp * p1.x + t * t * p2.x;
	(point).y = temp * temp * p0.y + 2 * t * temp * p1.y + t * t * p2.y;
	return point;
}


static FT_Vector BezierN(float t, std::vector<FT_Vector>&p, int n, int position) {
	if (n == 1) {
		FT_Vector  point;// = new FT_Vector();
		point.x = ((p[position].x * (1 - t)) + (p[position + 1].x * t));
		point.y = ((p[position].y * (1 - t)) + (p[position + 1].y * t));
		return point;
	}
	else {
		FT_Vector p1 = BezierN(t, p, n - 1, position);
		FT_Vector p2 = BezierN(t, p, n - 1, position + 1);
		//IntPoint tmp1 = (p1) * (1 - t);
		FT_Vector p3;
		p3.x= (p1.x) * (1 - t) + (p2.x)* t;
		p3.y = (p1.y) * (1 - t) + (p2.y)* t;
		return p3;
	}

}


int fcount = 0;
FT_Vector fCurrentPoint;
int moveto(const FT_Vector* to, void* user)
{
	printf("M %d %d\n", to->x, to->y);
	fCurrentPoint.x = to->x;
	fCurrentPoint.y = to->y;
	fpolyline = new std::vector<FT_Vector*>();
	fpolyline->push_back(new FT_Vector{ to->x,to->y });
	pointsList.push_back((fpolyline));
	fcount += 1;
	return 0;
}

int lineto(const FT_Vector* to, void* user)
{
	printf("L %d %d\n", to->x, to->y);
	fCurrentPoint.x = to->x;
	fCurrentPoint.y = to->y;
	fpolyline->push_back(new FT_Vector{ to->x,to->y });
	fcount += 1;
	return 0;
}


int conicto(const FT_Vector* control,
	const FT_Vector* to,
	void* user)
{
	printf("Q %d %d %d %d\n", control->x, control->y, to->x, to->y);

	/*glm::vec3 previous(fCurrentPoint.x, fCurrentPoint.y, 0);
	glm::vec3 current(to->x, to->y, 0);
	glm::vec3 contrl(control->x, control->y, 0);
	glm::vec3 vec = getBezierPointF(0.5, previous, current, contrl);*/

	FT_Vector vec = Bezier(0.5, fCurrentPoint, (*control),*to);

	(*fpolyline).push_back(new FT_Vector{ vec.x,vec.y });
	(*fpolyline).push_back(new FT_Vector{ to->x,to->y });


	fCurrentPoint.x = to->x;
	fCurrentPoint.y = to->y;

	fcount += 2;
	return 0;
}

int cubicto(const FT_Vector*  control1,
	const FT_Vector*  control2,
	const FT_Vector*  to,
	void*             user)
{
	std::vector<FT_Vector> control;
	control.push_back(fCurrentPoint);
	control.push_back(*control1);
	control.push_back(*control2);
	control.push_back(*to);
	FT_Vector p1 = BezierN(0.2,control,3, 0);
	FT_Vector p2 = BezierN(0.5, control, 3,0);
	FT_Vector p3 = BezierN(0.7, control, 3,0);

	(*fpolyline).push_back(new FT_Vector{ p1.x,p1.y });
	(*fpolyline).push_back(new FT_Vector{ p2.x,p2.y });
	(*fpolyline).push_back(new FT_Vector{ p3.x,p3.y });
	(*fpolyline).push_back(new FT_Vector{ to->x, to->y });
	printf("C %d %d %d %d %d %d\n", control1->x, control1->y, control2->x, control2->y, to->x, to->y);
	fcount += 3;
	return 0;
}

void ExtractOutline(FT_Outline outline)
{
	FT_Outline_Funcs callbacks;

	callbacks.move_to = moveto;
	callbacks.line_to = lineto;
	callbacks.conic_to = conicto;
	callbacks.cubic_to = cubicto;
	callbacks.shift = 0;
	callbacks.delta = 0;


	printf("ExtractOutline \n");
	fcount = 0;
	FT_Error error = FT_Outline_Decompose(&outline, &callbacks, &outline);
	if (error) {
		printf("Couldn't extract the outline: FT_Outline_Decompose() failed");
	}
}


int main(int argc, const char* argv[]) {
	HWND hwnd = ::GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(NULL);
	ShowWindow(hwnd, 0);

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

		ExtractOutline(face->glyph->outline);
	}

	//FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);


	wWinMain(instance, NULL, NULL, 1);

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	ShowWindow(hwnd, 1);
}