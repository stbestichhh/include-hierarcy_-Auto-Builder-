// autoincludeHierarcy.cpp : Определяет точку входа для приложения.
//

#include "DirecotryChecker.h"
#include "autoincludeHierarcy.h"
#include "ImagePathClass.h"
#include <gdiplus.h>
#include <algorithm>
#include <windows.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

#define MAX_LOADSTRING 100


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
Image* image = nullptr;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

ImagePathClass imagePathClass;

void LoadImage() {
    image = nullptr;
    if (image == nullptr) {
        
        //std::string imagePath = "C:\\OOPLabs\\ConsoleApplication1\\output.png";
        std::string imagePath = imagePathClass.GetImagePath();
        std::wstring wideImagePath(imagePath.begin(), imagePath.end());

        image = new Image(wideImagePath.c_str());
        if (image->GetLastStatus() != Ok) {
            // Handle image loading error if needed
            delete image;
            image = nullptr;
        }
    }
}

int offsetX = 0;
int offsetY = 0;
float zoomFactor = 1.0f;
void DrawImage(HDC hdc) {
    if (image != nullptr) {
        Graphics graphics(hdc);
        graphics.ScaleTransform(zoomFactor, zoomFactor);
        graphics.DrawImage(image, -offsetX, -offsetY); // Adjust the coordinates as needed
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AUTOINCLUDEHIERARCY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTOINCLUDEHIERARCY));

    MSG msg;


    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTOINCLUDEHIERARCY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_AUTOINCLUDEHIERARCY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
DirecotryChecker dir;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:    
    {
        SetClassLongPtr(hWnd, GCL_STYLE, GetClassLongPtr(hWnd, GCL_STYLE) | CS_HREDRAW | CS_VREDRAW);
    }
    break;
    case WM_SETFOCUS:
        SetFocus(hWnd);
        return 0;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT:
            offsetX -= 10;
            break;
        case VK_RIGHT:
            offsetX += 10;
            break;
        case VK_UP:
            offsetY -= 10;
            break;
        case VK_DOWN:
            offsetY += 10;
            break;
        case VK_PRIOR:  // PageUp
            zoomFactor *= 1.1f;
            break;
        case VK_NEXT:   // PageDown
            zoomFactor /= 1.1f;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;


    case WM_COMMAND:
        {

            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
            case ID_OPEN_FILE:
                {
                    dir.ShowDirectoryFiles(hWnd);
                    LoadImage();
                    InvalidateRect(hWnd, nullptr, TRUE);
                }
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawImage(hdc);
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

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
