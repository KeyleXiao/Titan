#pragma once



namespace Redis
{
	struct IBaseListOp
	{
		// 添加（左单个）
		//************************************
		// Returns:   bool	是否成功
		// Qualifier:
		// Parameter: const string & strKey
		// Parameter: const string & strValue
		// Parameter: long * pnVal	本操作完成后，List中元素的个数（只有本方法返回true时，才有意义）
		//************************************
		virtual bool	lPush(const string& strKey, const string& strValue, long *pnVal = nullptr) = 0;

		//************************************
		// Returns:   bool	是否成功
		// Qualifier:
		// Parameter: const string & strKey
		// Parameter: const T & strValue
		// Parameter: long * pnVal	本操作完成后，List中元素的个数（只有本方法返回true时，才有意义）
		//************************************
		template<typename T>
		bool	lPush(const string& strKey, const T& strValue, long *pnVal = nullptr)
		{
			return lPush(strKey, TToString(strValue), pnVal);
		}

		// 添加（右单个）
		virtual bool	rPush(const string& strKey, const string& strValue, long *pnVal = nullptr) = 0;

		//************************************
		// Returns:   bool	是否成功
		// Qualifier:
		// Parameter: const string & strKey
		// Parameter: const T & strValue
		// Parameter: long * pnVal	本操作完成后，List中元素的个数（只有本方法返回true时，才有意义）
		//************************************
		template<typename T>
		bool	rPush(const string& strKey, const T& strValue, long *pnVal = nullptr)
		{
			return rPush(strKey, TToString(strValue), pnVal);
		}

		//************************************
		// Returns:   bool	是否成功
		// Qualifier: 把 tValue 插入存于 strKey 的列表中在基准值 pivot 的前面
		//      注意: 必须保证strKey对应的是List
		// Parameter: const string & strKey
		// Parameter: const string & pivot
		// Parameter: const string & tValue
		//************************************
		bool	lInsertBefore(const string& strKey, const string& pivot, const string& tValue)
		{
			return lInsert(strKey, "before", pivot, tValue);
		}
		//************************************
		// Returns:   bool	是否成功
		// Qualifier: 把 tValue 插入存于 strKey 的列表中在基准值 pivot 的后面
		//      注意: 必须保证strKey对应的是List
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
		// Qualifier: 从存于 strKey 的列表里移除前 count 次出现的值为 strValue 的元素。 这个 count 参数通过下面几种方式影响这个操作：
		//				count > 0: 从头往尾移除值为 strValue 的元素。
		//				count < 0 : 从尾往头移除值为 strValue 的元素。
		//				count = 0 : 移除所有值为 strValue 的元素。
		// Parameter: const string & strKey
		// Parameter: long nCount
		// Parameter: const T & strValue
		// Parameter: long * pnVal		被移除的元素个数
		//************************************
		virtual bool	lRem(const string& strKey, long nCount, const string& strValue, long *pnVal = nullptr) = 0;

		//************************************
		// Returns:   bool
		// Qualifier: 修剪(trim)一个已存在的 list，这样 list 就会只包含指定范围的指定元素。
		//				start 和 stop 都是由0开始计数的， 这里的 0 是列表里的第一个元素（表头），1 是第二个元素，以此类推。
		//				start 和 end 也可以用负数来表示与表尾的偏移量，比如 -1 表示列表里的最后一个元素， -2 表示倒数第二个，等等。
		// Parameter: const string & strKey
		// Parameter: long nStart
		// Parameter: long nStop
		//************************************
		virtual bool	lTrim(const string& strKey, long nStart, long nStop) = 0;

		// 更改
		virtual bool	lSet(const string& strKey, long nIndex, const string& strValue) = 0;

		template<typename T>
		bool	lSet(const string& strKey, long nIndex, const T& tValue)
		{
			return lSet(strKey, nIndex, TToString(tValue));
		}

		// 获取（index元素）
		virtual bool	lIndex(const string& strKey, long nIndex, string* pstrValue) = 0;

		//************************************
		// Returns:   bool	是否成功
		// Qualifier: 取strKey对应的list的长度
		// Parameter: const string & strKey
		// Parameter: long * pnValue	list的长度
		//************************************
		virtual bool	lLen(const string& strKey, long* pnValue) = 0;

	protected:
		//// 添加（左批量）
		//virtual bool	lPushVec(const string& strKey, const vector<string>& vecValue) = 0;

		//// 添加（右批量）
		//virtual bool	rPushVec(const string& strKey, const vector<string>& vecValue) = 0;

		// 添加（插入，strPos:"before","after"）
		virtual bool	lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue) = 0;

		// 删除（左弹出）
		virtual bool	lPopString(const string& strKey, string* pstrValue) = 0;

		// 删除（右弹出）
		virtual bool	rPopString(const string& strKey, string* pstrValue) = 0;

		// 获取（区间元素）
		virtual bool	lRangeString(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) = 0;
	};
};