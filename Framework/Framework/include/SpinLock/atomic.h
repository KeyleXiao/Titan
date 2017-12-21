#pragma once
#include "spinLock.h"

// 
template<typename T>
struct atomic_m
{
	atomic_m():spin_mu(0){};
	atomic_m(T t):data(t),spin_mu(0){};
	T GetValueUnsafe()
	{
		return data;	
	}
	T GetValue()
	{
		while(!spin_mu.TryGetLock()){};
		T temp = data;
		spin_mu.UnLock();
		return temp;
	}
	void reset()
	{
		while(!spin_mu.TryGetLock()){};
		data = T();
		spin_mu.UnLock();
	}
	void SetValue(T newVal)
	{
		while(!spin_mu.TryGetLock()){};
		data = newVal;
		spin_mu.UnLock();
	}
	bool casSetValue(T oldVal, T newVal)
	{
		while(!spin_mu.TryGetLock()){};
		bool bSuc = spin_bool_compare_and_swap32(&data, oldVal, newVal);
		spin_mu.UnLock();
		return bSuc;
	}
	void AddValue(T addVa)
	{
		while(!spin_mu.TryGetLock()){};
		data += addVa;
		spin_mu.UnLock();
	}	
private:
	spin_mutex_s spin_mu;
	T	data;
};