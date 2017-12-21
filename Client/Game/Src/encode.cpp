#include "stdafx.h"
#include "encode.h"

__inline uint8 to_hex(const uint8 x)
{
	return x <= 9 ? x + '0': x - 10 + 'A';
}

__inline bool is_hex_digit(int c) {
	switch (c) {
		case '0': return true;
		case '1': return true;
		case '2': return true;
		case '3': return true;
		case '4': return true;
		case '5': return true;
		case '6': return true;
		case '7': return true;
		case '8': return true;
		case '9': return true;
		case 'A': return true;
		case 'B': return true;
		case 'C': return true;
		case 'D': return true;
		case 'E': return true;
		case 'F': return true;
		case 'a': return true;
		case 'b': return true;
		case 'c': return true;
		case 'd': return true;
		case 'e': return true;
		case 'f': return true;
		default: return false;
	}
}

__inline uint8 hex_to_dec(uint8 c) {
	switch (c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'A': return 10;
	case 'B': return 11;
	case 'C': return 12;
	case 'D': return 13;
	case 'E': return 14;
	case 'F': return 15;
	case 'a': return 10;
	case 'b': return 11;
	case 'c': return 12;
	case 'd': return 13;
	case 'e': return 14;
	case 'f': return 15;
	default: 
		assert("hex_to_dec failed");
		return -1;
	}
}
/**
@purpose    : URL����
@param	data: ���ݵ�ַ
@param  len	: ���ݳ���
@return		: ���ر������ִ�
*/

std::string encode_url(const unsigned char * data,uint32 len)
{
	std::string output_str;

	for ( uint32 i=0;i<len;++i )
	{
		if (isalpha((uint8)data[i])||isdigit((uint8)data[i]))
			output_str += data[i];
		else
		{
			output_str += '%';
			output_str += to_hex(data[i] >> 4);
			output_str += to_hex(data[i] % 16);
		}
	}

	return output_str;
}

/**
@purpose		 : URL����
@param	input_str: �����ַ���
@param  input_len: �����ַ�������
@param output_str: ����ַ��� 
@return			 : ����ܹ���ȷ���뷵��true
*/
bool decode_url(const unsigned char * input_str, uint32 input_len,std::string & output_str)
{
	if ( !output_str.empty())
	{
		output_str = "";
	}

	for (int x = 0; x < input_len; )
	{
		if (input_str[x] == '%' && x+2 < input_len && is_hex_digit(input_str[x+1]) && is_hex_digit(input_str[x+2]))
		{
			uint8 hb = hex_to_dec(input_str[x+1]);
			uint8 lb = hex_to_dec(input_str[x+2]);
			uint8 ch = (hb << 4) | lb;
			output_str += ch;
			x = x+3;
		}
		else
		{
			output_str += input_str[x]; 
			++x;
		}
	}

	return true;
}



/** ���ַ����������֣�ת����֧��url��ʽ��unicode���롣�˺���������lua�е���
//�������ڣ�2010��4��8�� by tanjr
@param   pszUrl : Ҫת�����ַ�
@return  ת�����Unicodeֵ(c���͵��ַ���)��ע�⣬��lua�в�֧��std::string����
*/
/*
	ע�⣺����ԭ����ת�������Ὣ _ @ # $ %���������Ҳ������ת���������Զ���¼��ҳ��ʱ��
	���������������ҽ�ɫ�޷��Զ���¼���������һ����ת��������ŵ�
*/

const char * URLEncode_Web(const char *pszUrl)   
{   
	if( pszUrl == NULL )   
		return 0;   

	// Call twice, get Buf Len , A TO W
	int nLength = MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, NULL, 0);   
	WCHAR* pWString = new WCHAR[nLength];   
	MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, pWString, nLength);   

	// Call twice, get Buf Len , W to U
	nLength = WideCharToMultiByte(CP_UTF8, 0, pWString, -1, NULL, 0, NULL, NULL);   
	CHAR* pString = new CHAR[nLength];   
	nLength = WideCharToMultiByte(CP_UTF8, 0, pWString, -1, pString, nLength, NULL, NULL); 

	static char hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};   

	//����strServerҪ�����ɾ�̬�ġ���ΪUnicode�ַ���������ͨ��new�ķ�ʽ��Ϊ�������ñ������ڴ��У�������
	//����strServer����һ����ʱ��������return֮ǰ�ͻᱻ���٣���lua�оͻ�ȡ������ȷ��Unicode����
	static std::string strOutPut; // ע�⣬static�ؼ����ڳ�������ʱִֻ��һ��
	//URLEncodeApi��������ᱻ�������á���string����strOutPut�Ǿ�̬�ģ�����ÿ��ʹ��֮ǰ���������strOutPut�����ݣ�����strOutPut��ֵ��Խ��Խ��
	strOutPut.clear(); // clear()�����ƺ���vc����е�

	for( int i = 0; i < nLength-1; i++ )   
	{   
		register unsigned char c = pString[i];   
		if( isalnum(c) || ispunct(c))    // ���ֻ���ĸ���������    
			strOutPut += c;   
		else if( isspace(c) )        // �����ո�     
			strOutPut += ' '; 
		else                        // ���б���   
		{   
			strOutPut += '%';   
			strOutPut += hex[c / 16];   
			strOutPut += hex[c % 16];   
		}   
	}   

	delete[] pWString;   
	delete[] pString;   

	//strOutPut = StringHelper::toUpper(strOutPut); //���ַ���ת���ɴ�д����Ϊ���ݿ��Ǳ�Ĭ���Ǵ�д��
	return strOutPut.c_str();   
}

const char * GBKToUTF8(const char *pszUrl)  
{
	if( pszUrl == NULL )   
		return 0;   

	static std::string strOutUTF8; // ע�⣬static�ؼ����ڳ�������ʱִֻ��һ��
	strOutUTF8.clear(); // clear()�����ƺ���vc����е�

	WCHAR * str1;  
	int nLength = MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, NULL, 0);  
	str1 = new WCHAR[nLength];  

	MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, str1, nLength);  
	nLength = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);  

	char * str2 = new char[nLength];  
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, nLength, NULL, NULL);  
	strOutUTF8 = str2;  

	delete[]str1;  
	delete[]str2;  

	return strOutUTF8.c_str();  
}