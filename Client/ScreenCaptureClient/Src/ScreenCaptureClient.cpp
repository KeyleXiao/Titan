#include "stdafx.h"
#include "ScreenCaptureClient.h"
#include "GameViewDef.h"

CScreenCaptureClient *g_pScreenCaptureClient = NULL;

CScreenCaptureClient::CScreenCaptureClient()
{
	srcWidth = outWidth = 1280;
	srcHeight = outHeight = 720;
	outFrameRate = 20;

	tempFilePath = "";
	outFilePath = "";
	tempFileName = "/temp.yuv";

	m_bExit = false;
	m_bInit = false;
	m_bCapture = false;
	m_bEncording = false;
	m_bValidFile = false;

	m_ScreenCaptureThread = INVALID_HANDLE_VALUE;
}

CScreenCaptureClient::~CScreenCaptureClient()
{

}

//创建
bool CScreenCaptureClient::create(void)
{
	InitCapture();

	// 显示层事件
	if (gClientGlobal->getEntityClient() != NULL)
	{
		gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SCREENOPEN_OPEN, this);
		gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SCREENOPEN_STOP, this);

		return true;
	}

	return false;
}

//释放
void CScreenCaptureClient::release(void)
{
	if (gClientGlobal->getEntityClient() != NULL)
	{
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SCREENOPEN_OPEN);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SCREENOPEN_STOP);
	}

    StartScreenCaptureThread(false);
	UnInitCapture();

	g_pScreenCaptureClient = NULL;

	delete this;
}

//初始化
void CScreenCaptureClient::InitCapture()
{
	//注册
	av_register_all();
	avdevice_register_all();

	m_bInit = true;
	m_bCapture = false;
}

//反初始化
void CScreenCaptureClient::UnInitCapture()
{
	m_bCapture = false;
	m_bInit = false;
}

//是否正在录制
bool CScreenCaptureClient::IsCapture()
{
	return m_bCapture;
}

//是否正在编码
bool CScreenCaptureClient::IsEncode()
{
	return m_bEncording;
}

//显示层消息处理
bool CScreenCaptureClient::onViewEvent(int eventID,int nParam,const char * strParam,void * ptrParam)
{
	switch(eventID)
	{
	case GVIEWCMD_SCREENOPEN_OPEN:  
		{
			if(ptrParam == NULL)
			{
				return false;
			}

			gameview_start_screenCapture* pCmd = (gameview_start_screenCapture*)ptrParam;
			StartScreenCapture(pCmd);
		}
		break;
	case GVIEWCMD_SCREENOPEN_STOP:  
		{
			if(ptrParam == NULL)
			{
				return false;
			}

			gameview_stop_screenCapture* pCmd = (gameview_stop_screenCapture*)ptrParam;
			StopScreenCapture(pCmd);
		}
		break;
	default:
		{
			TraceLn(__FUNCTION__ << ", event =" << eventID);
		}
	}
	return true;
}

//请求开始录制
bool CScreenCaptureClient::StartScreenCapture(gameview_start_screenCapture* cmd)
{
	if(cmd->point_x < 0 || cmd->point_y < 0 || cmd->width <= 0 || cmd->height <= 0)
		return false;

	if(cmd->szTempFilePath == "")
		return false;

	offsetX = cmd->point_x;
	offsetY = cmd->point_y;
	srcWidth = outWidth= cmd->width;
	srcHeight = outHeight = cmd->height;

	if(cmd->szTempFilePath != "")
	{
		tempFilePath = cmd->szTempFilePath;
		tempFilePath += tempFileName;
		tempFilePath = utf82a(tempFilePath);
	}

	if(IsCapture() || IsEncode())
		return false;

	m_bValidFile = false;
	m_bCapture = true;

	StartScreenCaptureThread(true);

	return true;
}

//请求结束录制
void CScreenCaptureClient::StopScreenCapture(gameview_stop_screenCapture* cmd)
{
	if(cmd->nValid > 0)
		m_bValidFile = true;

	if(cmd->szOutFilePath != "")
	{
		outFilePath = cmd->szOutFilePath;
		//outFilePath += ".mp4";
		outFilePath = utf82a(outFilePath);
	}

	m_bCapture = false;
	m_bValidFile = true;

	StartScreenCaptureThread(false);
}


bool CScreenCaptureClient::StartScreenCaptureThread(bool bStart)
{
	if(bStart)
	{
		if(INVALID_HANDLE_VALUE == m_ScreenCaptureThread)
		{
			m_bExit = false;
			m_ScreenCaptureThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScreenCaptureThread, this, 0, NULL);
			if(INVALID_HANDLE_VALUE == m_ScreenCaptureThread)
			{
				return false;
			}
		}
	}
	else
	{
		m_bExit = true;
		if(m_ScreenCaptureThread != INVALID_HANDLE_VALUE)
		{
			//::TerminateThread(m_ScreenCaptureThread, 0);
			m_ScreenCaptureThread = INVALID_HANDLE_VALUE;
		}
	}
	return true;
}


void* CScreenCaptureClient::ScreenCaptureThread(void* arg)
{
	CScreenCaptureClient* pScreenCaptureClinet = (CScreenCaptureClient*)arg;
	if(NULL != pScreenCaptureClinet)
	{
		pScreenCaptureClinet->RunScreenCapture();
	}
	return NULL;
}

//录屏线程功能
void CScreenCaptureClient::RunScreenCapture()
{
	//初始化
	AVFormatContext* pFormatContext_Video = avformat_alloc_context();
	AVDictionary* pDictionary = NULL;

	//设置录制参数
	StringHelper::String rate = "";
	StringHelper::fromInt(rate, outFrameRate);
	av_dict_set(&pDictionary,"framerate", rate.c_str(), 0);

	StringHelper::String offset_x = "";
	StringHelper::fromInt(offset_x, offsetX);
	av_dict_set(&pDictionary,"offset_x", offset_x.c_str(), 0);

	StringHelper::String offset_y = "";
	StringHelper::fromInt(offset_y, offsetY);
	av_dict_set(&pDictionary,"offset_y", offset_y.c_str(), 0);

	StringHelper::String resolution = "";
	StringHelper::format(resolution, "%dx%d",srcWidth, srcHeight);
	av_dict_set(&pDictionary,"video_size", resolution.c_str(), 0);

	av_dict_set(&pDictionary, "draw_mouse", "0",0);

	//获取输入流
	AVInputFormat* pInputFormat = av_find_input_format("gdigrab");
	if(avformat_open_input(&pFormatContext_Video, "desktop",pInputFormat, &pDictionary) != 0)
	{
		avformat_free_context(pFormatContext_Video);
		return;
	}
	if(avformat_find_stream_info(pFormatContext_Video,NULL) < 0)
	{
		avformat_close_input(&pFormatContext_Video);
		return;
	}

	//获取编码上下文
	AVCodecContext* pCodecContext_Video = pFormatContext_Video->streams[0]->codec;
	AVCodec* pCodec_Video = avcodec_find_decoder(pCodecContext_Video->codec_id);
	if(pCodec_Video == NULL)
	{
		avformat_close_input(&pFormatContext_Video);
		return;
	}
	if(avcodec_open2(pCodecContext_Video,pCodec_Video,NULL) < 0)
	{
		avformat_close_input(&pFormatContext_Video);
		return;
	}

	AVFrame* pFrame = av_frame_alloc();
	AVFrame* pFrameYuv420p = av_frame_alloc();
	if((pFrame == NULL) || (pFrameYuv420p == NULL))
	{
		avcodec_close(pCodecContext_Video);
		avformat_close_input(&pFormatContext_Video);
		return;
	}

	//填充图像
	int Yuv420pSize = avpicture_get_size(AV_PIX_FMT_YUV420P,outWidth,outHeight);
	uint8_t* Yuv420pBuf = (uint8_t* )av_malloc(Yuv420pSize);
	avpicture_fill((AVPicture* )pFrameYuv420p,Yuv420pBuf,AV_PIX_FMT_YUV420P,outWidth,outHeight);

	//打开临时文件
	FILE* fw_yuv420p = fopen(tempFilePath.c_str(),"wb");
	if(fw_yuv420p == NULL)
	{
		av_free(Yuv420pBuf);
		av_frame_free(&pFrameYuv420p);
		av_frame_free(&pFrame);
		avcodec_close(pCodecContext_Video);
		avformat_close_input(&pFormatContext_Video);
		return;
	}

	AVPacket pPacket;
	av_init_packet(&pPacket);
	pPacket.data = NULL;
	pPacket.size = 0;

	int got_picture_ptr = 0;
	while(!m_bExit)
	{
		//控制一下极限时间
		//TODO
		if(pFormatContext_Video->streams[0]->codec->frame_number > 600)
		{

		}

		if(m_bCapture)
		{
			if(av_read_frame(pFormatContext_Video,&pPacket) >= 0)
			{
				if(pPacket.stream_index == 0)
				{
					if(avcodec_decode_video2(pCodecContext_Video,pFrame,&got_picture_ptr,&pPacket) >= 0)
					{
						if(got_picture_ptr)
						{
							struct SwsContext* pSwsContext = sws_getContext(pCodecContext_Video->width,pCodecContext_Video->height,pCodecContext_Video->pix_fmt,outWidth,outHeight,AV_PIX_FMT_YUV420P,SWS_SINC,NULL,NULL,NULL);
							sws_scale(pSwsContext,(const uint8_t* const* )pFrame->data,pFrame->linesize,0,pCodecContext_Video->height,pFrameYuv420p->data,pFrameYuv420p->linesize);
							sws_freeContext(pSwsContext);
							fwrite(Yuv420pBuf,1,Yuv420pSize,fw_yuv420p);
						}
					}
				}
				av_free_packet(&pPacket);
			}
		}
	}

	//关闭文件，释放内存
	fclose(fw_yuv420p);
	av_free(Yuv420pBuf);
	av_frame_free(&pFrameYuv420p);
	av_frame_free(&pFrame);
	avcodec_close(pCodecContext_Video);
	avformat_close_input(&pFormatContext_Video);

	if(m_bValidFile)
		RunEncode();
}

////////////////////////////////编码文件////////////////////////////////////////

//bool CScreenCapture::EncodeFile()
//{
////如果正在编码，则返回
//if(m_bEncording)
//	return false;

////检查文件
//m_bEncording = true;
//FILE* fr_yuv420p = fopen(tempFilePath.c_str(),"rb");
//if(fr_yuv420p == NULL)
//{
//	m_bEncording = false;
//	return false;
//}
//
////初始化输出流
//AVOutputFormat* pOutputFormat = av_guess_format(NULL,outFilePath.c_str(),NULL);
////pOutputFormat->video_codec = AV_CODEC_ID_H264;

//if(pOutputFormat == NULL)
//{
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

////初始化上下文
//AVFormatContext* pFormatContext = avformat_alloc_context();
//if(pFormatContext == NULL)
//{
//	fclose(fr_yuv420p);
//	m_bEncording = false;	
//	return false;
//}

////赋值
//pFormatContext->oformat = pOutputFormat;
//strncpy(pFormatContext->filename,outFilePath.c_str(),sizeof(pFormatContext->filename));
//if(pOutputFormat->video_codec == AV_CODEC_ID_NONE)
//{
//	avformat_free_context(pFormatContext);
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

////输入流
//AVStream* pStream = avformat_new_stream(pFormatContext,NULL);
//if(pStream == NULL)
//{
//	avformat_free_context(pFormatContext);
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

////编码参数控制
//AVCodecContext* pCodecContext = pStream->codec;
//pCodecContext->codec_id = pFormatContext->oformat->video_codec;
//pCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
//pCodecContext->bit_rate = 400000;
//pCodecContext->gop_size = outFrameRate * 10;
//pCodecContext->pix_fmt = PIX_FMT_YUV420P;
//pCodecContext->width = outWidth;
//pCodecContext->height = outHeight;
//pCodecContext->time_base.num = 1;
//pCodecContext->time_base.den = outFrameRate;
//pCodecContext->me_range = 16;
//pCodecContext->max_qdiff = 4;        //视频中的所有帧(包括i/b/p)的最大Q值差距
//pCodecContext->qcompress = 0.6;      //0.1-1.0
//if(pCodecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
//	pCodecContext->max_b_frames = 2;
//if(pCodecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO)
//	pCodecContext->mb_decision = 2;
//if(!strcmp(pFormatContext->oformat->name,"mp4") || !strcmp(pFormatContext->oformat->name,"mov") || !strcmp(pFormatContext->oformat->name,"3gp"))
//	pCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

//AVCodec* pCodec = avcodec_find_encoder(pCodecContext->codec_id);
//if(avcodec_open2(pCodecContext,pCodec,NULL) < 0)
//{
//	avformat_free_context(pFormatContext);
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

//if(avio_open(&pFormatContext->pb,outFilePath.c_str(),AVIO_FLAG_WRITE) < 0)
//{
//	avcodec_close(pCodecContext);
//	avformat_free_context(pFormatContext);
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

//int video_outbuf_size = 0;
//uint8_t* video_outbuf = NULL;
//if(!(pFormatContext->oformat->flags & AVFMT_RAWPICTURE))
//{
//	video_outbuf_size = 200000;
//	video_outbuf = (uint8_t* )av_malloc(video_outbuf_size);
//}

//int pFrameSize = avpicture_get_size(pCodecContext->pix_fmt,pCodecContext->width,pCodecContext->height);
//uint8_t* pFrameBuf = (uint8_t* )av_malloc(pFrameSize);
//AVFrame* pFrame = avcodec_alloc_frame();
//if(pFrame == NULL)
//{
//	avio_close(pFormatContext->pb);
//	avcodec_close(pCodecContext);
//	avformat_free_context(pFormatContext);
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

//avpicture_fill((AVPicture *)pFrame,pFrameBuf,pCodecContext->pix_fmt,pCodecContext->width,pCodecContext->height);
//pFrame->format = pCodecContext->pix_fmt;
//pFrame->width  = pCodecContext->width;
//pFrame->height = pCodecContext->height;

////写入头
//if(avformat_write_header(pFormatContext,NULL) < 0)
//{
//	av_free(pFrame);
//	av_free(pFrameBuf);
//	avio_close(pFormatContext->pb);
//	avcodec_close(pCodecContext);
//	avformat_free_context(pFormatContext);
//	fclose(fr_yuv420p);
//	m_bEncording = false;
//	return false;
//}

////写数据
//while(1)
//{
//	if(fread(pFrame->data[0],1,pFrameSize,fr_yuv420p) != pFrameSize)
//		break;

//	pFrame->pts++;

//	int ReturnValue = 0,buf_size = 0;
//	AVPacket pPacket;
//	if(pFormatContext->oformat->flags & AVFMT_RAWPICTURE)
//	{
//		av_init_packet(&pPacket);
//		pPacket.flags |= AV_PKT_FLAG_KEY;
//		pPacket.stream_index = pStream->index;
//		pPacket.data = (uint8_t* )pFrame;
//		pPacket.size = sizeof(AVPicture);
//		ReturnValue = av_write_frame(pFormatContext,&pPacket);
//	}
//	else
//	{
//		buf_size = avcodec_encode_video(pCodecContext,video_outbuf,video_outbuf_size,pFrame);
//		if(buf_size > 0)
//		{
//			av_init_packet(&pPacket);
//			if(pCodecContext->coded_frame->key_frame)
//				pPacket.flags |= AV_PKT_FLAG_KEY;
//			pPacket.stream_index = pStream->index;
//			pPacket.data = video_outbuf;
//			pPacket.size = buf_size;
//			ReturnValue = av_write_frame(pFormatContext,&pPacket);
//		}
//		else
//			ReturnValue = 0;
//	}
//	if(ReturnValue != 0)
//	{
//		av_free_packet(&pPacket);
//		av_free(pFrame);
//		av_free(pFrameBuf);
//		avio_close(pFormatContext->pb);
//		avcodec_close(pCodecContext);
//		avformat_free_context(pFormatContext);
//		fclose(fr_yuv420p);
//		m_bEncording = false;
//		return false;
//	}
//}

//av_write_trailer(pFormatContext); 

////释放
//av_free(pFrame);
//av_free(pFrameBuf);
//avio_close(pFormatContext->pb);
//avcodec_close(pCodecContext);
//avformat_free_context(pFormatContext);
//fclose(fr_yuv420p);
//m_bEncording = false;
//return true;
//}
////////////////////////////////////////////////////////////////////////





void CScreenCaptureClient::RunEncode()
{
	//如果正在编码，则返回
	if(m_bEncording)
		return;

	m_bEncording = true;

	AVFormatContext *pFormatCtx = nullptr;
	AVOutputFormat *fmt = nullptr;
	AVStream *video_st = nullptr;	
	AVCodecContext *pCodecCtx=nullptr;
	AVCodec *pCodec=nullptr;

	uint8_t *picture_buf = nullptr;
	AVFrame *picture=nullptr;	
	int size;

	//打开YUV文件
	FILE *in_file = fopen(tempFilePath.c_str(), "rb");
	if(!in_file)
	{
		WarningLn("[REC]can not open file!");
		fclose(in_file);
		m_bEncording = false;
		return;
	}

	if(outFilePath.empty())
	{
		WarningLn("[REC]outFilePath null!");
		fclose(in_file);
		m_bEncording = false;
		return;
	}

	//初始化AVFormatContext结构体
	//FFmepg会根据文件名获取合适的封装格式
	avformat_alloc_output_context2(&pFormatCtx,NULL,NULL,outFilePath.c_str());
	if(pFormatCtx == NULL)
	{
		WarningLn("[REC] pFormatCtx null!");
		fclose(in_file);
		m_bEncording = false;
		return;
	}

	fmt = pFormatCtx->oformat;

	//打开输出文件
	if(avio_open(&pFormatCtx->pb,outFilePath.c_str(),AVIO_FLAG_READ_WRITE))
	{
		WarningLn("[REC] output file open fail!");
		if(pFormatCtx)
		{
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
		}
		fclose(in_file);
		m_bEncording = false;
		return;
	}

	//初始化视频码流
	video_st = avformat_new_stream(pFormatCtx,0);
	if(video_st==NULL)
	{ 
		WarningLn("[REC] failed allocating output stram!");
		if(pFormatCtx)
		{
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
		}
		fclose(in_file);
		m_bEncording = false;
		return;
	}

	video_st->time_base.num = 1;
	video_st->time_base.den =outFrameRate;

	//编码器设参，主要设置比特率、帧率
	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width=outWidth;
	pCodecCtx->height=outHeight;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = outFrameRate;
	pCodecCtx->bit_rate = 4000000;
	pCodecCtx->gop_size = outFrameRate * 10;  

	if(pCodecCtx->codec_id == AV_CODEC_ID_H264)
	{
		pCodecCtx->qmin = 10;
		pCodecCtx->qmax = 51;
		pCodecCtx->qcompress = 0.6;

		av_opt_set(pCodecCtx->priv_data, "preset", "fast", 0);
		av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);
	}
	if (pCodecCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO)
		pCodecCtx->max_b_frames = 2;
	if (pCodecCtx->codec_id == AV_CODEC_ID_MPEG1VIDEO)
		pCodecCtx->mb_decision = 2;

	//一些格式需要视频流数据头分开
	if(!strcmp(pFormatCtx->oformat->name,"mp4") || !strcmp(pFormatCtx->oformat->name,"mov") || !strcmp(pFormatCtx->oformat->name,"3gp"))
		pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if(!pCodec)
	{
		WarningLn("[REC] no right encoder!");
		if(pFormatCtx)
		{
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
		}
		fclose(in_file);
		m_bEncording = false;
		return;
	}
	if(avcodec_open2(pCodecCtx,pCodec,NULL)<0)
	{
		WarningLn("[REC] open encoder fail!");
		if(video_st)
		{
			avcodec_close(video_st->codec);
		}
		if(pFormatCtx)
		{
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
		}
		fclose(in_file);
		m_bEncording = false;
		return;
	}

	//输出格式信息
	av_dump_format(pFormatCtx,0,outFilePath.c_str(),1);

	//初始化帧
	picture = av_frame_alloc();
	picture->width=pCodecCtx->width;
	picture->height=pCodecCtx->height;
	picture->format=pCodecCtx->pix_fmt;
	size = avpicture_get_size(pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height);
	picture_buf = (uint8_t*)av_malloc(size);
	avpicture_fill((AVPicture*)picture,picture_buf,pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height);

	//写文件头
	avformat_write_header(pFormatCtx,NULL);

	//创建已经编码的帧
	AVPacket pkt;
	int y_size = pCodecCtx->width*pCodecCtx->height;
	av_new_packet(&pkt,size*3);

	for(int i=0;;i++)
	{
		//读入YUV
		if(fread(picture_buf,1,y_size*3/2,in_file)<0)
		{
			WarningLn("[REC] read file fail!");
			if(video_st)
			{
				avcodec_close(video_st->codec);
				av_free(picture);
				av_free(picture_buf);
			}
			if(pFormatCtx)
			{
				avio_close(pFormatCtx->pb);
				avformat_free_context(pFormatCtx);
			}
			fclose(in_file);

			m_bEncording = false;
			return;
		}
		else if(feof(in_file))
			break;

		picture->data[0] = picture_buf; //亮度Y
		picture->data[1] = picture_buf+y_size; //U
		picture->data[2] = picture_buf+y_size*5/4; //V
		//AVFrame PTS
		picture->pts=i;
		int got_picture = 0;

		//编码
		int ret = avcodec_encode_video2(pCodecCtx,&pkt,picture,&got_picture);
		if(ret<0)
		{
			WarningLn("[REC] encoder fail!");
			if(video_st)
			{
				avcodec_close(video_st->codec);
				av_free(picture);
				av_free(picture_buf);
			}
			if(pFormatCtx)
			{
				avio_close(pFormatCtx->pb);
				avformat_free_context(pFormatCtx);
			}
			fclose(in_file);

			m_bEncording = false;
			return;
		}

		if(got_picture == 1)
		{
			// parpare packet for muxing
			pkt.stream_index = video_st->index;
			av_packet_rescale_ts(&pkt, pCodecCtx->time_base, video_st->time_base);
			pkt.pos = -1;
			ret = av_interleaved_write_frame(pFormatCtx,&pkt);
			av_free_packet(&pkt);
		}
	}

	//Flush encoder
	int ret = flush_encoder(pFormatCtx,0);
	if(ret < 0)
	{
		WarningLn("[REC] flushing encoder failed!");
		if(video_st)
		{
			avcodec_close(video_st->codec);
			av_free(picture);
			av_free(picture_buf);
		}
		if(pFormatCtx)
		{
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
		}
		fclose(in_file);

		m_bEncording = false;
		return;
	}

	//写文件尾
	av_write_trailer(pFormatCtx);

	//释放内存
	if(video_st)
	{
		avcodec_close(video_st->codec);
		av_free(picture);
		av_free(picture_buf);
	}
	if(pFormatCtx)
	{
		avio_close(pFormatCtx->pb);
		avformat_free_context(pFormatCtx);
	}

	//关闭文件
	fclose(in_file);

	m_bEncording = false;

	return;
}

int CScreenCaptureClient::flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{
	int ret;
	int got_frame;
	AVPacket enc_pkt;

	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities & CODEC_CAP_DELAY))
		return 0;

	while (1) 
	{
		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);

		ret = avcodec_encode_video2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt, NULL, &got_frame);

		av_frame_free(NULL);

		if (ret < 0)
			break;

		if (!got_frame)
		{
			ret=0;
			break;
		}

		// parpare packet for muxing
		enc_pkt.stream_index = stream_index;

		av_packet_rescale_ts(&enc_pkt,
			fmt_ctx->streams[stream_index]->codec->time_base,
			fmt_ctx->streams[stream_index]->time_base);

		ret = av_interleaved_write_frame(fmt_ctx, &enc_pkt);

		if (ret < 0)
			break;
	}

	return ret;
}

API_EXPORT IScreenCaptureClient* CreateScreenCaptureClient(void)
{
	// 如果创建了，则直接返回
	if(g_pScreenCaptureClient != NULL)
	{
		return g_pScreenCaptureClient;
	}

	// 实例化系统模块
	g_pScreenCaptureClient = new CScreenCaptureClient();
	if(g_pScreenCaptureClient == NULL || !g_pScreenCaptureClient->create())
	{
		safeRelease(g_pScreenCaptureClient);

		return NULL;
	}

	return g_pScreenCaptureClient;
}
