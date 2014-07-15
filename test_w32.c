#include "mem_canvas.h"
#include "testdraw.h"

#define _WIN32_WINNT 0x0400
#define WINVER 0x0410
#include <windows.h>



static BITMAPINFO info;
static HBITMAP bitmap;
static HDC bitmapDC;
static HBRUSH bgbrush;
static canvas* cv;



LRESULT CALLBACK WindowProc( HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam )
{
    BLENDFUNCTION ftn;
    PAINTSTRUCT ps;
    HDC hDC;
    RECT r;

    switch( message )
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        ftn.BlendOp = AC_SRC_OVER;
        ftn.BlendFlags = 0;
        ftn.SourceConstantAlpha = 0xFF;
        ftn.AlphaFormat = AC_SRC_ALPHA;

        r.left = r.top = 0;
        r.right = 640;
        r.bottom = 480;

        testdraw( cv );

        hDC = BeginPaint( hWnd, &ps );
        FillRect( hDC, &r, bgbrush );
        AlphaBlend( hDC, 0, 0, 640, 480, bitmapDC,
                    0, 0, 640, 480, ftn );
        EndPaint( hWnd, &ps );
        return -1;
    }

    return DefWindowProc( hWnd, message, wParam, lParam );
}

int main( void )
{
    unsigned char* data;
    WNDCLASSEX wc;
    HWND hWnd;
    MSG msg;
    RECT r;

    /* register window class */
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "foo";

    RegisterClassEx( &wc );

    /* create window */
    SetRect( &r, 0, 0, 640, 480 );
    AdjustWindowRect( &r, WS_CAPTION | WS_SYSMENU, FALSE );
    hWnd = CreateWindowEx( 0, "foo", "W32 test", WS_CAPTION | WS_SYSMENU,
                           0, 0, r.right-r.left, r.bottom-r.top,
                           0, 0, wc.hInstance, 0 );

    /* craete bitmap */
    bitmapDC = CreateCompatibleDC( NULL );

    info.bmiHeader.biSize        = sizeof(info.bmiHeader);
    info.bmiHeader.biBitCount    = 32;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biPlanes      = 1;
    info.bmiHeader.biWidth       = 640;
    info.bmiHeader.biHeight      = -480;

    bitmap = CreateDIBSection( bitmapDC, &info, DIB_RGB_COLORS,
                               (void**)&data, 0, 0 );

    SelectObject( bitmapDC, bitmap );

    bgbrush = CreateSolidBrush( RGB(0,0,0) );

    /* create canvas */
    cv = canvas_memory_create_ext( data, 640, 480, 1 );

    /* main loop */
    ShowWindow( hWnd, SW_SHOWNORMAL );

    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    canvas_destroy( cv );

    return 0;
}

