#define _WYN_H
#ifdef  _WYN_H

typedef unsigned long Word;
typedef unsigned char uchar;
typedef unsigned int  uint;


typedef struct {
    int x;
    int y;
} wyn_vec2;
typedef struct {
    wyn_vec2 pos;
    wyn_vec2 size;
} wyn_rect;

#define _WYN_H
#define PLTFRM_WIN32

typedef struct {
    void* handle;     
    void* extra;
} wyndow;

typedef struct {
    char*    desc; 
    wyn_rect rect;
} wyn_crt_info;

#ifdef PLTFRM_WIN32
typedef enum {
	DWMWCP_DEFAULT      = 0,
	DWMWCP_DONOTROUND   = 1,
	DWMWCP_ROUND        = 2,
	DWMWCP_ROUNDSMALL   = 3
} DWM_WINDOW_CORNER_PREFERENCE;
int  wyn_win32_create(wyndow*, wyn_crt_info*);
int  wyn_win32_destroy();
#endif

#ifdef PLTFRM_LINUX
int  wyn_linux_create(wyndow*, wyn_crt_info*);
int  wyn_linux_destroy(wyndow* w);
#endif



int  wyn_create(wyndow*, wyn_crt_info*);
int  wyn_destroy(wyndow* w);


#endif
