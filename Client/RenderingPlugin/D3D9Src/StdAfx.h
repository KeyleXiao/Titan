#if defined(__WIN32__) || defined(_WIN32)
#include "windows.h"
#endif

#include "d3d9.h"


typedef int INT;
typedef unsigned int uint;
typedef unsigned int UINT;
typedef unsigned long       DWORD;

#define _RenderEngineExport 

#define ErrorLn(x) 
#define my_t2utf8(x) x

#define safeDeleteArray(x) if(x){delete[] x;(x)=0;};
#define safeRelease(x)	if(x){(x)->release();(x)=0;}
#define SAFERELEASE(x)	if(x){(x)->Release();(x)=0;}
extern IDirect3DDevice9* g_D3D9Device;
#define g_pDevice g_D3D9Device

//#define TOD3DMATRIX(m)		(*(D3DMATRIX*)&m)
//#define TOD3DCOLORVALUE(c)	(*(D3DCOLORVALUE*)&c)
//#define TOCOLORVALUE(c)		(*(ColorValue*)&c)
//#define TOD3DVECTOR(v)		(*(D3DVECTOR*)&v)
//#define TOD3DXMATRIX(m)		(*(D3DXMATRIX*)&m)
//#define TOD3DXQUATERNION(q)	(*(D3DXQUATERNION*)&q)
//#define TOD3DXVECTOR3(v)		(*(D3DXVECTOR3*)&v)
//#define TOD3DXVECTOR4(v)		(*(D3DXVECTOR4*)&v)


