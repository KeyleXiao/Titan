#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

// TODO: 在此处引用程序需要的其他头文件
#include "Base.h"
#define new RKT_NEW
using namespace rkt;
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))