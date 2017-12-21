/*******************************************************************
** �ļ���:	VoiceDeviceDef.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������ (sww8@163.com)
** ��  ��:	2011-05-16
** ��  ��:	1.0
** ��  ��:	�����豸���ģ�鶨��,��Ͳ�Ķ���,û������ͷ�������ض���
** Ӧ  ��:  ��VoiceModule�ֳ�,�Է�����
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once


#ifndef __D_VoiceDevice_H__
#define __D_VoiceDevice_H__




#pragma pack(1)

/////// �����궨�� ///////////////////////////////////////////////////////////////////


//#define  VOICE_PRINT_DEBUGBUF_INF					// �����������ŵ�����Ϣ
//#define  VOICE_CHANGEPLAYBUFF_OPEN				// �����Զ����ڲ����������幦��,�˹���ֻ����������������Ч���������Ĳ�Ҫʹ��
//#define  VOICE_DELAYAUDIO_OPEN					// ���ſ�ʱѭ�����Ź���,������翨����ͣ�ٸУ��ò������������������ȥ,����Ч�����е�������������


// tolua_begin


// ��Ƶ��ز���
#define CLAMP_MIN						-32000		// ������Сֵ
#define CLAMP_MAX						32000		// �������ֵ
#define TRANSMIT_FADE_START				5			// ����FADE��ʼ
#define TRANSMIT_FADE_STOP				16			// ����FADEֹͣ
#define TRANSMIT_START_STREAK			3			// �����������ʼ��ʱ����
#define TRANSMIT_STOP_STREAK			-25			// �������������ʱ����
#define	VOICE_SMOOTH_VOICE_PARAM		10			// �������������� ���õĲ���
#define	VOICE_SMOOTH_VOICE_PARAM1		50			// �������������� ���õĲ���,�������,��������С������

#define VOICE_AUDIO_CHANNEL_COUNT		2			// Ĭ������ͨ����,֧�ֵ���Ƶͨ����,������Ϊ2,������Ϊ1
#define CELT_FRAME_SIZE					512			// Celt֡��
#define BUFFERMAXSIZE					4096		// buffer�����С,һ��Ҫ�� CELT_FRAME_SIZE * VOICE_AUDIO_CHANNEL_COUNT Ҫ��,����治��

#define VOICE_DELAYAUDIOFRAMES			50			// ��ʱѭ�������жӴ�С




// ��Ƶ��ز���
#define VOICE_QCIF_WIDTH				320			// ����ͷ�ɼ�ͼ����,QCIF���͸�Ϊ320*240,�Է���������ͷ�ɼ�
#define VOICE_QCIF_HEIGHT				240			// ����ͷ�ɼ�ͼ��߶�
#define VOICE_IMAGE_WIDTH				VOICE_QCIF_WIDTH	// ��ʾ��Ƶͼ����,Ҫ������ͷ�ɼ�һ������
#define VOICE_IMAGE_HEIGHT				VOICE_QCIF_HEIGHT	// ��ʾ��Ƶͼ��߶�
#define VOICE_VIDEOIMAGE_MAXSIZE		VOICE_QCIF_WIDTH*VOICE_QCIF_HEIGHT*6	// ������һ֡��ƵRGB24����ڴ�,�������4��,�����ڴ�Խ��
#define VOICE_VIDEOTEXTURE_WIDTH		512			// ��ʾ��Ƶ����ͼ��߶�,Ϊ2��n�η�

#define VOICE_CAMERATHREAD_INTERVAL		500			// ����ͷ�߳����м��ʱ��,����(Ĭ��500)
#define VOICE_SNAPSCREEN_INTERVAL		30			// �ɼ���Ƶ��Ļ¼���߳����м��ʱ��,����(Ĭ��30)
#define VOICE_INBUF_CAMERAFRAMES		5			// ����ͷ�ɼ���Ƶ��໺�����֡,Ĭ��5
#define VOICE_INBUF_SCREENFRAMES		5			// ��Ļ¼��ɼ���Ƶ��໺�����֡,Ĭ��5
#define VOICE_OUTBUF_SHOWFRAMES			1			// ��ʾ��Ƶ��໺�����֡,Ĭ��1
#define VOICE_VIDEOSHOWCLS_INTERVAL		3000		// ����ʱû�źŵ���ʾͨ�����ʱ��,����(Ĭ��5000)
#define VOICE_VIDEOINTRA_INTERVAL		10000		// ���ٺ��뷢һ�������ؼ�֡(H263��Ƶѹ����),����(Ĭ��10000)
#define VOICE_VIDEOIMG_QUALITY			8			// ��Ƶѹ��Ĭ��ͼ������ϵ��(H263��Ƶѹ����),(Ĭ��8)
#define VOICE_VIDEOIMG_H264QUALITY		25			// ��Ƶѹ��Ĭ��ͼ������ϵ��(H264��Ƶѹ����),(Ĭ��25)
#define VOICE_VIDEO_USEH264				1			// ��Ƶѹ��ʹ��H264��Ƶѹ����,������H263



/////// ����ö�ٶ��� ///////////////////////////////////////////////////////////////////




///// �ͻ��˹������ݽṹ���� //////////////////////////////////////////////


// ����Ӧ������ö��
enum EMVoiceType
{
	//VoiceType_GM = 0,				// �ٷ�����Ӧ��
	VoiceType_Game = 0,				// ��Ϸ����Ӧ��
	VoiceType_Max,					// �������Ӧ������
};

// ¼������ �豸������
enum EMVoiceSourceType
{
	enVoiceSourceTypeMic ,		// ��˷�
	enVoiceSourceTypeMix ,		// ����
};


// �ɼ���Ƶ��������
enum EMVideoSourceType
{
	VideoSource_None ,			// ��
	VideoSource_Camera ,		// ����ͷ
	VideoSource_Screen ,		// ��Ļ¼��
	VideoSource_Max ,
};

// ��ʾ��Ƶ��ͨ������
enum EMVideoShowChannelType
{
	VideoShowChannel_User0 = 0,		// Ĭ���û���ʾ
	VideoShowChannel_User1,			// �û�1��ʾ
	VideoShowChannel_Local,			// ������ʾ,һ��Ҫ�ŵ����
	VideoShowChannel_Max,
};

// ��Ļ¼���С����
enum EMVideoCaptureScreenSizeType
{
	VideoCaptureScreenSize_Img = 0,			// Ĭ������ͷͼ���С
	VideoCaptureScreenSize_Img2,			// ����ͷͼ���СX2
	VideoCaptureScreenSize_Game,			// ��Ϸ����
	VideoCaptureScreenSize_Full,			// ȫ��Ļ
	VideoCaptureScreenSize_Max,
};

// ����ͷ��������
enum EMVideoCameraControlCode
{
	VideoCameraControl_None = 0,		// ��
	VideoCameraControl_Connect,			// ������Ƶ�ɼ�����
	VideoCameraControl_Disconnect,		// �Ͽ�������Ƶ�ɼ�����
	VideoCameraControl_StartCapture,	// ������Ƶ�ɼ�
	VideoCameraControl_StopCapture,		// ֹͣ��Ƶ�ɼ�
	VideoCameraControl_FormatDlg,		// ��Ƶ�����ʽѡ��
	VideoCameraControl_SourceDlg,		// ��Ƶ��Դѡ��
	VideoCameraControl_Max,
};

// ��Ƶ���ʿ������÷���ģʽ
enum EMVideoNetSpeedType
{
	VideoNetSpeed_Smooth = 0,			// ��������
	VideoNetSpeed_SmoothLow,			// ��������+��ʡ����(��֡ѹ����ǿ)
	VideoNetSpeed_SmoothHigh,			// ��������+�������(��֡ѹ������)
	VideoNetSpeed_Quality,				// ���Ȼ���
	VideoNetSpeed_QualityLow,			// ���Ȼ���+��ʡ����(��֡ѹ����ǿ)
	VideoNetSpeed_QualityHigh,			// ���Ȼ���+�������(��֡ѹ������)
	VideoNetSpeed_Max,
};




//// �ṹ����Ϣ //////////////////////////////////////////////////////////////////////


// �豸��Ϣ
struct SVoiceDeviceInfo
{
	int 		nDevice;			// �豸
	bool		bInputDevice;		// �Ƿ��������豸
	char		szDeviceName[64];	// �豸����


	SVoiceDeviceInfo(void)
	{
		memset( this, 0, sizeof(SVoiceDeviceInfo) );
	}
};


// tolua_end

////// �ṹ����Ϣ ////////////////////////////////////////////////////////////////////




#pragma pack()


#endif	// __D_VoiceDevice_H__