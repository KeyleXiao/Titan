// ===========================================================================
//	filename: 	encode.h
//
//	author:		������
//
//	created:	2011.12.15
//	
//	purpose:	�ṩһЩ���뺯��
//
// ===========================================================================

#pragma once
#include <string>
/**
@purpose    : URL����
@param	data: ���ݵ�ַ
@param  len	: ���ݳ���
@return		: ���ر������ִ�
*/

std::string encode_url(const unsigned char * data,uint32 len);

/**
@purpose		 : URL����
@param	input_str: �����ַ���
@param  input_len: �����ַ�������
@param output_str: ����ַ��� 
@return			 : ����ܹ���ȷ���뷵��true
*/
bool decode_url(const unsigned char * input_str, uint32 input_len,std::string & output_str);


/** ���ַ����������֣�ת����֧��url��ʽ��unicode���롣�˺���������lua�е���
//�������ڣ�2010��4��8�� by tanjr
@param   pszUrl : Ҫת�����ַ�
@return  ת�����Unicodeֵ(c���͵��ַ���)��ע�⣬��lua�в�֧��std::string����
*/
/*
	ע�⣺����ԭ����ת�������Ὣ _ @ # $ %���������Ҳ������ת���������Զ���¼��ҳ��ʱ��
	���������������ҽ�ɫ�޷��Զ���¼���������һ����ת��������ŵ�
*/

const char * URLEncode_Web(const char *pszUrl) ;


const char * GBKToUTF8(const char *pszUrl);
