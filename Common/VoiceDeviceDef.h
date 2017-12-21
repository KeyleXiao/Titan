/*******************************************************************
** 文件名:	VoiceDeviceDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音设备相关模块定义,最低层的定义,没有网络和服务器相关定义
** 应  用:  从VoiceModule分出,以分清层次
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __D_VoiceDevice_H__
#define __D_VoiceDevice_H__




#pragma pack(1)

/////// 公共宏定义 ///////////////////////////////////////////////////////////////////


//#define  VOICE_PRINT_DEBUGBUF_INF					// 开启语音播放调试信息
//#define  VOICE_CHANGEPLAYBUFF_OPEN				// 开放自动调节播放语音缓冲功能,此功能只能在连续语音才有效，不连续的不要使用
//#define  VOICE_DELAYAUDIO_OPEN					// 开放卡时循环播放功能,解决网络卡导致停顿感，用播放最近的语音来带过去,测试效果还有点杂音，不好用


// tolua_begin


// 音频相关参数
#define CLAMP_MIN						-32000		// 音量最小值
#define CLAMP_MAX						32000		// 音量最大值
#define TRANSMIT_FADE_START				5			// 传输FADE开始
#define TRANSMIT_FADE_STOP				16			// 传输FADE停止
#define TRANSMIT_START_STREAK			3			// 激活传输语音开始的时间间隔
#define TRANSMIT_STOP_STREAK			-25			// 激活不传输语音的时间间隔
#define	VOICE_SMOOTH_VOICE_PARAM		10			// 用于流畅语音中 所用的参数
#define	VOICE_SMOOTH_VOICE_PARAM1		50			// 用于流畅语音中 所用的参数,比这个大,能听到更小的声音

#define VOICE_AUDIO_CHANNEL_COUNT		2			// 默认语音通道数,支持的音频通道数,立体声为2,单声道为1
#define CELT_FRAME_SIZE					512			// Celt帧数
#define BUFFERMAXSIZE					4096		// buffer数组大小,一定要比 CELT_FRAME_SIZE * VOICE_AUDIO_CHANNEL_COUNT 要大,否则存不下

#define VOICE_DELAYAUDIOFRAMES			50			// 卡时循环播放列队大小




// 视频相关参数
#define VOICE_QCIF_WIDTH				320			// 摄像头采集图像宽度,QCIF类型改为320*240,以方便用摄像头采集
#define VOICE_QCIF_HEIGHT				240			// 摄像头采集图像高度
#define VOICE_IMAGE_WIDTH				VOICE_QCIF_WIDTH	// 显示视频图像宽度,要和摄像头采集一样才行
#define VOICE_IMAGE_HEIGHT				VOICE_QCIF_HEIGHT	// 显示视频图像高度
#define VOICE_VIDEOIMAGE_MAXSIZE		VOICE_QCIF_WIDTH*VOICE_QCIF_HEIGHT*6	// 用来存一帧视频RGB24最大内存,必须大于4倍,否则内存越界
#define VOICE_VIDEOTEXTURE_WIDTH		512			// 显示视频纹理图像高度,为2的n次方

#define VOICE_CAMERATHREAD_INTERVAL		500			// 摄像头线程运行间隔时间,毫秒(默认500)
#define VOICE_SNAPSCREEN_INTERVAL		30			// 采集视频屏幕录像线程运行间隔时间,毫秒(默认30)
#define VOICE_INBUF_CAMERAFRAMES		5			// 摄像头采集视频最多缓存多少帧,默认5
#define VOICE_INBUF_SCREENFRAMES		5			// 屏幕录像采集视频最多缓存多少帧,默认5
#define VOICE_OUTBUF_SHOWFRAMES			1			// 显示视频最多缓存多少帧,默认1
#define VOICE_VIDEOSHOWCLS_INTERVAL		3000		// 清理超时没信号的显示通道间隔时间,毫秒(默认5000)
#define VOICE_VIDEOINTRA_INTERVAL		10000		// 多少毫秒发一个独立关键帧(H263视频压缩用),毫秒(默认10000)
#define VOICE_VIDEOIMG_QUALITY			8			// 视频压缩默认图像质量系数(H263视频压缩用),(默认8)
#define VOICE_VIDEOIMG_H264QUALITY		25			// 视频压缩默认图像质量系数(H264视频压缩用),(默认25)
#define VOICE_VIDEO_USEH264				1			// 视频压缩使用H264视频压缩用,否则用H263



/////// 公共枚举定义 ///////////////////////////////////////////////////////////////////




///// 客户端公用数据结构定义 //////////////////////////////////////////////


// 语音应用类型枚举
enum EMVoiceType
{
	//VoiceType_GM = 0,				// 官方语音应用
	VoiceType_Game = 0,				// 游戏语音应用
	VoiceType_Max,					// 最大语音应用数量
};

// 录音控制 设备的类型
enum EMVoiceSourceType
{
	enVoiceSourceTypeMic ,		// 麦克风
	enVoiceSourceTypeMix ,		// 混音
};


// 采集视频输入类型
enum EMVideoSourceType
{
	VideoSource_None ,			// 无
	VideoSource_Camera ,		// 摄像头
	VideoSource_Screen ,		// 屏幕录像
	VideoSource_Max ,
};

// 显示视频的通道类型
enum EMVideoShowChannelType
{
	VideoShowChannel_User0 = 0,		// 默认用户显示
	VideoShowChannel_User1,			// 用户1显示
	VideoShowChannel_Local,			// 本地显示,一定要放到最后
	VideoShowChannel_Max,
};

// 屏幕录像大小类型
enum EMVideoCaptureScreenSizeType
{
	VideoCaptureScreenSize_Img = 0,			// 默认摄像头图像大小
	VideoCaptureScreenSize_Img2,			// 摄像头图像大小X2
	VideoCaptureScreenSize_Game,			// 游戏画面
	VideoCaptureScreenSize_Full,			// 全屏幕
	VideoCaptureScreenSize_Max,
};

// 摄像头控制命令
enum EMVideoCameraControlCode
{
	VideoCameraControl_None = 0,		// 无
	VideoCameraControl_Connect,			// 连接视频采集驱动
	VideoCameraControl_Disconnect,		// 断开连接视频采集驱动
	VideoCameraControl_StartCapture,	// 启动视频采集
	VideoCameraControl_StopCapture,		// 停止视频采集
	VideoCameraControl_FormatDlg,		// 视频输出格式选择
	VideoCameraControl_SourceDlg,		// 视频来源选择
	VideoCameraControl_Max,
};

// 视频码率控制配置分类模式
enum EMVideoNetSpeedType
{
	VideoNetSpeed_Smooth = 0,			// 优先流畅
	VideoNetSpeed_SmoothLow,			// 优先流畅+节省带宽(跳帧压缩加强)
	VideoNetSpeed_SmoothHigh,			// 优先流畅+更多带宽(跳帧压缩减弱)
	VideoNetSpeed_Quality,				// 优先画质
	VideoNetSpeed_QualityLow,			// 优先画质+节省带宽(跳帧压缩加强)
	VideoNetSpeed_QualityHigh,			// 优先画质+更多带宽(跳帧压缩减弱)
	VideoNetSpeed_Max,
};




//// 结构体信息 //////////////////////////////////////////////////////////////////////


// 设备信息
struct SVoiceDeviceInfo
{
	int 		nDevice;			// 设备
	bool		bInputDevice;		// 是否是输入设备
	char		szDeviceName[64];	// 设备名称


	SVoiceDeviceInfo(void)
	{
		memset( this, 0, sizeof(SVoiceDeviceInfo) );
	}
};


// tolua_end

////// 结构体信息 ////////////////////////////////////////////////////////////////////




#pragma pack()


#endif	// __D_VoiceDevice_H__