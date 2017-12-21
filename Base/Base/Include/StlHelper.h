#ifndef __stlhelper_h__
#define __stlhelper_h__

#define STL_SAFE_DELETE(x)	{if(x){delete (x);(x)=0;}}
#define STL_SAFE_RELEASE(x)	{if(x){(x)->release();(x)=0;}}

template<class T>void STLDeleteAssociate(T &t)
{
	typename T::iterator i = t.begin();
	for(; i != t.end(); ++i)
	{
		STL_SAFE_DELETE((*i).second);
	}
	t.clear();
}

template<class T>void STLDeleteSequence(T &t)
{
	typename T::iterator i = t.begin();
	for(; i != t.end(); ++i)
	{
		STL_SAFE_DELETE(*i);
	}
	t.clear();
}

template<class T>void STLReleaseAssociate(T &t)
{
	typename T::iterator i = t.begin();
	for(; i != t.end(); ++i)
	{
		STL_SAFE_RELEASE((*i).second);
	}
	t.clear();
}

template<class T>void STLReleaseSequence(T &t)
{
	typename T::iterator i = t.begin();
	for(; i != t.end(); ++i)
	{
		(*i)->release();
	}
	t.clear();
}

template<class T,class TS>void STLDeleteAssociateSequence(T& t)
{
	typename T::iterator i = t.begin();
	for(; i != t.end(); ++i)
	{
		TS *pTS = (*i).second;
		if(pTS)
		{
			STLDeleteSequence(*pTS);
		}
		STL_SAFE_DELETE((*i).second);
	}
	t.clear();
}

#endif