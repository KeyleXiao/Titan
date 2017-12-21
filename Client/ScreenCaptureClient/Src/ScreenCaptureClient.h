/*******************************************************************
** 文件名:	ScreenCaptureClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	昔文博
** 日  期:	24/7/2017
** 版  本:	1.0
** 描  述:	录屏客户端
********************************************************************/

#ifndef __SCREENCAPTURECLIENT_H__
#define __SCREENCAPTURECLIENT_H__

#include "IScreenCaptureClient.h"
#include "IEntityClient.h"
#include "GameViewDef.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"

#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(avcodec))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(avformat))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(avutil))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(avdevice))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(avfilter))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(swscale))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(postproc))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(swresample))

//#pragma comment(lib, "avcodec.lib")
//#pragma comment(lib, "avformat.lib")
//#pragma comment(lib, "avutil.lib")
//#pragma comment(lib, "avdevice.lib")
//#pragma comment(lib, "swscale.lib")
};

class CScreenCaptureClient : public IScreenCaptureClient, public IGameViewEventHandler
{
public:

	CScreenCaptureClient();

	virtual ~CScreenCaptureClient();

	//创建
	bool create(void);

	//释放
	virtual void release(void);

	//是否正在视频采集
	virtual bool IsCapture();

	//是否正在视频编码
	virtual bool IsEncode();

	//显示层消息处理
	virtual bool onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam );

	bool StartScreenCapture(gameview_start_screenCapture* cmd);

	void StopScreenCapture(gameview_stop_screenCapture* cmd); 

private:

	//初始化采集
	void InitCapture();

	//反初始化
	void UnInitCapture();

	//开启录屏线程
	bool StartScreenCaptureThread(bool bStart);

	//录屏专用线程
	static void* ScreenCaptureThread(void* arg);

	//录屏线程功能实现
	void RunScreenCapture(void);

	//编码功能实现
	void RunEncode(void);

	int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index);

private: 

	// 录屏线程
	HANDLE m_ScreenCaptureThread;

	//退出线程
	bool m_bExit;

	//是否已初始化成功
	bool m_bInit;

	//是否打开视频采集
	bool m_bCapture;

	//是否正在编码
	bool m_bEncording;

	//是否为有效文件(即需要编码文件)
	bool m_bValidFile;

	//临时文件名称
	char* tempFileName;

public:
	int offsetX;              //窗口位置偏移
	int offsetY;
	int srcWidth;             //源数据宽高
	int srcHeight;
	std::string tempFilePath; //临时文件目录

	int outWidth;             //输出宽高
	int outHeight;
	int outFrameRate;         //输出帧率
	std::string outFilePath;  //输出文件路径
};

extern CScreenCaptureClient* g_pScreenCaptureClient;

#endif // __SCREENCAPTURECLIENT_H__