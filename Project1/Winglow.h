#ifndef WINGLOW_INCLUDE
#define WINGLOW_INCLUDE

class Winglow {
public:
	Winglow();
	~Winglow();

	bool isAlive();
    void makeCurrent();
    void swapBuffers();
    void show();
    void hide();
    void minimize();
    void maximize();
    void restore();

private:
	struct WinglowImpl;
	WinglowImpl *pimpl;
};

#endif
