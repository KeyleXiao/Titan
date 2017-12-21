#pragma once
#include "hashMap.h"

namespace spin_hashmap
{	

template <typename Key, typename T, typename Compare, typename Hash>
class hashmap;

//
// HashIterator
template <typename Key, typename T, typename Compare = std::equal_to<Key>,
	typename Hash = DefaultHash<Key> >
class HashIterator
{
public:
	HashIterator(node<const Key, T>* pNode, hashmap<Key, T, Compare, Hash>* inHashmap, const Compare& comp = Compare(),
		const Hash& hash = Hash());
	~HashIterator();

	HashIterator<Key, T, Compare, Hash>& operator++();
	const HashIterator<Key, T, Compare, Hash> operator++(int);

	HashIterator<Key, T, Compare, Hash>& operator--();
	const HashIterator<Key, T, Compare, Hash> operator--(int);

	void operator=(const HashIterator& rhs);
	bool operator==(const HashIterator& rhs) const;
	bool operator!=(const HashIterator& rhs) const;
	
	node<const Key, T>*	operator->() const;

private:
	Hash				m_Hash;
	node<const Key, T>* m_node;
	hashmap<Key, T, Compare, Hash>* m_Hashmap;

	// Helper methods for operator++ and operator--
	void increment();
	void decrement();
	void change_node(node<const Key, T>* node);
};

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>::HashIterator( node<const Key, T>* pNode,
	hashmap<Key, T, Compare, Hash>* inHashmap, const Compare& comp, const Hash& hash)
	:m_Hashmap(inHashmap), 	m_node(pNode), m_Hash(hash)
{
	m_node->AddUsingValue(1);
}

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>::~HashIterator()
{
	m_node->AddUsingValue(-1);
	if (m_node->GetDelFlag() && m_node->GetUsingValue() <= 0){
		delete m_node;
	}
}

template <typename Key, typename T, typename Compare, typename Hash>
void HashIterator<Key, T, Compare, Hash>::increment()
{	
	elemBucket<const Key, T>* pElem = m_Hashmap->m_tElemsT.hashNodes(m_node->GetHashKey());
	node<const Key, T>* pNextNode = m_node->GetNext();
	if (m_node->GetDelFlag() || pNextNode == NULL)
	{
		m_Hashmap->m_tElemsT.setResizeAble(false);	// 暂停hash map的rehash功能
		HashIterator<Key, T, Compare, Hash> iterFind = m_Hashmap->find_upper(m_node->GetKey(), pElem);
		change_node(iterFind.m_node);
		if (pElem != NULL )
		{
			pElem->FreeHead();	
		}
		m_Hashmap->m_tElemsT.setResizeAble(true);	// 恢复hash map的rehash功能
	}
	else
	{
		change_node(pNextNode);		
		pElem->FreeHead();		
	}	
}

template <typename Key, typename T, typename Compare, typename Hash>
node<const Key, T>*	HashIterator<Key, T, Compare, Hash>::operator->() const
{
	return (&(*m_node));
}

template <typename Key, typename T, typename Compare, typename Hash>
void HashIterator<Key, T, Compare, Hash>::operator=(const HashIterator& rhs)
{	
	if (m_Hashmap != rhs.m_Hashmap)
	{
		m_Hashmap = rhs.m_Hashmap;	
	}	
	change_node(rhs.m_node);	
}

template <typename Key, typename T, typename Compare, typename Hash>
void HashIterator<Key, T, Compare, Hash>::change_node(node<const Key, T>* node)
{
	if (node == NULL || node == m_node)
	{
		return;
	}
	m_node->AddUsingValue(-1);
	if (m_node->GetDelFlag() && m_node->GetUsingValue() <= 0){
		delete m_node;
	}
	m_node = node;
	m_node->AddUsingValue(1);
}

template <typename Key, typename T, typename Compare, typename Hash>
bool HashIterator<Key, T, Compare, Hash>::operator==(const HashIterator& rhs) const
{
	return (m_node == rhs.m_node && m_Hashmap == rhs.m_Hashmap);
}

template <typename Key, typename T, typename Compare, typename Hash>
bool HashIterator<Key, T, Compare, Hash>::operator!=(const HashIterator& rhs) const
{
	return (m_node != rhs.m_node || m_Hashmap != rhs.m_Hashmap);
}

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>& 
	HashIterator<Key, T, Compare, Hash>::operator++()
{
	increment();
	return (*this);
}

template <typename Key, typename T, typename Compare, typename Hash>
const HashIterator<Key, T, Compare, Hash> 
	HashIterator<Key, T, Compare, Hash>::operator++(int)
{
	HashIterator<Key, T, Compare, Hash> oldIt = *this;
	increment();
	return (oldIt);
}

} // namespace spin_hashmap