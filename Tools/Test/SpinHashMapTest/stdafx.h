// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "CommonDef.h"
#include "Product.h"


#ifndef SAFE_DELETE
#define SAFE_DELETE(o)	{ if(o) { delete (o), (o) = 0; }  }
#endif