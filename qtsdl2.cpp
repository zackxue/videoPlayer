#include "qtsdl2.h"
#include "cap_ffmpeg_impl.hpp"
#include <QResizeEvent>

qtsdl2::qtsdl2(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		puts("# error initializing SDL");
		puts(SDL_GetError());
		return ;
	}
	ui.setupUi(this);
	resize(600, 400);
 	m_Cap = new CvCapture_FFMPEG;
	//m_Cap->open("rtsp://159.99.251.194/");
	m_Cap->open("r.mp4");
	m_SdlWin = SDL_CreateWindowFrom((void *)this->winId());
	/*  0 stand for direct3d */
	m_SdlRender = SDL_CreateRenderer(m_SdlWin, 0, 
		0);
	int w = width();
	int h = height();
	m_pTex = SDL_CreateTexture(m_SdlRender, SDL_PIXELFORMAT_RGB24, 
		SDL_TEXTUREACCESS_STREAMING, width(), height());


	m_UpdateSize = false;
	m_SdlThread = new tthread::thread(qtsdl2::Run, (void *)this);
}

qtsdl2::~qtsdl2()
{

}

void qtsdl2::resizeEvent ( QResizeEvent * event )
{
	m_Mutex.lock();
	m_UpdateSize = true;
	
	QWidget::resizeEvent(event);
	m_Mutex.unlock();
}
void qtsdl2::mouseDoubleClickEvent(QMouseEvent *e)
{
    QWidget::mouseDoubleClickEvent(e);
	if(isFullScreen()) {
		this->setWindowState(Qt::WindowMaximized);
	} else {
		this->setWindowState(Qt::WindowFullScreen);
	}
}

void qtsdl2::updateSize()
{
	m_UpdateSize = false;

	SDL_DestroyTexture(m_pTex);
	m_pTex = NULL;
	SDL_DestroyRenderer(m_SdlRender);
	m_SdlRender = NULL;
	//SDL_DestroyWindow(m_SdlWin);
	//m_SdlWin = NULL;
	
	//m_SdlWin = SDL_CreateWindowFrom((void *)this->winId());
	int w = width();
	int h = height();
	//w = 1280;
	//h = 700;
	printf("[%s, %d], w %d h %d\n", __FILE__, __LINE__, w, h);
	w = (w/4) * 4;
	h = (h/4) * 4;
	m_Cap->setSize(w, h);
	
	m_SdlRender = SDL_CreateRenderer(m_SdlWin, 0, 
		0);
	m_pTex = SDL_CreateTexture(m_SdlRender, SDL_PIXELFORMAT_RGB24, 
		SDL_TEXTUREACCESS_STREAMING, w, h);
}

void qtsdl2::Run(void * pParam)
{
 	qtsdl2 *pQtSdl2 = NULL;
	if (pParam == NULL)
	{
		return;
	}
	pQtSdl2 = (qtsdl2 *)pParam;

	pQtSdl2->Run1();
}
void qtsdl2::Run1()
{
	int w = width();
	int h = height();
	m_Cap->setSize(width(), height());
	unsigned char *pData = NULL;
	int step = 0;
	int width = 0;
	int height = 0;
	int cn = 0;
	SDL_Surface *pRgb  = NULL; 
	void *pixels;
	int pitch;
	//m_Cap->open("rtsp://159.99.251.194/");
	m_Cap->open("r.mp4");

	while(1)
	{
		m_Mutex.lock();
		if (m_UpdateSize == true)
			updateSize();
		
		if (m_Cap->grabFrame() == false)
		{
			delete m_Cap;
			 m_Cap = new CvCapture_FFMPEG;
			//m_Cap->open("rtsp://159.99.251.194/");
			m_Cap->open("r.mp4");
			updateSize();
			m_Mutex.unlock();
			continue;
		}
		m_Cap->retrieveFrame(0, &pData, &step, &width, &height, &cn);
#if 0
		/* Render the Data to Widget */
		pRgb = SDL_CreateRGBSurfaceFrom(pData, width, height, 24, step, 
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				 0xff000000, /* Red bit mask. */
				 0x00ff0000, /* Green bit mask. */
				 0x0000ff00, /* Blue bit mask. */
				 0x00000000  /* Alpha bit mask. */
#else
				 0x000000ff, /* Red bit mask. */
				 0x0000ff00, /* Green bit mask. */
				 0x00ff0000, /* Blue bit mask. */
				 0x00000000  /* Alpha bit mask. */
#endif
				);
		pTex = SDL_CreateTextureFromSurface(m_SdlRender, pRgb);
		SDL_FreeSurface(pRgb);
		pRgb = NULL;
		SDL_RenderClear(m_SdlRender);
		SDL_RenderCopy(m_SdlRender, pTex, NULL, NULL);
		SDL_RenderPresent(m_SdlRender);
		SDL_DestroyTexture(pTex);
		pTex = NULL;
#else
		pixels = NULL;
		
		SDL_LockTexture(m_pTex, NULL, &pixels, &pitch);
		if (pixels)
			memcpy(pixels, pData, width * height * cn);
		SDL_UnlockTexture(m_pTex);
		SDL_RenderClear(m_SdlRender);
		SDL_RenderCopy(m_SdlRender, m_pTex, NULL, NULL);
		SDL_RenderPresent(m_SdlRender);
		m_Mutex.unlock();
#endif
		Sleep(20);
	}
}


