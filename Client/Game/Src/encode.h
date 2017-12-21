// ===========================================================================
//	filename: 	encode.h
//
//	author:		唐其文
//
//	created:	2011.12.15
//	
//	purpose:	提供一些编码函数
//
// ===========================================================================

#pragma once
#include <string>
/**
@purpose    : URL编码
@param	data: 数据地址
@param  len	: 数据长度
@return		: 返回编码后的字串
*/

std::string encode_url(const unsigned char * data,uint32 len);

/**
@purpose		 : URL解码
@param	input_str: 输入字符串
@param  input_len: 输入字符串长度
@param output_str: 输出字符串 
@return			 : 如果能够正确解码返回true
*/
bool decode_url(const unsigned char * input_str, uint32 input_len,std::string & output_str);


/** 将字符（包括汉字）转换成支持url格式的unicode编码。此函数可以在lua中调用
//创建日期：2010年4月8日 by tanjr
@param   pszUrl : 要转换的字符
@return  转换后的Unicode值(c类型的字符串)，注意，在lua中不支持std::string类型
*/
/*
	注意：由于原来的转换函数会将 _ @ # $ %等特殊符号也进行了转换，导致自动登录网页的时候
	出现有特殊符号玩家角色无法自动登录，故再添加一个不转换特殊符号的
*/

const char * URLEncode_Web(const char *pszUrl) ;


const char * GBKToUTF8(const char *pszUrl);
