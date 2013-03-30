#ifndef QTSDL2_H
#define QTSDL2_H

#include <QtGui/QWidget>
#include "ui_qtsdl2.h"

#define NOMINMAX
#define NOMINMAX 
//#include <inttypes.h>

#include "SDL.h"
#undef main

#include "tinythread.h"
#include "fast_mutex.h"
using  namespace tthread;

class CvCapture_FFMPEG;

class qtsdl2 : public QWidget
{
	Q_OBJECT

public:
	qtsdl2(QWidget *parent = 0, Qt::WFlags flags = 0);
	~qtsdl2();
	void resizeEvent ( QResizeEvent * event );
	void mouseDoubleClickEvent(QMouseEvent *e);
	void updateSize();
public:
	static void Run(void * pParam);
	void Run1();
/* Thread */
private:
	tthread::thread *m_SdlThread;
	CvCapture_FFMPEG* m_Cap;
	tthread::fast_mutex m_Mutex;
	bool m_UpdateSize;


/* SDL2 */
	SDL_Window *m_SdlWin;
	SDL_Renderer *m_SdlRender;
	SDL_Texture *m_pTex;; 
	

private:
	Ui::qtsdl2Class ui;
};

#endif // QTSDL2_H
