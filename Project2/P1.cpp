#include "windows.h"
#include <vector>

typedef struct  IntPoint {
	int x;
	int y;
	int r;
	IntPoint operator*(float t) {
		IntPoint tmp;// = new circlePoint();
		tmp.x = (int)(this->x * t);
		tmp.y = (int)(this->y * t);
		return tmp;
	}
	IntPoint operator+(IntPoint & p) {
		IntPoint *tmp = new IntPoint();
		tmp->x = this->x + p.x;
		tmp->y = this->y + p.y;
		return *tmp;
	}
	IntPoint operator+(IntPoint && p) {
		IntPoint tmp;// = new circlePoint();
		tmp.x = this->x + p.x;
		tmp.y = this->y + p.y;
		return tmp;
	}
}IntPoint;

static IntPoint bigCircle;

static IntPoint smallCircle;
static IntPoint* controlCircle;

static IntPoint bigCircle1;
static IntPoint bigCircle2;
static IntPoint smallCircle1;
static IntPoint smallCircle2;




/**
  * B(t) = (1 - t)^2 * P0 + 2t * (1 - t) * P1 + t^2 * P2, t ∈ [0,1]
  *
  * @param t  曲线长度比例
  * @param p0 起始点
  * @param p1 控制点
  * @param p2 终止点
  * @return t对应的点
  */
static IntPoint* Bezier(float t, IntPoint p0, IntPoint p1, IntPoint p2) {
	IntPoint * point = new IntPoint();
	float temp = 1 - t;
	(*point).x = temp * temp * p0.x + 2 * t * temp * p1.x + t * t * p2.x;
	(*point).y = temp * temp * p0.y + 2 * t * temp * p1.y + t * t * p2.y;
	return point;
}


static IntPoint BezierN(float t, std::vector<IntPoint>&p, int n, int position) {
	if (n == 1) {
		return ((p[position] * (1 - t)) + (p[position + 1] * t));
	}
	else {
		IntPoint p1 = BezierN(t, p, n - 1, position);
		IntPoint p2 = BezierN(t, p, n - 1, position + 1);
		//IntPoint tmp1 = (p1) * (1 - t);
		IntPoint p3 = (p1) * (1 - t) + (p2)* t;
		return p3;
	}

}

static IntPoint *GetControl(float t, IntPoint &bigCircle, IntPoint &smallCircle) {
	IntPoint * point = new IntPoint();
	(*point).x = bigCircle.x*t + (1 - t)*smallCircle.x;
	(*point).y = bigCircle.y*t + (1 - t)*smallCircle.y;
	return point;
}


void initCircle() {

	int ox = 100;
	int oy = 100;

	int r1 = 50;
	int r2 = 50;
	bigCircle = IntPoint{ ox,oy ,50 };
	bigCircle1 = IntPoint{ ox + r1,oy };
	bigCircle2 = IntPoint{ ox - r1,oy };
	smallCircle = IntPoint{ ox,oy ,50 };
	smallCircle1 = IntPoint{ ox + r2,oy };
	smallCircle2 = IntPoint{ ox - r2,oy };

	controlCircle = GetControl(0.5, bigCircle, smallCircle);
}

void updateCircle(float t) {
	int ox = 100;
	int oy = 100;
	//r2 = r2 * t;
	int r1 = 50;
	int r2 = 50;
	int max = 200;

	smallCircle1 = IntPoint{ ox + (int)(r2*(1 - t)),oy + (int)(t * max) };
	smallCircle2 = IntPoint{ ox - (int)(r2*(1 - t)),oy + (int)(t * max) };
	smallCircle = IntPoint{ ox,oy + (int)(t * max) ,(int)(r2*(1 - t)) };
	controlCircle = GetControl(0.5, bigCircle, smallCircle);
}



static float ssdfasdf = 0;
void draw(HDC hdc) {
	POINT		apt[202];
	HGDIOBJ	oldBrush= SelectObject(hdc, GetStockObject(GRAY_BRUSH));

	//GDIDrawCircle(hdc, bigCircle.x, bigCircle.y, RGB(255, 0, 0), bigCircle.r);
	//GDIDrawCircle(hdc, smallCircle.x, smallCircle.y, RGB(255, 0, 0), smallCircle.r);

	Pie(hdc, bigCircle.x - bigCircle.r, bigCircle.y - bigCircle.r, bigCircle.x + bigCircle.r, bigCircle.y + bigCircle.r, bigCircle.x + bigCircle.r, bigCircle.y, bigCircle.x - bigCircle.r, bigCircle.y);
	Pie(hdc, smallCircle.x - smallCircle.r, smallCircle.y - smallCircle.r, smallCircle.x + smallCircle.r, smallCircle.y + smallCircle.r, smallCircle.x - smallCircle.r, smallCircle.y, smallCircle.x + smallCircle.r, smallCircle.y);
	
	
	IntPoint pre1 = bigCircle1;
	for (int i = 0; i <= 100; i++) {
		IntPoint* next = Bezier(0.01*i, bigCircle1, *controlCircle, smallCircle1);
		//GDIDrawLine2(hdc, pre1.x, pre1.y, next->x, next->y, RGB(255, 0, 0, 0));
		pre1 = *next;
		apt[i].x = next->x;
		apt[i].y = next->y;
	}

	IntPoint pre2 = bigCircle2;
	for (int i = 0; i <=100; i++) {
		std::vector<IntPoint> vec;
		vec.push_back(bigCircle2);
		vec.push_back(*controlCircle);
		vec.push_back(smallCircle2);
		IntPoint* next = &BezierN(0.01f*(100-i), vec, 2, 0); //Bezier(0.01*i, bigCircle2, *controlCircle, smallCircle2);
		//GDIDrawLine2(hdc, pre2.x, pre2.y, next->x, next->y, RGB(255, 0, 255, 0));
		pre2 = *next;

		apt[101+i].x = next->x;
		apt[101+i].y = next->y;
	}

	apt[201 ].x = bigCircle2.x;
	apt[201 ].y = bigCircle2.y-1;


	SetPolyFillMode(hdc, ALTERNATE);

	Polygon(hdc, apt, 200);


	ssdfasdf += 0.01f;
	if (ssdfasdf >= 1) {
		ssdfasdf = 0;
	}

	updateCircle(ssdfasdf);


	SelectObject(hdc, oldBrush);
}

void RefreshView(HWND hWnd, HDC hdc) {
	
	draw(hdc);
}


int main(int argc, const char* argv[]) {
	HWND hwnd = ::GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(NULL);
	ShowWindow(hwnd, 0);

	initCircle();
	
	wWinMain(instance, NULL, NULL, 1);
	ShowWindow(hwnd, 1);
}