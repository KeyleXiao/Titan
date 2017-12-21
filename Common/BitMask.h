/*******************************************************************
** �ļ���:	BitMask.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	
** ��  ��:	1.0
** ��  ��:	
** Ӧ  ��:  λ�洢
********************************************************************/

#pragma once

struct BitMask
{
	BYTE* pData;
	int nCapcity;   // �ֽ���

	BitMask()
	{
#define INIT_CAP 8
		nCapcity = INIT_CAP;
		pData = (BYTE*)malloc(nCapcity);
		memset(pData, 0, nCapcity);
	}

	~BitMask()
	{
		if (pData != NULL)
		{
			free(pData);
			pData = NULL;
		}
	}

	// nBit��0��ʼ
	void set(int nBit)
	{
		if (nBit < 0)
			return;

		int nIndex = nBit / 8;
		if (nIndex >= nCapcity)
		{
			BYTE* pNew = (BYTE*)malloc(nCapcity * 2);
			memset(pNew, 0, nCapcity * 2);
			if (pNew == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__ << ", malloc failed, size=" << nCapcity * 2);
				return;
			}

			if (pData != NULL)
			{
				memcpy(pNew, pData, nCapcity);
				free(pData);
				pData = pNew;
			}
			nCapcity *= 2;
		}

		if (pData == NULL)
			return;

		int nIndex2 = nBit % 8;

		pData[nIndex] |= (1 << nIndex2);
	}

	void clear(int nBit)
	{
		if (nBit < 0)
			return;

		if (pData == NULL)
			return;

		int nIndex = nBit / 8;
		if (nIndex >= nCapcity)
			return;

		int nIndex2 = nBit % 8;

		pData[nIndex] &= (~(1 << nIndex2));
	}

	bool test(int nBit)
	{
		if (nBit < 0)
			return false;

		if (pData == NULL)
			return false;

		int nIndex = nBit / 8;
		if (nIndex >= nCapcity)
			return false;

		int nIndex2 = nBit % 8;

		return pData[nIndex] & (1 << nIndex2);
	}
};