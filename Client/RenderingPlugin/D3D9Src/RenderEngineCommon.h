#ifndef __RenderEngineCommon_H__
#define __RenderEngineCommon_H__

#include "StdAfx.h"
#include "BufferDef.h"

struct Vector2
{
public:
	float x,y;
};
struct Vector3
{
public:
	float x,y,z;
};
struct Vector4
{
public:
	float x,y,z,w;
};

struct ColorValue
{
public:
	float r,g,b,a;
};

namespace rkt
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	enum RenderMode
	{
		RENDER_MODE_NORMAL,
		RENDER_MODE_SELECTION
	};

	/** 渲染队列的排序优先级 */
	enum RenderQueueOrder
	{
		RENDER_QUEUE_SHADOW,		//阴影最先画		- 0
		RENDER_QUEUE_OPAQUE,		//不透明对象次之	- 1
		RENDER_QUEUE_TRANSPARENT,	//透明对象			- 2
		RENDER_QUEUE_BLEND,			//混合对象			- 3
		RENDER_QUEUE_ADVANCED,		//FX				- 4
		RENDER_QUEUE_FX,			//PS				- 5
		RENDER_QUEUE_FXEX,			//PS				- 6
		RENDER_QUEUE_MANAGED,		//接管模式			- 7
		RENDER_QUEUE_COUNT
	};

	/** Billboard type. */
	enum BillboardType
	{
		BILLBOARD_NOT_USED,
		BILLBOARD_LOCK_NONE,
		BILLBOARD_LOCK_X,
		BILLBOARD_LOCK_Y,
		BILLBOARD_LOCK_Z
	};

	/** Interpolation type. */
	enum InterpolationType
	{
		INTERPOLATION_NONE,
		INTERPOLATION_LINEAR,
		INTERPOLATION_HERMITE,
		INTERPOLATION_BEZIER
	};

	#define RE_MAX_TEXTURE_COORD_SETS	8
	#define RE_SHADOW_TRANSPARENCY		64
	#define RE_MAX_SIMULTANEOUS_LIGHTS	8

	/** An enumeration describing which material properties should track the vertex colours */
	enum TrackVertexColorEnum
	{
		TVC_NONE        = 0x0,
		TVC_AMBIENT     = 0x1,        
		TVC_DIFFUSE     = 0x2,
		TVC_SPECULAR    = 0x4,
		TVC_EMISSIVE    = 0x8,
		TVC_AMBIENT_AND_DIFFUSE	=	0x3
	};

	/** Light Type. */
	enum LightType
	{
		/// Point light sources give off light equally in all directions, so require only position not direction
		LT_POINT=0,
		/// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
		LT_DIRECTIONAL=1,
		/// Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
		LT_SPOTLIGHT=2
	};

	/** Fog modes. */
	enum FogMode
	{
		/// No fog. Duh.
		FOG_NONE,
		/// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
		FOG_EXP,
		/// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
		FOG_EXP2,
		/// Fog density increases linearly between the start and end distances
		FOG_LINEAR
	};

	/** Primitive Type. */
	enum PrimitiveType
	{
		PT_LINES,
		PT_LINE_STRIP,
		//PT_LINE_LOOP,
		PT_TRIANGLES,
		PT_TRIANGLE_STRIP,
		PT_TRIANGLE_FAN,
		//PT_QUADS,
		//PT_QUAD_STRIP,
		PT_POINTS
	};

	/** The broad type of detail for rendering. */
	enum FillMode
	{
		/// Only points are rendered.
		FM_POINTS = 1,
		/// Wireframe models are rendered.
		FM_WIREFRAME = 2,
		/// Solid polygons are rendered.
		FM_SOLID = 3
	};

	enum CompareFunction
	{
		CMPF_ALWAYS_FAIL,
		CMPF_ALWAYS_PASS,
		CMPF_LESS,
		CMPF_LESS_EQUAL,
		CMPF_EQUAL,
		CMPF_NOT_EQUAL,
		CMPF_GREATER_EQUAL,
		CMPF_GREATER
	};

	/** Hardware culling modes based on vertex winding.
	This setting applies to how the hardware API culls triangles it is sent. */
	enum CullingMode
	{
		/// Hardware never culls triangles and renders everything it receives.
		CULL_NONE = 1,
		/// Hardware culls triangles whose vertices are listed clockwise in the view (default).
		CULL_CLOCKWISE = 2,
		/// Hardware culls triangles whose vertices are listed anticlockwise in the view.
		CULL_COUNTERCLOCKWISE = 3
	};

	/** Blending factors for manually blending objects with the scene. If there isn't a predefined
	SceneBlendType that you like, then you can specify the blending factors directly to affect the
	combination of object and the existing scene. See Material::setSceneBlending for more details.
	*/
	enum SceneBlendFactor
	{
		SBF_ONE,
		SBF_ZERO,
		SBF_DEST_COLOR,
		SBF_SOURCE_COLOR,
		SBF_ONE_MINUS_DEST_COLOR,
		SBF_ONE_MINUS_SOURCE_COLOR,
		SBF_DEST_ALPHA,
		SBF_SOURCE_ALPHA,
		SBF_ONE_MINUS_DEST_ALPHA,
		SBF_ONE_MINUS_SOURCE_ALPHA

	};

	/** Filtering options for textures / mipmaps. */
	enum FilterOptions
	{
		/// No filtering, used for FILT_MIP to turn off mipmapping
		FO_NONE,
		/// Use the closest pixel
		FO_POINT,
		/// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
		FO_LINEAR,
		/// Similar to FO_LINEAR, but compensates for the angle of the texture plane
		FO_ANISOTROPIC
	};

	/** Blend modes for models / particle systems / ribbon system. */
	enum BlendMode
	{
		BM_OPAQUE=0,
		BM_TRANSPARENT,
		BM_BLEND_ALPHA,
		BM_ADDITIVE_OLD,
		BM_ADDITIVE_ALPHA,
		BM_MODULATE_OLD,
		BM_MODULATE2X_COLOR,
		BM_MODULATE_COLOR,
		BM_MODULATE_ALPHA,
		BM_MODULATE2X_ALPHA,
		BM_ADDITIVEHX_COLOR,
		BM_ADDITIVE_COLOR,
		BM_EXCLUSIONHX_ALPHA,
		BM_EXCLUSION_ALPHA,
		BM_EXCLUSIONHX_COLOR,
		BM_EXCLUSION_COLOR,
		BM_ZONLY=99,
	};

	/** Texture addressing mode. */
	enum TextureAddressingMode
	{
		/// Texture wraps at values over 1.0
		TAM_WRAP,
		/// Texture mirrors (flips) at joins over 1.0
		TAM_MIRROR,
		/// Texture clamps at 1.0
		TAM_CLAMP_TO_EDGE,
		//
		TAM_CLAMP
	};

	/// Enum describing the ways to generate texture coordinates
	enum TexCoordCalcMethod
	{
		/// No calculated texture coordinates
		TEXCALC_NONE,
		/// Environment map based on vertex normals
		TEXCALC_ENVIRONMENT_MAP,
		/// Environment map based on vertex positions
		TEXCALC_ENVIRONMENT_MAP_PLANAR,
		TEXCALC_ENVIRONMENT_MAP_REFLECTION,
		TEXCALC_ENVIRONMENT_MAP_NORMAL,
		/// Projective texture
		TEXCALC_PROJECTIVE_TEXTURE
	};

	/** Enum identifying special mipmap numbers
    */
	enum RequestMipmap
	{
		/// Auto Generate mipmaps up to 1x1
		MIP_UNLIMITED = -2,
		/// Use TextureManager default
		MIP_DEFAULT = -1,
		/// Only One
		MIP_NOMIPMAP = 0,
		/// Use From File
		MIP_FROM_FILE = 1
	};

	/// Enum describing the various actions which can be taken onthe stencil buffer
	enum StencilOperation
	{
		/// Leave the stencil buffer unchanged
		SOP_KEEP,
		/// Set the stencil value to zero
		SOP_ZERO,
		/// Set the stencil value to the reference value
		SOP_REPLACE,
		/// Increase the stencil value by 1, clamping at the maximum value
		SOP_INCREMENT,
		/// Decrease the stencil value by 1, clamping at 0
		SOP_DECREMENT,
		/// Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
		SOP_INCREMENT_WRAP,
		/// Decrease the stencil value by 1, wrapping when decrementing 0
		SOP_DECREMENT_WRAP,
		/// Invert the bits of the stencil buffer
		SOP_INVERT
	};
	/** @} */

	/// 多流的相关设置
	enum VertexDeclarationType
	{
		VD_NULL,			//无效的，可以用来清除VD
		VD_POS_COLOR,		//多流 - 位置和颜色
		VD_POSNORMAL_TEX0,	//多流 - 位置和法线 + 贴图
		VD_POSNORMAL_TEX0COLOR,	//多流 - 位置和法线 + 贴图和颜色
		VD_POSTEX0,			//单流 - 位置和贴图
		VD_POSCOLORTEX0,	//单流 - 位置颜色贴图，顺序不能错，在VIA/S3G Unichrome IGP的显卡上，顺序不对不能跑
		VD_POS_TEX0_TEX1,	//多流 - 位置 + 贴图0 + 贴图1
		VD_POSCOLOR,		//单流 - 位置颜色
		VD_POS_TEX0,		//多流 - 位置 + 贴图
		VD_POSTEX0TEX1,		//单流 - 位置贴图0贴图1
		VD_POS4TEX0,		//单流 - 位置4和贴图
		VD_POS3NORMAL_WEIGHTSBONEINDICES,	// Added by Zhen.YUE @2009-6-9
							//多流 - 位置3和法线 + 权重和骨骼索引					----- 用于骨骼动画
		VD_POS3NORMAL_TEX0_WEIGHTSBONEINDICES,
							//多流 - 位置3和法线 + 纹理坐标 + 权重、骨骼索引		----- 用于骨骼动画
		VD_POS3NORMAL_TEX0COLOR_WEIGHTSBONEINDICES,
							//多流 - 位置3和法线 + 纹理坐标和颜色 + 权重和骨骼索引	----- 用于骨骼动画
		VD_POS3_WEIGHTSBONEINDICES,	// Added by unnamed @2009-8-16
							//多流 - 位置3 + 权重和骨骼索引					----- 用于骨骼动画
		VD_POS3_TEX0_WEIGHTSBONEINDICES,
							//多流 - 位置3 + 纹理坐标 + 权重、骨骼索引		----- 用于骨骼动画
		VD_POS3_TEX0COLOR_WEIGHTSBONEINDICES,
							//多流 - 位置3 + 纹理坐标和颜色 + 权重和骨骼索引	----- 用于骨骼动画
		VD_POS_TEX0COLOR,	//多流 - 位置 + 贴图和颜色
		VD_POS,             //单流，位置
		VD_MEM,             //单流，内存数据格式
		VD_POSTEX3,             //单流，位置+float3纹理
		VD_MEM2,             //单流，内存数据格式，双uv
		VD_POS_TEX0TEX1COLOR,	//多流 - 位置 + 贴图和颜色
		VD_POS_TEX0Tex1,
		VD_POSNORMALCOLORTEX0,		//单流 - 位置+法线+diffuse+贴图0,一定要按这个顺序，否则在某些显卡不正常
		VD_POST,  //单流，变换后顶点格式   post
		VD_POSTCOLOR,   //单流，变换后顶点格式  post+color
		VD_POSTTEX0,    //单流，变换后顶点格式   post+tex0
		VD_POSTCOLORTEX0,    //单流，变换后顶点格式 post+color+tex0
		VD_POSTTEX0TEX1,    //单流，变换后顶点格式 post+tex0+tex1

		VD_POSNORMAL_TEX0TEX1COLOR,	//多流 - 位置法线 + 贴图和颜色
		VD_POSNORMAL_TEX0TEX1,

		VD_POSTEX0TEX1TEX2,    //单流 —— 位置，贴图1贴图2贴图3

		VD_COUNT
	};

	struct PosColorTex0
	{
		Vector3 pos;
		uint color;
		Vector2 tc;
	};

	struct PosTex0
	{
		Vector3 pos;
		Vector2 tc;
	};

	struct PosColor
	{
		Vector3 pos;
		uint color;
	};

	struct PosTex0Tex1
	{
		Vector3	 pos;
		Vector2 tc1;
		Vector2 tc2;
	};

	struct Pos4Tex0
	{
		Vector4 pos;
		Vector2 tc;
	};

	struct PosTex0Tex1Tex2
	{
		Vector3	 pos;
		Vector2 tc1;
		Vector2 tc2;
		Vector2 tc3;
	};

	/// 贴图操作
	enum TextureOp
	{
		TO_DISABLE = 1,
		TO_SELECTARG1 = 2,
		TO_SELECTARG2 = 3,
		TO_MODULATE = 4,
		TO_MODULATE2X = 5,
		TO_MODULATE4X = 6,
		TO_ADD = 7,
		TO_ADDSIGNED = 8,
		TO_ADDSIGNED2X = 9,
		TO_SUBTRACT = 10,
		TO_ADDSMOOTH = 11,
		TO_BLENDDIFFUSEALPHA = 12,
		TO_BLENDTEXTUREALPHA = 13,
		TO_BLENDFACTORALPHA = 14,
		TO_BLENDTEXTUREALPHAPM = 15,
		TO_BLENDCURRENTALPHA = 16,
		TO_PREMODULATE = 17,
		TO_MODULATEALPHA_ADDCOLOR = 18,
		TO_MODULATECOLOR_ADDALPHA = 19,
		TO_MODULATEINVALPHA_ADDCOLOR = 20,
		TO_MODULATEINVCOLOR_ADDALPHA = 21,
		TO_BUMPENVMAP = 22,
		TO_BUMPENVMAPLUMINANCE = 23,
		TO_DOTPRODUCT3 = 24,
		TO_MULTIPLYADD = 25,
		TO_LERP = 26,
	};

	/// 贴图参数
	enum TextureArg
	{
		TA_CONSTANT,
		TA_CURRENT,
		TA_DIFFUSE,
		TA_SELECTMASK,
		TA_SPECULAR,
		TA_TEMP,
		TA_TEXTURE,
		TA_TFACTOR,
	};

	/// 常用的贴图混合模式
	enum TextureBlending
	{
		TB_NONE,		//关闭贴图混合
		TB_ALPHAMAP,	//0 - AlphaMap 1 - Texture，一般用于地表渲染
		TB_DIFFUSEMAP,	//使用Diffuse颜色值的Alpha混合贴图，类似于OpenGL中的顶点Alpha
		TB_TFACTOR,		//使用颜色来模拟混合调色，以取代用光照实现的调色
		TB_DIFFUSECOLORMAP,	//使用Diffuse颜色值、TFactor颜色以及贴图来获得最终效果
		TB_TEXTUREFLOWMAP	,//stage1的贴图流动
		TB_ALPHALIGHTMAP,//0 - AlphaMap+lightColor  1 - Texture*LightColor，一般用于地表渲染，带光照
		TB_ENVIRMAP	,   //环境贴图，0通道普通纹理，1通道允许envirmap或者cubic map
		TB_DIFFUSEMAP_ADD, ////使用Diffuse颜色值的Alpha混合贴图，用加法混合方式
		TB_NORMALMAPPING,//法线贴图,0通道法线贴图，1通道普通纹理
		TB_BUMPMAPPING,//凹凸贴图，0通道普通纹理，1通道凹凸纹理，2通道环境纹理
	};

	//贴图特效
	enum MapEffect
	{
		ME_NONE = 0,//无
		ME_BUMPMAPPING,//凹凸贴图,需要高度图，环境贴图,固定管线实现，0通道原始贴图，1通道凹凸贴图（由高度图计算而来，IRenderSystemCalcuBumpMap），2通道环境贴图
		ME_NORMALMAPPING,//法线贴图，需要法线贴图，固定管线实现,0通道法线贴图，1通道原始贴图
	};

	// _GAMEMARKET_VER_BEGAIN
	//材质特效
	enum MatrtialEffect
	{
		MTE_NONE = 0,//无
		MTE_GLOW,//光晕效果，shader实现，需要PS2.0
	};
	// _GAMEMARKET_VER_END

	struct SurfaceParam
	{
		ColorValue ambient;
		ColorValue diffuse;
		ColorValue specular;
		ColorValue emissive;
		float shininess;
	};

	/// 设备丢失相关 - D3D下才会丢失
	enum DeviceStatus
	{
		DS_OK,			//设备正常
		DS_LOST,		//设备丢了
		DS_NOTRESET,	//设备需要Reset
		DS_FATALERROR,	//硬件错误
	};

	enum ImageFormat
	{
		IF_TGA,
		IF_JPG,
		IF_BMP,
		IF_DDS,
		IF_PNG
	};

	enum ResPool
	{
		POOL_MANAGED,
		POOL_DEFAULT,
		POOL_SYSTEMMEM,
	};


	enum eMaterial
	{
		eMaterial_0 = 0,
		eMaterial_1,
		eMaterial_Size,
	};

	enum ColorSource
	{
		COLORSRC_COLOR,
		COLORSRC_MATERIAL,
	};

	enum CubeTextureFaceType
	{
		CUBEMAP_FACE_POSITIVE_X=1,
		CUBEMAP_FACE_NEGATIVE_X,
		CUBEMAP_FACE_POSITIVE_Y,
		CUBEMAP_FACE_NEGATIVE_Y,
		CUBEMAP_FACE_POSITIVE_Z,
		CUBEMAP_FACE_NEGATIVE_Z
	};
	
	struct Lock_Rect
	{
		INT Pitch;
		void* pBits;
	};
}

#endif