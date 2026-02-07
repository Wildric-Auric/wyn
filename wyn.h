#ifndef _WYN_H
#define _WYN_H
#include <stdint.h>

#ifndef __cplusplus
typedef unsigned char bool;
#endif
typedef unsigned long Word;
typedef unsigned char uchar;
typedef unsigned int  uint;
typedef uint64_t ui64;

#define WYN_RESIZE_CBK_IDX 0

typedef struct {
    int x;
    int y;
} wyn_vec2;
typedef struct {
    wyn_vec2 pos;
    wyn_vec2 size;
} wyn_rect;
typedef struct {
    bool should_close;
} wyn_state;
typedef struct {
    ui64      handle;     
    void*     extra;
    wyn_state state;
    void*     _callbacks         [16];
    void*     _callbakcs_usr_data[16];
} wyndow;

typedef struct {
    char*     desc; 
    wyn_rect  rect;
} wyn_crt_info;

typedef void (*wyn_rz_callback_proc)(wyndow* w, wyn_vec2 new_size, void* usr_data);

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

extern int (*wyn_create)(wyndow*, wyn_crt_info*);
extern int (*wyn_destroy)(wyndow* w);
extern int (*wyn_show)(wyndow* w,bool flag);
extern int (*wyn_set_title)(wyndow* w, const char* desc);
extern int (*wyn_update)(wyndow* w);
extern int (*wyn_swap)(wyndow* w);
void wyn_rz_cbk_reg(wyndow* w, wyn_rz_callback_proc, void* usr_data);


#endif
