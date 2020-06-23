#pragma once

// https://github.com/eligantRU/oop/tree/dev/lw7

#include <memory>
#include <assert.h>

template <class T>
class CMyList
{
	template <class T>
	struct SNode
	{
		template <class T>
		SNode(T && data, SNode * prev, std::unique_ptr<SNode> && next)
			:data(std::forward<T>(data))
			,prev(prev)
			,next(std::move(next))
		{

		}

		T data;
		SNode * prev;
		std::unique_ptr<SNode> next;
	};

public:
	CMyList() = default;

	~CMyList()
	{
		clear();
	}
	
	CMyList(const CMyList & list)
	{
		CMyList<T> tmp;
		tmp.insert(tmp.begin(), list.begin(), list.end());
		swap(tmp);
	}

	CMyList(CMyList && list)
	{
		m_size = list.m_size;
		m_firstNode = std::move(list.m_firstNode);
		m_lastNode = list.m_lastNode;
		list.m_lastNode = nullptr;
		list.m_size = 0;
	}

	CMyList(const std::initializer_list<T> & il)
	{
		for (const auto & element : il)
		{
			push_back(element);
		}
	}

	explicit CMyList(const size_t n)
		:CMyList(n, T())
	{

	}

	CMyList(const size_t n, const T & value)
	{
		insert(begin(), n, value);
	}

	CMyList & operator=(const CMyList & list)
	{
		if (list != *this)
		{
			CMyList<T> tmp;
			tmp.insert(tmp.begin(), list.begin(), list.end());
			swap(tmp);
		}
		return *this;
	}

	CMyList & operator=(CMyList && list)
	{
		if (list != *this)
		{
			clear();
			m_size = list.m_size;
			m_firstNode = std::move(list.m_firstNode);
			m_lastNode = list.m_lastNode;
			list.m_size = 0;
			list.m_lastNode = nullptr;
		}
		return *this;
	}

	CMyList & operator=(const std::initializer_list<T> & il)
	{
		clear();
		for (const auto & element : il)
		{
			push_back(element);
		}
		return *this;
	}

	template <class T>
	class CIterator
	{
		friend CMyList;

		CIterator(SNode<T> * node, bool isReverse = false)
			:m_pNode(node)
			,m_isReverse(isReverse)
		{

		}

	public:
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef ptrdiff_t difference_type;
		typedef T * pointer;
		typedef T & reference;

		CIterator() = delete;

		T & operator*() const
		{
			assert(this->m_pNode);
			return m_pNode->data;
		}

		CIterator<T> & operator--()
		{
			m_pNode = (!m_isReverse) ? (m_pNode->prev) : (m_pNode->next.get());
			return *this;
		}

		CIterator<T> & operator++()
		{
			m_pNode = (!m_isReverse) ? (m_pNode->next.get()) : (m_pNode->prev);
			return *this;
		}

		CIterator<T> operator--(int)
		{
			auto copy = *this;
			--*this;
			return copy;
		}

		CIterator<T> operator++(int)
		{
			auto copy = *this;
			++*this;
			return copy;
		}

		bool operator==(const CIterator & it) const
		{
			return (m_pNode == it.m_pNode);
		}

		bool operator!=(const CIterator & it) const
		{
			return (m_pNode != it.m_pNode);
		}

	private:
		SNode<T> * operator->() const
		{
			return m_pNode;
		}

		SNode<T> * m_pNode = nullptr;
		bool m_isReverse;
	};

	void clear()
	{
		erase(begin(), end());
	}

	size_t size() const
	{
		return m_size;
	}

	size_t max_size() const
	{
		return std::numeric_limits<size_t>::max();
	}

	bool empty() const
	{
		return (size() == 0);
	}

	void append(const T & data)
	{
		auto newNode = std::make_unique<SNode<T>>(data, m_lastNode, nullptr);
		auto * newLastNode = newNode.get();
		if (m_lastNode)
		{
			m_lastNode->next = std::move(newNode);
		}
		else
		{
			m_firstNode = std::move(newNode);
		}
		m_lastNode = newLastNode;
		++m_size;
	}

	void append(T && data)
	{
		auto newNode = std::make_unique<SNode<T>>(std::move(data), m_lastNode, nullptr);
		auto * newLastNode = newNode.get();
		if (m_lastNode)
		{
			m_lastNode->next = std::move(newNode);
		}
		else
		{
			m_firstNode = std::move(newNode);
		}
		m_lastNode = newLastNode;
		++m_size;
	}

	void push_back(const T & data)
	{
		append(data);
	}

	void push_front(const T & data)
	{
		if (empty())
		{
			append(data);
		}
		else
		{
			auto secondNode = std::move(m_firstNode);
			m_firstNode = std::make_unique<SNode<T>>(data, nullptr, std::move(secondNode));
			++m_size;
		}
	}

	void push_back(T && data)
	{
		append(std::move(data));
	}

	void push_front(T && data)
	{
		if (empty())
		{
			append(std::move(data));
		}
		else
		{
			auto secondNode = std::move(m_firstNode);
			m_firstNode = std::make_unique<SNode<T>>(std::move(data), nullptr, std::move(secondNode));
			++m_size;
		}
	}

	void insert(const CIterator<T> & it, const T & data)
	{
		if (it == begin())
		{
			push_front(data);
		}
		else if (it == end())
		{
			push_back(data);
		}
		else
		{
			auto newNode = std::make_unique<SNode<T>>(data, it->prev, std::move(it->prev->next));
			it->prev = std::move(newNode.get());
			newNode->prev->next = std::move(newNode);
			++m_size;
		}
	}

	void insert(const CIterator<T> & it, T && data)
	{
		if (it == begin())
		{
			push_front(std::move(data));
		}
		else if (it == end())
		{
			push_back(std::move(data));
		}
		else
		{
			auto newNode = std::make_unique<SNode<T>>(std::move(data), it->prev, std::move(it->prev->next));
			it->prev = std::move(newNode.get());
			newNode->prev->next = std::move(newNode);
			++m_size;
		}
	}

	void insert(const CIterator<T> & it, const size_t n, const T & data)
	{
		for (size_t i = 0; i < n; ++i)
		{
			insert(it, data);
		}
	}

	void insert(const CIterator<T> & it, const std::initializer_list<T> & il)
	{
		for (const auto & el : il)
		{
			insert(it, el);
		}
	}

	void insert(const CIterator<T> & insIt, const CIterator<T> & first, const CIterator<T> & last)
	{
		for (auto it = first; it != last; ++it)
		{
			insert(insIt, *it);
		}
	}

	void erase(const CIterator<T> & it)
	{
		assert(size() > 0);
		if (size() == 1)
		{
			m_firstNode = nullptr;
			m_lastNode = nullptr;
			m_size = 0;
			return;
		}
		else if (it == begin())
		{
			m_firstNode = std::move(m_firstNode->next);
			m_firstNode->prev = nullptr;
		}
		else if (it.m_pNode == m_lastNode)
		{
			m_lastNode = it->prev;
			m_lastNode->next = nullptr;
		}
		else
		{
			it->next->prev = it->prev;
			it->prev->next = std::move(it->next);
		}
		--m_size;
	}

	void erase(const CIterator<T> & first, const CIterator<T> & last)
	{
		auto it = first;
		while (it != last)
		{
			assert(size() > 0);
			erase(it++);
		}
	}

	void pop_front()
	{
		erase(begin());
	}

	void pop_back()
	{
		erase(rbegin());
	}

	CIterator<T> begin()
	{
		return CIterator<T>(m_firstNode.get());
	}

	CIterator<T> end()
	{
		if (empty())
		{
			return begin();
		}
		return CIterator<T>(m_lastNode->next.get());
	}

	const CIterator<T> begin() const
	{
		return CIterator<T>(m_firstNode.get());
	}

	const CIterator<T> end() const
	{
		if (empty())
		{
			return begin();
		}
		return CIterator<T>(m_lastNode->next.get());
	}

	const CIterator<T> cbegin() const
	{
		return CIterator<T>(m_firstNode.get());
	}

	const CIterator<T> cend() const
	{
		if (empty())
		{
			return cbegin();
		}
		return CIterator<T>(m_lastNode->next.get());
	}

	CIterator<T> rbegin()
	{
		if (empty())
		{
			return CIterator<T>(m_firstNode.get(), true);
		}
		return CIterator<T>(m_lastNode, true);
	}

	CIterator<T> rend()
	{
		if (empty())
		{
			return rbegin();
		}
		return CIterator<T>(m_firstNode->prev, true);
	}

	const CIterator<T> rbegin() const
	{
		if (empty())
		{
			return CIterator<T>(m_firstNode.get(), true);
		}
		return CIterator<T>(m_lastNode, true);
	}

	const CIterator<T> rend() const
	{
		if (empty())
		{
			return rbegin();
		}
		return CIterator<T>(m_firstNode->prev, true);
	}

	const CIterator<T> crbegin() const
	{
		if (empty())
		{
			return CIterator<T>(m_firstNode.get(), true);
		}
		return CIterator<T>(m_lastNode, true);
	}

	const CIterator<T> crend() const
	{
		if (empty())
		{
			return crbegin();
		}
		return CIterator<T>(m_firstNode->prev, true);
	}

	T & front()
	{
		assert(m_firstNode);
		return m_firstNode->data;
	}

	const T & front() const
	{
		assert(m_firstNode);
		return m_firstNode->data;
	}

	T & back()
	{
		assert(m_lastNode);
		return m_lastNode->data;
	}

	const T & back() const
	{
		assert(m_lastNode);
		return m_lastNode->data;
	}

	void remove(const T & value)
	{
		for (auto it = begin(); it != end();)
		{
			if (*it != value)
			{
				++it;
			}
			else
			{
				erase(it++);
			}
		}
	}

	void resize(const size_t n)
	{
		if (n >= size())
		{
			insert(end(), n - size(), T());
		}
		else
		{
			while (size() != n)
			{
				assert(size() > 0);
				pop_back();
			}
		}
	}

	void resize(const size_t n, const T & value)
	{
		if (n >= size())
		{
			insert(end(), n - size(), value);
		}
		else
		{
			while (size() != n)
			{
				assert(size() > 0);
				pop_back();
			}
		}
	}

	void swap(CMyList & list)
	{
		std::swap(list, *this);
	}

private:
	size_t m_size = 0;
	std::unique_ptr<SNode<T>> m_firstNode;
	SNode<T> * m_lastNode = nullptr;
};

template <class T>
bool operator==(const CMyList<T> & lhs, const CMyList<T> & rhs)
{
	if (&lhs == &rhs)
	{
		return true;
	}
	if (lhs.size() == rhs.size())
	{
		auto itr = rhs.begin();
		for (auto itl = lhs.begin(); itl != lhs.end(); ++itl, ++itr)
		{
			if (*itl != *itr)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

template <class T>
bool operator!=(const CMyList<T> & lhs, const CMyList<T> & rhs)
{
	return !(lhs == rhs);
}
