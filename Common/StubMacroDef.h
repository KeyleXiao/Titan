/*******************************************************************
** 文件名:	StubMacroDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/13/2014
** 版  本:	1.0
** 描  述:	Stub宏定义文件

			
********************************************************************/


#ifndef __COMMON_STUB_MACRO_DEF_H__
#define __COMMON_STUB_MACRO_DEF_H__


#ifdef OBUF
#undef OBUF
#endif
#define OBUF(t_data)	\
	obuf256 t_data;		\
	t_data << getServiceType();

#define __STUB_INNER_HANDLER_MACRO__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 定义宏方便stub书写
#define assertBreak  __debugbreak /*__asm{ int 3}*/




#ifdef VERSEION_DEBUG
#define BUILD_DEBUG_INFO(FUNC, pMsg)    sstrcpyn(pMsg->szName, #FUNC, sizeof(pMsg->szName))
#else
#define BUILD_DEBUG_INFO(FUNC, pMsg) 
#endif

// 构建一个没有参数的消息现场
#define  BUILD_MSG_CONTEXT_VOID(FUNC)								\
	int nMsgLen = sizeof(SERVICE_MESSAGE);							\
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
	pMsg->context_len = 0;						   					\
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
    BUILD_DEBUG_INFO(FUNC, pMsg);

// 构建一个消息现场,函数调用的参数都打包在PARAM定义的结构里面
#define  BUILD_MSG_CONTEXT(FUNC,PARAM)								\
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM);			\
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
	pMsg->context_len = sizeof(PARAM);								\
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM * pParam= (PARAM*)(pMsg+1);							 	\
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出参数
#define  GET_MSG_PARAM(PARAM,PTR)    			                    \
	if ( pMsg->context_len!=sizeof(PARAM) )		        			\
		{assertBreak;return false;}									\
	PTR= (PARAM*)(pMsg->context);	    			 	            \

// 构建一个带一个参数的消息现场
#define  BUILD_MSG_CONTEXT_1(FUNC,PARAM,VALUE)  					\
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM);			\
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
	pMsg->context_len = sizeof(PARAM);								\
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM * pParam= (PARAM*)(pMsg+1);							 	\
	*pParam = VALUE;                                                \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出1个参数
#define  GET_MSG_PARAM_1(PARAM1,VALUE1)			                    \
	if ( pMsg->context_len!=sizeof(PARAM1) )		    			\
		{assertBreak;return false;}									\
	PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
	VALUE1 = *pParam1;                                              \


// 构建一个带两个参数的消息现场
#define  BUILD_MSG_CONTEXT_2(FUNC,PARAM1,VALUE1,PARAM2,VALUE2)  	      \
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2);  \
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
	pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2);				\
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM1 * pParam1= (PARAM1*)(pMsg+1);						 	\
	*pParam1 = VALUE1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	*pParam2 = VALUE2;                                              \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出2个参数
#define  GET_MSG_PARAM_2(PARAM1,VALUE1,PARAM2,VALUE2)               \
	if ( pMsg->context_len!=sizeof(PARAM1)+sizeof(PARAM2) )			\
		{assertBreak;return false;}									\
	PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
	VALUE1 = *pParam1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	VALUE2 = *pParam2;                                              \

// 构建一个带三个参数的消息现场
#define  BUILD_MSG_CONTEXT_3(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3)  	         \
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3);  \
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		                     \
	pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3);				     \
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM1 * pParam1= (PARAM1*)(pMsg+1);						 	\
	*pParam1 = VALUE1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	*pParam2 = VALUE2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	*pParam3 = VALUE3;                                              \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出3个参数
#define  GET_MSG_PARAM_3(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3) \
	if ( pMsg->context_len!=sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3) )	\
		{assertBreak;return false;}									\
	PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
	VALUE1 = *pParam1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	VALUE2 = *pParam2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	VALUE3 = *pParam3;                                              \

// 构建一个带四个参数的消息现场
#define  BUILD_MSG_CONTEXT_4(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4)  	         \
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4);   \
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		                                     \
	pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4);				     \
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM1 * pParam1= (PARAM1*)(pMsg+1);						 	\
	*pParam1 = VALUE1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	*pParam2 = VALUE2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	*pParam3 = VALUE3;                                              \
	PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
	*pParam4 = VALUE4;                                              \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \


// 从消息现场中取出4个参数
#define  GET_MSG_PARAM_4(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4)               \
	if ( pMsg->context_len!=sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4) )		\
		{assertBreak;return false;}									\
	PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
	VALUE1 = *pParam1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	VALUE2 = *pParam2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	VALUE3 = *pParam3;                                              \
	PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
	VALUE4 = *pParam4;                                              \

// 构建一个带五个参数的消息现场
#define  BUILD_MSG_CONTEXT_5(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5)  	         \
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5);\
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		                                     \
	pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5);				     \
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM1 * pParam1= (PARAM1*)(pMsg+1);						 	\
	*pParam1 = VALUE1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	*pParam2 = VALUE2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	*pParam3 = VALUE3;                                              \
	PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
	*pParam4 = VALUE4;                                              \
	PARAM5 * pParam5= (PARAM5*)(pParam4+1);						 	\
	*pParam5 = VALUE5;                                              \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出5个参数
#define  GET_MSG_PARAM_5(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5)               \
	if ( pMsg->context_len!=sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5) )	  \
		{assertBreak;return false;}									\
	PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
	VALUE1 = *pParam1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	VALUE2 = *pParam2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	VALUE3 = *pParam3;                                              \
	PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
	VALUE4 = *pParam4;                                              \
	PARAM5 * pParam5= (PARAM5*)(pParam4+1);						 	\
	VALUE5 = *pParam5;                                              \

// 构建一个带六个参数的消息现场
#define  BUILD_MSG_CONTEXT_6(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6)  	         \
	int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5)+sizeof(PARAM6);\
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		                                     \
	pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5)+sizeof(PARAM6);				     \
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
	PARAM1 * pParam1= (PARAM1*)(pMsg+1);						 	\
	*pParam1 = VALUE1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	*pParam2 = VALUE2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	*pParam3 = VALUE3;                                              \
	PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
	*pParam4 = VALUE4;                                              \
	PARAM5 * pParam5= (PARAM5*)(pParam4+1);						 	\
	*pParam5 = VALUE5;                                              \
	PARAM6 * pParam6= (PARAM6*)(pParam5+1);						 	\
	*pParam6 = VALUE6;                                              \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出6个参数
#define  GET_MSG_PARAM_6(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6)               \
	if ( pMsg->context_len!=sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5)+sizeof(PARAM6) )	  \
		{assertBreak;return false;}									\
	PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
	VALUE1 = *pParam1;                                              \
	PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
	VALUE2 = *pParam2;                                              \
	PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
	VALUE3 = *pParam3;                                              \
	PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
	VALUE4 = *pParam4;                                              \
	PARAM5 * pParam5= (PARAM5*)(pParam4+1);						 	\
	VALUE5 = *pParam5;                                              \
	PARAM6 * pParam6= (PARAM6*)(pParam5+1);						 	\
	VALUE6 = *pParam6;                                              \


// 构建一个带七个参数的消息现场
#define  BUILD_MSG_CONTEXT_7(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6,PARAM7,VALUE7)  	         \
    int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5)+sizeof(PARAM6)+sizeof(PARAM7);\
    SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		                                     \
    pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5)+sizeof(PARAM6)+sizeof(PARAM7);				     \
    auto temp_adrr = &FUNC;                                         \
    pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			\
    PARAM1 * pParam1= (PARAM1*)(pMsg+1);						 	\
    *pParam1 = VALUE1;                                              \
    PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
    *pParam2 = VALUE2;                                              \
    PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
    *pParam3 = VALUE3;                                              \
    PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
    *pParam4 = VALUE4;                                              \
    PARAM5 * pParam5= (PARAM5*)(pParam4+1);						 	\
    *pParam5 = VALUE5;                                              \
    PARAM6 * pParam6= (PARAM6*)(pParam5+1);						 	\
    *pParam6 = VALUE6;                                              \
    PARAM7 * pParam7= (PARAM7*)(pParam6+1);						 	\
    *pParam7 = VALUE7;                                              \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中取出7个参数
#define  GET_MSG_PARAM_7(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6,PARAM7,VALUE7)               \
    if ( pMsg->context_len!=sizeof(PARAM1)+sizeof(PARAM2)+sizeof(PARAM3)+sizeof(PARAM4)+sizeof(PARAM5)+sizeof(PARAM6)+sizeof(PARAM7) )	  \
        {assertBreak;return false;}									\
        PARAM1 * pParam1= (PARAM1*)(pMsg->context);	    			 	\
        VALUE1 = *pParam1;                                              \
        PARAM2 * pParam2= (PARAM2*)(pParam1+1);						 	\
        VALUE2 = *pParam2;                                              \
        PARAM3 * pParam3= (PARAM3*)(pParam2+1);						 	\
        VALUE3 = *pParam3;                                              \
        PARAM4 * pParam4= (PARAM4*)(pParam3+1);						 	\
        VALUE4 = *pParam4;                                              \
        PARAM5 * pParam5= (PARAM5*)(pParam4+1);						 	\
        VALUE5 = *pParam5;                                              \
        PARAM6 * pParam6= (PARAM6*)(pParam5+1);						 	\
        VALUE6 = *pParam6;                                              \
        PARAM7 * pParam7= (PARAM7*)(pParam6+1);						 	\
        VALUE7 = *pParam7;                                              \

// 构建一个消息现场,函数调用的参数都打包BUF对象中
#define  BUILD_MSG_BUFFER(FUNC,BUF)								    \
	int nMsgLen = sizeof(SERVICE_MESSAGE)+BUF.size();				\
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
	pMsg->context_len = BUF.size();					         		\
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        		\
	memcpy( pMsg->context,BUF.data(),pMsg->context_len );           \
	BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 构建一个消息现场,函数调用的参数都打包BUF对象中
#define  BUILD_MSG_BUFFER_LEN(FUNC,LEN)								\
	int nMsgLen = sizeof(SERVICE_MESSAGE)+LEN;				        \
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
	pMsg->context_len = LEN;					             		\
	auto temp_adrr = &FUNC;                                         \
	pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        		\
	rkt::ofixbuf t_data(pMsg->context,LEN );			            \
	BUILD_DEBUG_INFO(FUNC, pMsg);                                   \

// 从消息现场中返回返回值
#define RETURN_FROM_MSG_BUF(TYPE)                                   \
	if ( resultBuf.good() && resultBuf.size()==sizeof(TYPE)  )      \
            return *(TYPE*)resultBuf.data();                        \

// 从消息现场中返回字符串
#define RETURN_STRING_FROM_MSG_BUF()                                \
    if ( resultBuf.good() && resultBuf.size() > 0    ) {            \
        ibuffer t_in(resultBuf.data(),resultBuf.size());            \
        string vReturn;                                             \
        t_in >> vReturn;                                            \
        if ( t_in.good() ) return vReturn;                          \
    }                                                               \
    return "";                                                      \

// 构建一个带一个参数的消息现场
#define  BUILD_MSG_CONTEXT_EX_1(FUNC,PARAM,VALUE)  					\
    int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM);			\
    SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		\
    pMsg->context_len = sizeof(PARAM);								\
    auto temp_adrr = &FUNC;                                         \
    pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        		\
    memcpy(pMsg+1, &VALUE, sizeof(PARAM));                           \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                   \


// 构建一个带两个参数的消息现场
#define  BUILD_MSG_CONTEXT_EX_2(FUNC,PARAM1,VALUE1,PARAM2,VALUE2)  	      \
    int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(PARAM1)+sizeof(PARAM2);  \
    SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		      \
    pMsg->context_len = sizeof(PARAM1)+sizeof(PARAM2);				      \
    auto temp_adrr = &FUNC;                                               \
    pMsg->function_id = *(DWORD_PTR*)&temp_adrr;	        			  \
    memcpy(pMsg+1, &VALUE1, sizeof(PARAM1));                                \
    memcpy((PARAM1*)(pMsg+1) + 1, &VALUE2, sizeof(PARAM2));                 \
    BUILD_DEBUG_INFO(FUNC, pMsg);                                          \


// 判断函数名称，临时占用enqueue_tick保存下变量...待修改
#define TEST_SAME_FUNCTION(name)	true){auto temp_adrr = &name;pMsg->enqueue_tick=*(DWORD_PTR*)&temp_adrr;} if ( pMsg->enqueue_tick==pMsg->function_id 


template<typename _Ty>
struct return_type_traits;

template<>
struct return_type_traits<char*>
{
    typedef char*  value_type;
};
template<>
struct return_type_traits<DWORD_PTR>
{
    typedef DWORD_PTR  value_type;
};

template<typename _Ty>
struct MsgFunctionName  
{
#ifdef VERSEION_DEBUG
    typedef typename return_type_traits<char*>::value_type value_type;
#else
    typedef typename return_type_traits<DWORD_PTR>::value_type value_type;
#endif

    value_type operator()(_Ty* pMsg)
    {
#ifdef VERSEION_DEBUG
        return pMsg->szName;
#else
        return pMsg->function_id;
#endif
    }
} ;





// 定义一套特殊的宏，用于包装有Type的
#define BUILD_LOGIC_TYPE_MSG
#define BUILD_TYPE_MSG_CONTEXT_VOID(FUNC)                                                                       BUILD_MSG_CONTEXT_1(FUNC,BYTE,m_nServiceType)
#define BUILD_TYPE_MSG_CONTEXT_1(FUNC,PARAM1,VALUE1)                                                            BUILD_MSG_CONTEXT_2(FUNC,BYTE,m_nServiceType,PARAM1,VALUE1)
#define BUILD_TYPE_MSG_CONTEXT_2(FUNC,PARAM1,VALUE1,PARAM2,VALUE2)                                              BUILD_MSG_CONTEXT_3(FUNC,BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2)
#define BUILD_TYPE_MSG_CONTEXT_3(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3)                                BUILD_MSG_CONTEXT_4(FUNC,BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3)
#define BUILD_TYPE_MSG_CONTEXT_4(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4)                  BUILD_MSG_CONTEXT_5(FUNC,BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4)
#define BUILD_TYPE_MSG_CONTEXT_5(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5)    BUILD_MSG_CONTEXT_6(FUNC,BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5)
#define BUILD_TYPE_MSG_CONTEXT_6(FUNC,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6)    BUILD_MSG_CONTEXT_7(FUNC,BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6)


#define GET_TYPE_MSG_PARAM_1(PARAM1,VALUE1)                                                                     GET_MSG_PARAM_2(BYTE,m_nServiceType,PARAM1,VALUE1)
#define GET_TYPE_MSG_PARAM_2(PARAM1,VALUE1,PARAM2,VALUE2)                                                       GET_MSG_PARAM_3(BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2)
#define GET_TYPE_MSG_PARAM_3(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3)                                         GET_MSG_PARAM_4(BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3)
#define GET_TYPE_MSG_PARAM_4(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4)                           GET_MSG_PARAM_5(BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4)
#define GET_TYPE_MSG_PARAM_5(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5)             GET_MSG_PARAM_6(BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5)
#define GET_TYPE_MSG_PARAM_6(PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6)             GET_MSG_PARAM_7(BYTE,m_nServiceType,PARAM1,VALUE1,PARAM2,VALUE2,PARAM3,VALUE3,PARAM4,VALUE4,PARAM5,VALUE5,PARAM6,VALUE6)


#endif	// __COMMON_STUB_MACRO_DEF_H__