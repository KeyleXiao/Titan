/*******************************************************************
** 文件名:	StackTrace.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	9/4/2017
** 版  本:	1.0
** 描  述:	打印函数调用栈https://fresky.github.io/2015/06/21/how-to-get-the-call-stack-from-code/
********************************************************************/
#pragma comment(lib, "dbghelp.lib")
#include <windows.h>
#include <DbgHelp.h>
#include <iostream>
class StackTrace {
private:
    static const int m_MaxFrameCount = 62;
    int m_FrameCount;
    void* m_Frames[m_MaxFrameCount];
public:
    StackTrace() {
        m_FrameCount = CaptureStackBackTrace(1, m_MaxFrameCount, m_Frames, NULL);
    }
    void Print()
    {
        HANDLE process = GetCurrentProcess();

        SymSetOptions(SYMOPT_LOAD_LINES); // set symbol option to load the source code lines
        SymInitialize(process, NULL, TRUE);  // init symbol

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]; // define the PSYMBOL_INFO
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        IMAGEHLP_LINE64 line; // define the IMAGEHLP_LINE64
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        TraceLn("*************************callstack***************************");
        for (int i = 0; i < m_FrameCount; i++)
        {
            DWORD64  dw64Displacement;
            SymFromAddr(process, (DWORD64)(m_Frames[i]), &dw64Displacement, symbol); // get symbol info
            std::cout << symbol->Name;
            DWORD dwDisplacement;
            if (SymGetLineFromAddr64(process, (DWORD64)(m_Frames[i]), &dwDisplacement, &line)) // get line info
            {
                TraceLn(symbol->Name << "," << line.FileName << ", line " << line.LineNumber);
            }
        }
        TraceLn("*************************************************************");
        SymCleanup(process);
    }
};