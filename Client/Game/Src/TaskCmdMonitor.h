/*******************************************************************
** 文件名: TaskCmdMonitor.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/2/2017
** 版  本: 1.0
** 描  述: 命令监控

********************************************************************/
#ifndef __TASK_CMD_MONITOR_H__
#define __TASK_CMD_MONITOR_H__
#pragma once

#include <strstream>


template<int cmd_off, int array_size>
class CTaskCmdMonitor
{

public:
    CTaskCmdMonitor()
    {
        memset(arr, 0, sizeof(arr));
    }

    void reset()
    {
        memset(arr, 0, sizeof(arr));
    }

    void count(int nCmd)
    {
        ++arr[nCmd-cmd_off];
    }

    // 打印
    void print(int threshold = 10)
    {
        ostrstream  oss;
        for(int i=0; i<array_size; ++i)
        {
            if( arr[i] >= threshold )
            {
                oss << (i+cmd_off) <<"=" << arr[i] << endl;
            }
        }

        char* strText = oss.str();
        if(strText)
        {
            WarningLn(strText);
        }
    }

private:
    int     arr[array_size];
};


#endif // __TASK_CMD_MONITOR_H__