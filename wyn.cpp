#include <Windows.h>
#include "wyn.h"

#define wyn_alloc(type) (type*)malloc(sizeof(type))
#define wyn_free(type, ptr) free((type*)ptr); ptr = 0;
#ifdef PLTFRM_WIN32

LRESULT CALLBACK def_win_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //KeyEvent event;
	//Window* win = Window::stGetWindow((winHandle)hwnd);
	switch (uMsg)
	{
		//Inputs------------
		case WM_KEYDOWN:
//			event.key		= (Key)wParam;
//			event.eventType = NWIN_KeyPressed;
//			win->_getKeyboard().record(event);
			break;

		case WM_KEYUP:
//			event.key = (Key)wParam;
//			event.eventType = NWIN_KeyReleased;
//			win->_getKeyboard().record(event);
			break;

		case WM_LBUTTONDOWN:
//			event.key = NWIN_KEY_LBUTTON;
//			event.eventType = NWIN_KeyPressed;
//			win->_getKeyboard().record(event);
			break;

		case WM_LBUTTONUP:
//			event.key = NWIN_KEY_LBUTTON;
//			event.eventType = NWIN_KeyReleased;
//			win->_getKeyboard().record(event);
			break;

		case WM_RBUTTONDOWN:
//			event.key = NWIN_KEY_RBUTTON;
//			event.eventType = NWIN_KeyPressed;
//			win->_getKeyboard().record(event);
			break;

		case WM_RBUTTONUP:
//			event.key = NWIN_KEY_RBUTTON;
//			event.eventType = NWIN_KeyReleased;
//			win->_getKeyboard().record(event);
			break;
		//-------------------

		case WM_CREATE: 
		{
			RECT rcClient;
			GetWindowRect(hwnd, &rcClient);  
			SetWindowPos(hwnd, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom -rcClient.top, SWP_FRAMECHANGED);
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
//			if (win != nullptr)
//				ReleaseDC((HWND)win->_getHandle(), hdc);
			return 0;
		}
		case WM_SIZE: {
//			if (win == nullptr) return 0;
//			NWIN_CALL_CALL_BACK(win->resizeCallback, (winHandle)hwnd, {LOWORD(lParam), HIWORD(lParam)});
//  		NWIN_CALL_CALL_BACK(win->drawCallback, (winHandle)hwnd);
			return 0;
		};
		case WM_DESTROY: {
//			Window::stShouldNotUpdate(hwnd);
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

typedef struct {
    HINSTANCE inst;
    MSG       msg;
    HDC       hdc;
    DWORD     stl;
} wyn_win32_data;

int wyn_win32_create(wyndow* w, wyn_crt_info* crt_info) {
    static char n[16] = {'w','y','n',1,0};
    static WNDCLASS  wc{};
    ++n[3];
    HINSTANCE module; 
    RECT      win_rect; 
    HANDLE    handle;
    handle  = NULL;
    module  = GetModuleHandle(0);
    wc.lpszClassName = n;
    wc.lpfnWndProc   = def_win_proc;   
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance     = module;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);  
    RegisterClass(&wc);

    Word ex_stl = 0x00040000L;
    Word stl    = 0x00000000L | 0x00C00000L | 
                  0x00080000L | 0x00040000L | 
                  0x00020000L | 0x00010000L;
    handle      = CreateWindowEx(ex_stl, n, (const char*)crt_info->desc, 
                   stl, crt_info->rect.pos.x, crt_info->rect.pos.y, 
                   crt_info->rect.size.x, crt_info->rect.size.y, 0, 0, module,0); 
    w->handle = handle; 
    if (!handle) 
        return 1;
    
    wyn_win32_data* dat = wyn_alloc(wyn_win32_data);
    dat->stl = stl;
    dat->hdc = GetDC((HWND)handle);
    dat->msg = {};
    w->extra = dat;
    ShowWindow((HWND)handle, SW_SHOWDEFAULT);
    return 0;
}

int wyn_win32_destroy(wyndow* w) {
    int res = DestroyWindow((HWND)w->handle);
    wyn_free(wyn_win32_data, w->extra);
    return res;
}
#endif

#ifdef PLTFRM_LINUX
int  wyn_linux_create(wyndow* w);
int  wyn_linux_destroy();
#endif

int wyn_create(wyndow* w, wyn_crt_info* inf) {
#ifdef PLTFRM_WIN32
    return wyn_win32_create(w, inf);
#endif
#ifdef PLTFRM_LINUX
    return wyn_linux_create(w, inf);
#endif
}

int wyn_destroy(wyndow* w) {
#ifdef PLTFRM_WIN32
    return wyn_win32_destroy(w);
#endif
#ifdef PLTFRM_LINUX
    return wyn_win32_destroy(w);
#endif
}

int main() {
    wyndow       w;
    wyn_crt_info inf;
    inf.desc     = (char*)"HelloWorld";
    inf.rect.pos = {};
    inf.rect.size= {.x = 200,.y=200};
    wyn_create(&w, &inf);
    while (1) {};
    wyn_destroy(&w);
}
