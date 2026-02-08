#include "wyn.h"

#define wyn_alloc(type) (type*)malloc(sizeof(type))
#define wyn_free(type, ptr)                                                                                                      \
	free((type*)ptr);                                                                                                            \
	ptr = 0;
#ifdef PLTFRM_WIN32

#include <Windows.h>

LRESULT CALLBACK def_win_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// KeyEvent event;
	// Window* win = Window::stGetWindow((winHandle)hwnd);
	switch(uMsg) {
	// Inputs------------
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

	case WM_CREATE: {
		RECT rcClient;
		GetWindowRect(hwnd, &rcClient);
		SetWindowPos(hwnd, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
					 SWP_FRAMECHANGED);
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC			hdc = BeginPaint(hwnd, &ps);
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
	MSG		  msg;
	HDC		  hdc;
	DWORD	  stl;
} wyn_win32_data;

int wyn_win32_create(wyndow* w, wyn_crt_info* crt_info) {
	static char		n[16] = {'w', 'y', 'n', 1, 0};
	static WNDCLASS wc{};
	++n[3];
	HINSTANCE module;
	RECT	  win_rect;
	HANDLE	  handle;
	handle			 = NULL;
	module			 = GetModuleHandle(0);
	wc.lpszClassName = n;
	wc.lpfnWndProc	 = def_win_proc;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance	 = module;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	Word ex_stl = 0x00040000L;
	Word stl	= 0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L;
	handle		= CreateWindowEx(ex_stl, n, (const char*)crt_info->desc, stl, crt_info->rect.pos.x, crt_info->rect.pos.y,
								 crt_info->rect.size.x, crt_info->rect.size.y, 0, 0, module, 0);
	w->handle	= handle;
	if(!handle)
		return 1;

	wyn_win32_data* dat = wyn_alloc(wyn_win32_data);
	dat->stl			= stl;
	dat->hdc			= GetDC((HWND)handle);
	dat->msg			= {};
	w->extra			= dat;
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
#include "stdlib.h"
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#ifdef __cplusplus
#include <cstring.h>
#else
#include <string.h>
#endif

typedef GLXContext (*PFN_glXCreateContextAttribsARB)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

#define LNX_CHECK(val, msg)                                                                                                      \
	if(!val) {                                                                                                                   \
		printf("%s", msg);                                                                                                       \
		return (-1);                                                                                                             \
	}
typedef struct {
	Display*	dsp;
	GLXWindow	glx_win;
	GLXFBConfig fb_conf;
	wyn_rect	last_metrics;
} lnx_wyn_data;
#define get_lnx(w) ((lnx_wyn_data*)((w)->extra))

#define lnx_call(func, proc, ...)                                                                                                \
	if(func)                                                                                                                     \
	((proc)(func))(__VA_ARGS__)

static int fbAttribs[] = {GLX_X_RENDERABLE,
						  True,
						  GLX_DRAWABLE_TYPE,
						  GLX_WINDOW_BIT,
						  GLX_RENDER_TYPE,
						  GLX_RGBA_BIT,
						  GLX_X_VISUAL_TYPE,
						  GLX_TRUE_COLOR,
						  GLX_RED_SIZE,
						  8,
						  GLX_GREEN_SIZE,
						  8,
						  GLX_BLUE_SIZE,
						  8,
						  GLX_ALPHA_SIZE,
						  8,
						  GLX_DEPTH_SIZE,
						  24,
						  GLX_STENCIL_SIZE,
						  8,
						  GLX_DOUBLEBUFFER,
						  True,
						  None};

int wyn_linux_show(wyndow* w, bool flag) {
	lnx_wyn_data* lnx = get_lnx(w);
	if(flag) {
		XMapWindow(lnx->dsp, w->handle);
		return 0;
	}
	XUnmapWindow(lnx->dsp, w->handle);
	return 0;
}

int wyn_linux_set_title(wyndow* w, const char* desc) {
	lnx_wyn_data* lnx = get_lnx(w);
	return XStoreName(lnx->dsp, w->handle, desc);
}

int wyn_linux_set_pos(wyndow* w, wyn_vec2 p) { return XMoveWindow(get_lnx(w)->dsp, w->handle, p.x, p.y); }

int wyn_linux_create(wyndow* w, wyn_crt_info* crt_inf) {
	lnx_wyn_data*		 dat = wyn_alloc(lnx_wyn_data);
	int					 root;
	Window				 win;
	XVisualInfo*		 vi;
	GLXFBConfig*		 fbConfings;
	Colormap			 cmap;
	XSetWindowAttributes swa;
	XEvent				 xev;
	int					 count;
	dat->dsp = XOpenDisplay(NULL);
	LNX_CHECK(dat->dsp, "Display Openning Failed");
	root	   = DefaultRootWindow(dat->dsp);
	fbConfings = glXChooseFBConfig(dat->dsp, DefaultScreen(dat->dsp), fbAttribs, &count);
	LNX_CHECK(fbConfings, "FBConf not found");
	dat->fb_conf = fbConfings[0];
	XFree(fbConfings);
	vi = glXGetVisualFromFBConfig(dat->dsp, dat->fb_conf);
	LNX_CHECK(vi, "Get Visual failed");
	cmap		   = XCreateColormap(dat->dsp, root, vi->visual, AllocNone);
	swa.colormap   = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask | KeyReleaseMask;
	win = XCreateWindow(dat->dsp, root, crt_inf->rect.pos.x, crt_inf->rect.pos.y, crt_inf->rect.size.x, crt_inf->rect.size.y, 0,
						vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	dat->glx_win = glXCreateWindow(dat->dsp, dat->fb_conf, win, 0);
	Bool tmp;
	XkbSetDetectableAutoRepeat(dat->dsp, True, &tmp);
	XSetWindowBackground(dat->dsp, win, 0x0);
	LNX_CHECK(dat->glx_win, "Failed to create glx window");
	memcpy(&dat->last_metrics, &crt_inf->rect, sizeof(crt_inf->rect));
	w->handle			  = win;
	w->extra			  = dat;
	w->state.should_close = 0;
	memset(w->_callbacks, 0, sizeof(w->_callbacks));
	memset(w->_callbakcs_usr_data, 0, sizeof(w->_callbakcs_usr_data));
	memset(w->keyboard.events, 0, sizeof(w->keyboard.events));
	memset(w->keyboard.update_list, 0, sizeof(w->keyboard.update_list));
	w->keyboard.update_idx = -1;
	wyn_linux_show(w, 1);
	wyn_linux_set_title(w, crt_inf->desc);
	return 0;
}

int wyn_linux_kb_record(wyn_kboard* kb, KeySym c, int event) {
	int* v = 0;
	if(c > 255 || c < 0)
		return 1;
	v = &kb->events[c];
	if(event & Wyn_OnKeyRelease) {
		*v &= ~Wyn_OnKeyPressLock;
		*v &= ~Wyn_KeyPressed;
		*v |= Wyn_OnKeyRelease;
	}
	if(event & Wyn_KeyPressed) {
		*v |= Wyn_KeyPressed;
		if(!(*v & Wyn_OnKeyPressLock))
			*v |= Wyn_OnKeyPress;
		*v |= Wyn_OnKeyPressLock;
	}
	*v = *v | (event & Wyn_LastUpdate);
	kb->update_idx++;
	kb->update_list[kb->update_idx] = c;
	return 0;
}

int wyn_linux_update(wyndow* w) {
	lnx_wyn_data* lnx = get_lnx(w);
	int			  k;
	while(w->keyboard.update_idx != -1) {
		k = w->keyboard.update_list[w->keyboard.update_idx];
		w->keyboard.events[k] &= ~Wyn_OnKeyPress;
		w->keyboard.events[k] &= ~Wyn_OnKeyRelease;
		w->keyboard.update_idx--;
	}
	while(XPending(lnx->dsp) > 0) {
		XWindowAttributes gwa;
		XEvent			  event = {0};
		XNextEvent(lnx->dsp, &event);
		switch(event.type) {
		case Expose: {
			XWindowAttributes attribs;
			wyn_vec2		  s;
			XGetWindowAttributes(lnx->dsp, w->handle, &attribs);
			break;
		}
		case ConfigureNotify: {
			XConfigureEvent ev = event.xconfigure;
			wyn_vec2		s;
			s.x = ev.width;
			s.y = ev.height;
			if(s.x != lnx->last_metrics.size.x || s.y != lnx->last_metrics.size.y) {
				lnx_call(w->_callbacks[WYN_RESIZE_CBK_IDX], wyn_rz_callback_proc, w, s,
						 w->_callbakcs_usr_data[WYN_RESIZE_CBK_IDX]);
			}
			lnx->last_metrics.pos.x	 = ev.x;
			lnx->last_metrics.pos.y	 = ev.y;
			lnx->last_metrics.size.x = ev.width;
			lnx->last_metrics.size.y = ev.height;
			break;
		}
		case KeyPress: {
			KeySym k = XLookupKeysym(&event.xkey, 0);
			wyn_linux_kb_record(&w->keyboard, k, Wyn_KeyPressed);
			break;
		}
		case KeyRelease: {
			KeySym k = XLookupKeysym(&event.xkey, 0);
			wyn_linux_kb_record(&w->keyboard, k, Wyn_OnKeyRelease);
			break;
		}
		default:
			break;
		}
	}
	return 0;
}

int wyn_linux_swap(wyndow* w) {
	lnx_wyn_data* lnx = get_lnx(w);
	glXSwapBuffers(lnx->dsp, lnx->glx_win);
	return 0;
}

int wyn_linux_destroy(wyndow* win) {
	lnx_wyn_data* data = (lnx_wyn_data*)win->extra;
	glXDestroyWindow(data->dsp, data->glx_win);
	XDestroyWindow(data->dsp, (Window)win->handle);
	XCloseDisplay(data->dsp);
	wyn_free(lnx_wyn_data, win->extra);
	win->extra	= 0;
	win->handle = 0;
	return 0;
}

static int ctx_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,
							3,
							GLX_CONTEXT_MINOR_VERSION_ARB,
							2,
							GLX_CONTEXT_PROFILE_MASK_ARB,
							GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
							GLX_CONTEXT_FLAGS_ARB,
							GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
							None};

int wyn_linux_glctx_create(wyndow* w, wyn_glctx* glc, wyn_glctx_crt_info* crt_info) {
	lnx_wyn_data*				   lnx = get_lnx(w);
	PFN_glXCreateContextAttribsARB glXCreateContextAttribsARB =
		(PFN_glXCreateContextAttribsARB)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	ctx_attribs[1] = crt_info->major;
	ctx_attribs[3] = crt_info->minor;
	ctx_attribs[5] =
		crt_info->compatibilityProfile ? GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
	glc->handle = glXCreateContextAttribsARB(lnx->dsp, lnx->fb_conf, 0, 1, ctx_attribs);
	LNX_CHECK(glc, "Failed to create gl context");
	glXMakeContextCurrent(lnx->dsp, lnx->glx_win, lnx->glx_win, glc->handle);
	glc->owner = w;
	return 0;
}

void wyn_linux_glctx_destroy(wyn_glctx* glc) {
	lnx_wyn_data* lnx = get_lnx(glc->owner);
	glXMakeContextCurrent(lnx->dsp, lnx->glx_win, lnx->glx_win, 0);
	glXDestroyContext(lnx->dsp, glc->handle);
	glc->handle = 0;
}

void wyn_linux_glctx_make_current(wyndow* w, wyn_glctx* glc) {
	lnx_wyn_data* lnx = get_lnx(glc->owner);
	if(!glc) {
		glXMakeContextCurrent(lnx->dsp, lnx->glx_win, lnx->glx_win, 0);
		return;
	}
	glXMakeContextCurrent(lnx->dsp, lnx->glx_win, lnx->glx_win, glc->handle);
	glc->handle = 0;
	glc->owner	= 0;
}

bool wyn_linux_key_pressed(wyndow* w, int key) { return w->keyboard.events[key] & Wyn_KeyPressed; }

bool wyn_linux_on_key_press(wyndow* w, int key) { return w->keyboard.events[key] & Wyn_OnKeyPress; }

bool wyn_linux_on_key_release(wyndow* w, int key) { return w->keyboard.events[key] & Wyn_OnKeyRelease; }

#endif

#ifdef PLTFRM_WIN32
int (*wyn_create)(wyndow*, wyn_crt_info*)	 = wyn_win32_create;
int (*wyn_destroy)(wyndow* w, wyn_crt_info*) = wyn_win32_destroy;
int (*wyn_show)(wyndow*, bool)				 = wyn_win32_show;
#endif
#ifdef PLTFRM_LINUX
int (*wyn_create)(wyndow*, wyn_crt_info*)						  = wyn_linux_create;
int (*wyn_destroy)(wyndow* w)									  = wyn_linux_destroy;
int (*wyn_show)(wyndow*, bool)									  = wyn_linux_show;
int (*wyn_set_title)(wyndow*, const char*)						  = wyn_linux_set_title;
int (*wyn_update)(wyndow* w)									  = wyn_linux_update;
int (*wyn_swap)(wyndow* w)										  = wyn_linux_swap;
int (*wyn_glctx_create)(wyndow*, wyn_glctx*, wyn_glctx_crt_info*) = wyn_linux_glctx_create;
void (*wyn_glctx_make_current)(wyndow* w, wyn_glctx* glc)		  = wyn_linux_glctx_make_current;
void (*wyn_glctx_destroy)(wyn_glctx* glc)						  = wyn_linux_glctx_destroy;
bool (*wyn_key_pressed)(wyndow* w, int key)						  = wyn_linux_key_pressed;
bool (*wyn_on_key_press)(wyndow* w, int key)					  = wyn_linux_on_key_press;
bool (*wyn_on_key_release)(wyndow* w, int key)					  = wyn_linux_on_key_release;
#endif

void wyn_rz_cbk_reg(wyndow* w, wyn_rz_callback_proc proc, void* usr_data) {
	w->_callbacks[WYN_RESIZE_CBK_IDX]		   = proc;
	w->_callbakcs_usr_data[WYN_RESIZE_CBK_IDX] = usr_data;
}

void resize(wyndow* w, wyn_vec2 s, void* usr_data) {
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	// printf("Resize Size: %d, %d\n", s.x, s.y);
	// fflush(stdout);
}

int main() {
	wyndow			   w;
	wyn_crt_info	   inf;
	wyn_glctx_crt_info gl_inf;
	wyn_glctx		   glc;
	inf.desc					= (char*)"X11Window!";
	inf.rect.pos.x				= 0;
	inf.rect.pos.y				= 0;
	inf.rect.size.x				= 400;
	inf.rect.size.y				= 400;
	gl_inf.major				= 3;
	gl_inf.minor				= 2;
	gl_inf.compatibilityProfile = 0;
	wyn_create(&w, &inf);
	wyn_glctx_create(&w, &glc, &gl_inf);
	wyn_rz_cbk_reg(&w, resize, 0);
	resize(&w, inf.rect.pos, 0);
	while(!w.state.should_close) {
		if(w.keyboard.events['a'] & Wyn_OnKeyPress) {
			printf("OnKeyPress\n");
			fflush(stdout);
		}
		if(w.keyboard.events['a'] & Wyn_OnKeyRelease) {
			printf("OnKeyRelease\n");
			fflush(stdout);
		}
		wyn_update(&w);
		wyn_swap(&w);
	};
	wyn_glctx_destroy(&glc);
	wyn_destroy(&w);
}
