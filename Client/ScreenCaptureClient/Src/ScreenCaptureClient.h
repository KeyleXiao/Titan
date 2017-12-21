/*******************************************************************
** �ļ���:	ScreenCaptureClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���Ĳ�
** ��  ��:	24/7/2017
** ��  ��:	1.0
** ��  ��:	¼���ͻ���
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

	//����
	bool create(void);

	//�ͷ�
	virtual void release(void);

	//�Ƿ�������Ƶ�ɼ�
	virtual bool IsCapture();

	//�Ƿ�������Ƶ����
	virtual bool IsEncode();

	//��ʾ����Ϣ����
	virtual bool onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam );

	bool StartScreenCapture(gameview_start_screenCapture* cmd);

	void StopScreenCapture(gameview_stop_screenCapture* cmd); 

private:

	//��ʼ���ɼ�
	void InitCapture();

	//����ʼ��
	void UnInitCapture();

	//����¼���߳�
	bool StartScreenCaptureThread(bool bStart);

	//¼��ר���߳�
	static void* ScreenCaptureThread(void* arg);

	//¼���̹߳���ʵ��
	void RunScreenCapture(void);

	//���빦��ʵ��
	void RunEncode(void);

	int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index);

private: 

	// ¼���߳�
	HANDLE m_ScreenCaptureThread;

	//�˳��߳�
	bool m_bExit;

	//�Ƿ��ѳ�ʼ���ɹ�
	bool m_bInit;

	//�Ƿ����Ƶ�ɼ�
	bool m_bCapture;

	//�Ƿ����ڱ���
	bool m_bEncording;

	//�Ƿ�Ϊ��Ч�ļ�(����Ҫ�����ļ�)
	bool m_bValidFile;

	//��ʱ�ļ�����
	char* tempFileName;

public:
	int offsetX;              //����λ��ƫ��
	int offsetY;
	int srcWidth;             //Դ���ݿ��
	int srcHeight;
	std::string tempFilePath; //��ʱ�ļ�Ŀ¼

	int outWidth;             //������
	int outHeight;
	int outFrameRate;         //���֡��
	std::string outFilePath;  //����ļ�·��
};

extern CScreenCaptureClient* g_pScreenCaptureClient;

#endif // __SCREENCAPTURECLIENT_H__