
#include <windows.h>

#define MAX_LOADSTRING 100

// �˴���ģ���а����ĺ�����ǰ������:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void RefreshView(HWND hWnd, HDC hdc);
// ȫ�ֱ���:
static HINSTANCE hInst;                                // ��ǰʵ��
static WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
static WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˴����ô��롣

	// ��ʼ��ȫ���ַ���
	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
	wsprintfW(szWindowClass, L"_12345");
	wsprintfW(szWindowClass, L"title");
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

	MSG msg;

	// ����Ϣѭ��:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor = NULL;// LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;// (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
	int cxScreen, cyScreen;
	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);
	int w = 400;
	int h = 300;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		cxScreen/2-w/2, cyScreen/2 - h/2, w, h, nullptr, nullptr, hInstance, nullptr);
	//SetTimer(hWnd, 1, 60, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	//CreateEGLContext(hWnd, &eglDisplay, &eglContext, &eglSurface);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//HPEN newPen;       // ���ڴ����»���   
	//HPEN *pOldPen;     // ���ڴ�žɻ���   
	//HBRUSH newBrush;   // ���ڴ����»�ˢ   
	//HBRUSH *pOldBrush; // ���ڴ�žɻ�ˢ   
	RECT rect;
	GetClientRect(hWnd, &rect);

	switch (message)
	{
	case WM_ERASEBKGND:
			return 1;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
			/*case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;*/
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
		/*if (eglSurface) {
			glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		}*/
		InvalidateRect(hWnd, NULL, true);
		
		//Invalidate(true);
		/*PostMessage(hWnd, WM_PAINT, 0, 0);
		UpdateWindow(hWnd);*/
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_TIMER:

		InvalidateRect(hWnd, NULL, true);
		//PostMessage(hWnd, WM_PAINT, 0, 0);
		//return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ˴�����ʹ�� hdc ���κλ�ͼ����...

		Rectangle(hdc, 0, 0, rect.right, rect.bottom);
		RefreshView(hWnd,hdc);
		
		EndPaint(hWnd, &ps);


	}
	break;
	

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
