/*******************************************************************
** 文件名:	CharacterEncrypt.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/30/2010   15:38
** 版  本:	1.0
** 描  述:	
** 应  用:  简单字符串加密器(字符串随机置换)	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __CHARACTER_ENCRYPT_H__
#define __CHARACTER_ENCRYPT_H__

#include "Singleton.h"
#include <map>
#include <vector>

/** 
   将字母表随机乱序,以达到加密的目的
*/
class CharacterEncrypt : public Singleton<CharacterEncrypt>
{
protected:
	typedef   std::map<char,char>   ENCRYPT_DICTIONARY;
	typedef   std::map<int,ENCRYPT_DICTIONARY>  DIC_MAP;
	DIC_MAP   m_EncryptDic;              // 加密字典
	DIC_MAP   m_DecryptDic;              // 解密字典

	std::map<int,DWORD>       m_useLog;  // 使用使用记录,清理最近10分钟内没有使用的字典

public:
	// 生成字典
	void GenerateDictionary( int key )
	{
		static char characters[] = 
		{
				'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
				'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
				'0','1','2','3','4','5','6','7','8','9'
		};

		static unsigned char encry_characters[] = 
		{
			129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,200,201,151,152,153,154,
			159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,
			190,191,192,193,194,195,196,197,198,199
		};

		int total = sizeof(encry_characters)/sizeof(unsigned char);

		std::vector<char> rand_index;
		RandomDictionary( rand_index,key,total );

		int index = 0;
		for ( int n=0;n<sizeof(characters)/sizeof(char);++n )
		{
			index = rand_index[n];
			m_EncryptDic[key][characters[n]] = encry_characters[ index ];
			m_DecryptDic[key][encry_characters[index]]= characters[n];
		}
	}

	// 随机生成字典，参考微软空当接龙洗牌算法
	void RandomDictionary( std::vector<char> & rand_index,int key,int total )
	{
		std::vector<char> raw_index( total );
		for ( int i=0;i<total;++i )
		{
			raw_index[i] = i;
		}

		rand_index.clear();

		srand(key);

		while( total>0 )
		{
			int d = rand() % RAND_MAX;
			int index = d % total;
			rand_index.push_back( (char)raw_index[index] );  // 随机抽走一张
			raw_index[index] = raw_index[total-1];           // 最后一张往前补
			--total;
		}

		srand(GetTickCount());
	}

	// 清理字典,10分钟内没使用则自动清除
	void CleanDictionary()
	{
		DIC_MAP::iterator it     = m_EncryptDic.begin();

		DWORD now = GetTickCount();

		for ( ;it!=m_EncryptDic.end(); )
		{
			if ( now - m_useLog[it->first] > 10*60*1000 )
			{
				m_DecryptDic.erase( it->first );
				m_useLog.erase( it->first );
				it = m_EncryptDic.erase( it );
			}else
			{
				++it;
			}
		}
	}

	// 取得字典
	ENCRYPT_DICTIONARY * GetDictionary( int key,bool is_encrypt )
	{
		m_useLog[key] = GetTickCount();

		DIC_MAP * pDicMap = is_encrypt ? &m_EncryptDic : &m_DecryptDic;

		DIC_MAP::iterator it = pDicMap->find( key );
		if ( it == pDicMap->end() )
		{
			GenerateDictionary( key );
			
			if ( pDicMap->size() > 1024 )
			{
				CleanDictionary();
			}

			it = pDicMap->find( key );
			if ( it == pDicMap->end() )
			{
				ErrorLn("生成字典失败key="<<key);
				return 0;
			}
		}

		return &(it->second);;
	}

	/** 
	请求加密一段字符串
	@param  str : 要加密的字符串,必须以0结尾,加密后会直接修改该字符串
	@param  key : 密钥,必须是一个正整数
	*/
	void Encrypt( char * str,int key )
	{
		if ( str==0 || key<0 )
		{
			return;
		}

		ENCRYPT_DICTIONARY * dic = GetDictionary( key,true );
		if ( dic==0 )
		{
			return;
		}

		while( *str )
		{
			ENCRYPT_DICTIONARY::iterator it = dic->find(*str);
			if ( it!=dic->end() )
			{
				*str = it->second;
			}

			++str;
		}
	}

	/** 
	请求解密一段字符串
	@param  str : 要解密的字符串,必须以0结尾,加密后会直接修改该字符串
	@param  key : 密钥,必须是一个正整数
	*/
	void Decrypt( char * str,int key )
	{
		if ( str==0 || key<0 )
		{
			return;
		}

		ENCRYPT_DICTIONARY * dic = GetDictionary( key,false );
		if ( dic==0 )
		{
			return;
		}

		while( *str )
		{
			ENCRYPT_DICTIONARY::iterator it = dic->find(*str);
			if ( it!=dic->end() )
			{
				*str = it->second;
			}

			++str;
		}
	}

	void Decrypt( char * str,int len,int key )
	{
		if ( str==0 || key<0 )
		{
			return;
		}

		ENCRYPT_DICTIONARY * dic = GetDictionary( key,false );
		if ( dic==0 )
		{
			return;
		}

		for ( int i=0;i<len;++i )
		{
			ENCRYPT_DICTIONARY::iterator it = dic->find(*(str+i));
			if ( it!=dic->end() )
			{
				*(str+i) = it->second;
			}
		}
	}

	/** 
	请求解密一段字符串
	@param  str : 要解密的字符串,必须以0结尾,加密后会直接修改该字符串
	@param  key : 密钥,必须是一个正整数
	*/
	bool DecryptEx( char * str,int len,int key )
	{
		if ( str==0 || key<0 )
		{
			return false;
		}

		ENCRYPT_DICTIONARY * dic = GetDictionary( key,false );
		if ( dic==0 )
		{
			return false;
		}

		for ( int i=0;i<len;++i )
		{
			if ( *(str+i)==0 )
			{
				return true;
			}

			ENCRYPT_DICTIONARY::iterator it = dic->find(*(str+i));
			if ( it!=dic->end() )
			{
				*(str+i) = it->second;
			}else
			{
				if ( dic->find( *(str+i) )!= dic->end() )
				{
					return false;
				}
			}
		}

		return true;
	}
};

#endif//__CHARACTER_ENCRYPT_H__