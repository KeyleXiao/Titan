﻿#ifndef	CAESAR_CRC_H__DEF
#define CAESAR_CRC_H__DEF

/********************************************************************
	created:	2004/03/15
	created:	15:3:2004   17:09
	filename: 	e:\p_work\test\crc.h
	file path:	e:\p_work\test
	file base:	crc
	file ext:	h
	
	purpose:	处理CRC循环校验所需要的方法，可选择临时计算，
				或者一次计算出数值查表以增加速度．
*********************************************************************/

#include "Common.h"

#ifdef SUPPORT_CRC

namespace rkt {

class RKT_EXPORT CCRC
{

public:
	CCRC();
	virtual ~CCRC();


public:
	#define CRC16_CCITT	0x1021
	#define CRC16_DEFAULT	0x8005
	#define CRC32_DEFAULT	0x04C10DB7
	
	/*******************************************************************/
	/*
	函 数 名 称:	RunCRC16
	功 能 描 述：	执行对数据段的CRC16循环冗余校验
	参 数 说 明：	aData[in]:待校验数据
					aSize[in]:待校验数据长度
					aPoly[in]:创建表所需要的多项式
					
	  返回值 说明：	循环冗余校验结果
	  更 新 日 期：	2003.12.19
	/*******************************************************************/
	static	unsigned short RunCRC16( const unsigned char * aData, unsigned long aSize, unsigned short aPoly );
	/*******************************************************************/
	/*
	函 数 名 称:	RunCRC32
	功 能 描 述：	执行对数据段的CRC32循环冗余校验
	参 数 说 明：	aData[in]:待校验数据
					aSize[in]:待校验数据长度
					aPoly[in]:创建表所需要的多项式
					
	  返回值 说明：	循环冗余校验结果
	  更 新 日 期：	2003.12.19
	/*******************************************************************/
	static	unsigned long  RunCRC32( const unsigned char * aData, unsigned long aSize, unsigned long  aPoly );

};

} // rkt

/*
CRC算法简介：

RC校验的基本思想是利用线性编码理论，在发送端根据要传送的k位二进制码序列，
以一定的规则产生一个校验用的监督码（既CRC码）r位，并附在信息后边，构成
一个新的二进制码序列数共(k+r)位，最后发送出去。在接收端，则根据信息码和
CRC码之间所遵循的规则进行检验，以确定传送中是否出错。 
16位的CRC码产生的规则是先将要发送的二进制序列数左移16位（既乘以 ）后，再
除以一个多项式，最后所得到的余数既是CRC码，如式（2-1）式所示，其中B(X)表
示n位的二进制序列数，G(X)为多项式，Q(X)为整数，R(X)是余数（既CRC码）。 
（2-1） 求CRC码所采用模2加减运算法则，既是不带进位和借位的按位加减，这种加
减运算实际上就是逻辑上的异或运算，加法和减法等价，乘法和除法运算与普通代数
式的乘除法运算是一样，符合同样的规律。生成CRC码的多项式如下，其中CRC-16和
CRC-CCITT产生16位的CRC码，而CRC-32则产生的是32位的CRC码. 

CRC-16：（美国二进制同步系统中采用） 
CRC-CCITT：（由欧洲CCITT推荐） 
CRC-32： 

接收方将接收到的二进制序列数（包括信息码和CRC码）除以多项式，如果余数为0，
则说明传输中无错误发生，否则说明传输有误，关于其原理这里不再多述。用软件
计算CRC码时，接收方可以将接收到的信息码求CRC码，比较结果和接收到的CRC码是否相同。
*/


#endif //SUPPORT_CRC

#endif