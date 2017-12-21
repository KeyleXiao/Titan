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
@purpose    : URL编码
@param	data: 数据地址
@param  len	: 数据长度
@return		: 返回编码后的字串
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
@purpose		 : URL解码
@param	input_str: 输入字符串
@param  input_len: 输入字符串长度
@param output_str: 输出字符串 
@return			 : 如果能够正确解码返回true
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



/** 将字符（包括汉字）转换成支持url格式的unicode编码。此函数可以在lua中调用
//创建日期：2010年4月8日 by tanjr
@param   pszUrl : 要转换的字符
@return  转换后的Unicode值(c类型的字符串)，注意，在lua中不支持std::string类型
*/
/*
	注意：由于原来的转换函数会将 _ @ # $ %等特殊符号也进行了转换，导致自动登录网页的时候
	出现有特殊符号玩家角色无法自动登录，故再添加一个不转换特殊符号的
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

	//变量strServer要声明成静态的。因为Unicode字符串编码是通过new的方式（为了能永久保存在内存中）创建的
	//否则strServer就是一个临时变量，在return之前就会被销毁，在lua中就获取不到正确的Unicode编码
	static std::string strOutPut; // 注意，static关键字在程序运行时只执行一次
	//URLEncodeApi这个函数会被经常调用。而string对象strOutPut是静态的，所以每次使用之前必须先清空strOutPut的内容！否则strOutPut的值会越来越大
	strOutPut.clear(); // clear()函数似乎是vc里独有的

	for( int i = 0; i < nLength-1; i++ )   
	{   
		register unsigned char c = pString[i];   
		if( isalnum(c) || ispunct(c))    // 数字或字母或特殊符号    
			strOutPut += c;   
		else if( isspace(c) )        // 包含空格     
			strOutPut += ' '; 
		else                        // 进行编码   
		{   
			strOutPut += '%';   
			strOutPut += hex[c / 16];   
			strOutPut += hex[c % 16];   
		}   
	}   

	delete[] pWString;   
	delete[] pString;   

	//strOutPut = StringHelper::toUpper(strOutPut); //将字符串转换成大写，因为数据库那边默认是大写的
	return strOutPut.c_str();   
}

const char * GBKToUTF8(const char *pszUrl)  
{
	if( pszUrl == NULL )   
		return 0;   

	static std::string strOutUTF8; // 注意，static关键字在程序运行时只执行一次
	strOutUTF8.clear(); // clear()函数似乎是vc里独有的

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