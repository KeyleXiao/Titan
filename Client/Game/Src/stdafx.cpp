// stdafx.cpp : 只包括标准包含文件的源文件
// Game.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


const std::string& getBasePath()
{
    static std::string sBaseDir;

    if (sBaseDir.empty())
    {
        sBaseDir = rkt::getWorkDir();
        std::string workdir = sBaseDir;
        MStringHelper::toLower(workdir);
        size_t pos = workdir.rfind("\\bin");
        if (pos != std::string::npos)
            sBaseDir.erase(pos, sBaseDir.length());
        else
        {
            pos = workdir.rfind("\\Bin");
            if (pos != std::string::npos)
                sBaseDir.erase(pos, sBaseDir.length());
        }

        // 可以配置客户端的BasePath，比如指向外网客户端的 远征Online 目录，这样对调试方便，可以不从svn下载资源
        Ini ini("config.ini");
        sBaseDir = ini.getString("App", "BasePath", sBaseDir.c_str());
    }

    return sBaseDir;
}