/*******************************************************************
** 文件名:	e:\CoGame\Demo\src\Character\RandGenerator.h
** 版  权:	(C) 深圳未名网络技术有限公司(www.weimingtech.com)
** 创建人:	隆寒辉(Forlion Lon)
** 日  期:	2007/9/13  16:04
** 版  本:	1.0
** 描  述:	随机数发生器，使用前须播种子CRandGenerator::Seed
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifdef SUPPORT_RANDGENERATOR

#define GENIUS_NUMBER 0x376EAC5D

class CRandGenerator
{
	enum 
	{
		Number = 0x1000, 
	};
public:
	/** 固定种子
	@param   
	@param   
	@return  
	*/
	void Seed(DWORD dwSeed)
	{
		m_Seed = dwSeed^GENIUS_NUMBER;

		Batch();
	}

	/** 随机种子
	@param   
	@param   
	@return  
	*/
	DWORD Seed(void)
	{
		int s;
		__asm 
		{
			_asm _emit 0x0f _asm _emit 0x31
				mov s,eax
		}

		Seed(s);

		return s^GENIUS_NUMBER;
	}

	/** 
	@param   
	@param   
	@return  
	*/
	DWORD GetSeed(void) const
	{
		return m_Seed^GENIUS_NUMBER;
	}
	
	/** 产生一个WORD
	@param   
	@param   
	@return  
	*/
	WORD GetWORD(void)
	{
		if(m_Ptr >= Number * 2)
		{
			Batch();
			m_Ptr = 0;
		}

		return *((WORD *)m_pBuffer + m_Ptr++);
	};

	/** 产生一个DWord
	@param   
	@param   
	@return  
	*/
	DWORD GetDWORD(void)
	{
		if(m_Ptr >= Number * 2 - 1)
		{
			Batch();
			m_Ptr=0;
		}	

		m_Ptr += 2;
		return *(DWORD *)((WORD *)m_pBuffer + m_Ptr - 2);
	}

	/** 
	@param   
	@param   
	@return  
	*/
	CRandGenerator(void)
	{
		int s;
		m_pBuffer = new DWORD[Number];
		m_Ptr = Number * 2;
		__asm
		{
			_asm _emit 0x0f _asm _emit 0x31
				mov s,eax
		}

		m_Seed = s;
	}

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CRandGenerator(void)
	{
		delete[] m_pBuffer;
	}

private:
	/** 产生一批
	@param   
	@param   
	@return  
	*/
	void		Batch(void)
	{
		__asm 
		{
			cld
				mov edi,[ecx]CRandGenerator.m_pBuffer
				mov eax,[ecx]CRandGenerator.m_Seed
				mov ecx,0x1000		;//WRandom::Number
			mov esi,0xE7BD2160

				ALIGN 4
_loop_circle1:
			push ecx
				mov ecx,32
_loop_circle2:
			mov ebx,eax
				shl eax,1
				and ebx,esi;	//select the bit for xor
			mov edx,ebx
				bswap ebx
				xor bx,dx
				xor bh,bl;		// because P only judge one byte
			;// so must XOR to judge the p of whole word 
			jp _next;		//jp equals the xor 
			inc eax
_next:
			dec ecx
				jnz _loop_circle2
				pop ecx
				stosd
				dec ecx
				jnz _loop_circle1
				mov ecx,this
				mov [ecx]CRandGenerator.m_Seed,eax
		}
	}

private:
	DWORD *		m_pBuffer;
	DWORD		m_Seed;
	unsigned	m_Ptr;
};

extern CRandGenerator g_RandGenerator;

#endif // #ifdef SUPPORT_RANDGENERATOR