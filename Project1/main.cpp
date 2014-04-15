#include "leanwin.h"
#include "winglow.h"

#include "gl_core_3_2.h"

int main() {
	Winglow winglow;
    winglow.makeCurrent();
    winglow.show();

    if (!ogl_LoadFunctions()) return 1;
    static const GLfloat magenta[] = {1.0f, 0.0f, 1.0f, 1.0f};

	MSG msg;
	while (winglow.isAlive()) {
        glClearBufferfv(GL_COLOR, 0, magenta);
        winglow.swapBuffers();
        GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
