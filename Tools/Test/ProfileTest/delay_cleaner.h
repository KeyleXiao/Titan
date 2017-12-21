// ===========================================================================
//	filename: 	delay_cleaner.h
//
//	author:		CHENTAO
//
//	created:	2016/09/19
//	
//	purpose:	�ӳ�������
//
// ===========================================================================


#ifndef  __delay_cleaner_h__
#define  __delay_cleaner_h__

#include "singleton.h"
#include <list>
using namespace std;


	// �ӳ�deleteһЩָ��
	template<
		typename T,
		int __DELAY_CLEAN_TIME = 10000  // �ӳ�����ʱ��
	>
	class delay_cleaner : public Singleton<delay_cleaner<T,__DELAY_CLEAN_TIME>>
	{
	private:
		typedef std::list<T *>     GARB_LIST;

		struct clean_list
		{
			GARB_LIST  * garbagebin_front;      // ǰ�˻���Ͱ,��ǰ��Ͱ���붫��
			GARB_LIST  * garbagebin_back;       // ��˻���Ͱ,���Ͱ��Ķ��������ӳٺ��������ͬʱ��תͰ
			DWORD   swap_tick;                  // �ϴη�תʱ��
		};

		DWORD       tls_index;			// �߳�ר�д洢���� 

		inline clean_list * get_clean_list()
		{
			clean_list * plist = (clean_list *)TlsGetValue(tls_index);
			if ( plist==0 )
			{
				plist = new clean_list;

                plist->garbagebin_front = new GARB_LIST();
                plist->garbagebin_back  = new GARB_LIST();
				plist->swap_tick = getTickCount();

				TlsSetValue(tls_index,plist);
			}

			return plist;
		}

		void swap(clean_list * plist)
		{
			plist->swap_tick = getTickCount();
			GARB_LIST * temp_list   = plist->garbagebin_front;
			plist->garbagebin_front = plist->garbagebin_back;
			plist->garbagebin_back  = temp_list;
		}

	public:
		delay_cleaner()
		{
			tls_index = ::TlsAlloc();
		}

		virtual ~delay_cleaner()
		{
			::TlsFree(tls_index);
			tls_index  =0;
		}

		void destroy()
		{
			clean_list * plist = (clean_list *)TlsGetValue(tls_index);
			if ( plist )
			{
                if(plist->garbagebin_back)
                {
                    GARB_LIST::iterator it = plist->garbagebin_back->begin();
                    for ( ;it!=plist->garbagebin_back->end();++it )
                    {
                        delete *it;
                    }

                    delete plist->garbagebin_back; 
                    plist->garbagebin_back = 0;
                }


                if(plist->garbagebin_front)
                {
				    it = plist->garbagebin_front->begin();
				    for ( ;it!=plist->garbagebin_front->end();++it )
				    {
					    delete *it;
				    }

                    delete plist->garbagebin_front; 
                    plist->garbagebin_front = 0;
                }

				delete plist;
				TlsSetValue(tls_index,0);
			}
		}

		void clean( T * ptr )
		{
			clean_list * plist = get_clean_list();
			
			// ��ʱ����
			if ( getTickCount() - plist->swap_tick > __DELAY_CLEAN_TIME )
			{
				GARB_LIST::iterator it = plist->garbagebin_back->begin();
				for ( ;it!=plist->garbagebin_back->end();++it )
				{
					delete *it;
				}
				plist->garbagebin_back->clear();
				swap(plist);
			}

			plist->garbagebin_front->push_back(ptr);
		}
	};

#endif //__delay_cleaner_h__
