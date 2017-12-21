using System;
using System.Reflection;
using System.IO;
using System.Text;

struct ITimerHandler
{
};

interface TimerService
{
		/**
		@purpose          : ����һ����ʱ��
		@param	 timerID  : ��ʱ��ID
		@param   interval : ��ʱ�����ü��
		@param   handler  : ����ӿ�
		@param   callTimes: ���ô���,Ĭ�ϵ��������
		@param   debugInfo: ������Ϣ
		@return		      : ������óɹ��򷵻�true
		*/
		unsafe bool SetTimer( UInt32 timerID,UInt32  interval,ITimerHandler * handler,UInt32 callTimes,string debugInfo );

		/**
		@purpose          : ɾ����ʱ��
		@param	 timerID  : ��ʱ��ID
		@param   handler  : ����ӿ�
		@return		      : �����Ƿ�ɾ���ɹ�
		*/
		unsafe bool KillTimer( UInt32  timerID,ITimerHandler * handler );

		void release();
};