#pragma once



namespace Redis
{
	struct IBaseListOp
	{
		// ��ӣ��󵥸���
		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier:
		// Parameter: const string & strKey
		// Parameter: const string & strValue
		// Parameter: long * pnVal	��������ɺ�List��Ԫ�صĸ�����ֻ�б���������trueʱ���������壩
		//************************************
		virtual bool	lPush(const string& strKey, const string& strValue, long *pnVal = nullptr) = 0;

		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier:
		// Parameter: const string & strKey
		// Parameter: const T & strValue
		// Parameter: long * pnVal	��������ɺ�List��Ԫ�صĸ�����ֻ�б���������trueʱ���������壩
		//************************************
		template<typename T>
		bool	lPush(const string& strKey, const T& strValue, long *pnVal = nullptr)
		{
			return lPush(strKey, TToString(strValue), pnVal);
		}

		// ��ӣ��ҵ�����
		virtual bool	rPush(const string& strKey, const string& strValue, long *pnVal = nullptr) = 0;

		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier:
		// Parameter: const string & strKey
		// Parameter: const T & strValue
		// Parameter: long * pnVal	��������ɺ�List��Ԫ�صĸ�����ֻ�б���������trueʱ���������壩
		//************************************
		template<typename T>
		bool	rPush(const string& strKey, const T& strValue, long *pnVal = nullptr)
		{
			return rPush(strKey, TToString(strValue), pnVal);
		}

		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier: �� tValue ������� strKey ���б����ڻ�׼ֵ pivot ��ǰ��
		//      ע��: ���뱣֤strKey��Ӧ����List
		// Parameter: const string & strKey
		// Parameter: const string & pivot
		// Parameter: const string & tValue
		//************************************
		bool	lInsertBefore(const string& strKey, const string& pivot, const string& tValue)
		{
			return lInsert(strKey, "before", pivot, tValue);
		}
		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier: �� tValue ������� strKey ���б����ڻ�׼ֵ pivot �ĺ���
		//      ע��: ���뱣֤strKey��Ӧ����List
		// Parameter: const string & strKey
		// Parameter: const string & pivot
		// Parameter: const string & tValue
		//************************************
		bool	lInsertAfter(const string& strKey, const string& pivot, const string& tValue)
		{
			return lInsert(strKey, "after", pivot, tValue);
		}

		//************************************
		// Returns:   bool
		// Qualifier: �Ӵ��� strKey ���б����Ƴ�ǰ count �γ��ֵ�ֵΪ strValue ��Ԫ�ء� ��� count ����ͨ�����漸�ַ�ʽӰ�����������
		//				count > 0: ��ͷ��β�Ƴ�ֵΪ strValue ��Ԫ�ء�
		//				count < 0 : ��β��ͷ�Ƴ�ֵΪ strValue ��Ԫ�ء�
		//				count = 0 : �Ƴ�����ֵΪ strValue ��Ԫ�ء�
		// Parameter: const string & strKey
		// Parameter: long nCount
		// Parameter: const T & strValue
		// Parameter: long * pnVal		���Ƴ���Ԫ�ظ���
		//************************************
		virtual bool	lRem(const string& strKey, long nCount, const string& strValue, long *pnVal = nullptr) = 0;

		//************************************
		// Returns:   bool
		// Qualifier: �޼�(trim)һ���Ѵ��ڵ� list������ list �ͻ�ֻ����ָ����Χ��ָ��Ԫ�ء�
		//				start �� stop ������0��ʼ�����ģ� ����� 0 ���б���ĵ�һ��Ԫ�أ���ͷ����1 �ǵڶ���Ԫ�أ��Դ����ơ�
		//				start �� end Ҳ�����ø�������ʾ���β��ƫ���������� -1 ��ʾ�б�������һ��Ԫ�أ� -2 ��ʾ�����ڶ������ȵȡ�
		// Parameter: const string & strKey
		// Parameter: long nStart
		// Parameter: long nStop
		//************************************
		virtual bool	lTrim(const string& strKey, long nStart, long nStop) = 0;

		// ����
		virtual bool	lSet(const string& strKey, long nIndex, const string& strValue) = 0;

		template<typename T>
		bool	lSet(const string& strKey, long nIndex, const T& tValue)
		{
			return lSet(strKey, nIndex, TToString(tValue));
		}

		// ��ȡ��indexԪ�أ�
		virtual bool	lIndex(const string& strKey, long nIndex, string* pstrValue) = 0;

		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier: ȡstrKey��Ӧ��list�ĳ���
		// Parameter: const string & strKey
		// Parameter: long * pnValue	list�ĳ���
		//************************************
		virtual bool	lLen(const string& strKey, long* pnValue) = 0;

	protected:
		//// ��ӣ���������
		//virtual bool	lPushVec(const string& strKey, const vector<string>& vecValue) = 0;

		//// ��ӣ���������
		//virtual bool	rPushVec(const string& strKey, const vector<string>& vecValue) = 0;

		// ��ӣ����룬strPos:"before","after"��
		virtual bool	lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue) = 0;

		// ɾ�����󵯳���
		virtual bool	lPopString(const string& strKey, string* pstrValue) = 0;

		// ɾ�����ҵ�����
		virtual bool	rPopString(const string& strKey, string* pstrValue) = 0;

		// ��ȡ������Ԫ�أ�
		virtual bool	lRangeString(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) = 0;
	};
};