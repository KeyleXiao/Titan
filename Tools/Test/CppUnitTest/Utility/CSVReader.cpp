#include "stdafx.h"
#include "CSVReader.h"
const char* CParamTraits<char>::type_str		= "char";
const char* CParamTraits<char>::first_format	= "%c%n";
const char* CParamTraits<char>::second_format	= ",%c%n";

const char* CParamTraits<int>::type_str			= "int";
const char* CParamTraits<int>::first_format		= "%d%n";
const char* CParamTraits<int>::second_format	= ",%d%n";

const char* CParamTraits<float>::type_str		= "float";
const char* CParamTraits<float>::first_format	= "%f%n";
const char* CParamTraits<float>::second_format	= ",%f%n";

const char* CParamTraits<double>::type_str		= "double";
const char* CParamTraits<double>::first_format	= "%g%n";
const char* CParamTraits<double>::second_format	= ",%g%n";

const char* CParamTraits<string>::type_str		= "string";
const char* CParamTraits<string>::first_format	= "%[^,]%n";
const char* CParamTraits<string>::second_format	= ",%[^,]%n";

