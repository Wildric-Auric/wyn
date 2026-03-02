#include "wyn.h"
#include <GL/gl.h>
#include <stdio.h>


void resize(wyndow* w, wyn_vec2* s, void* usr_data) {
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
	resize(&w, &inf.rect.pos, 0);
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
