#pragma once

// Implementation of queue exploiting contiguous memory for speed
// Assumes the queue will not grow indefinitely, rather will be reset before it gets too big

template <typename queue>
class queueIterator
{
public:
	using ValueType = typename queue::ValueType;
public:
	queueIterator(ValueType* ptr)
		: m_ptr(ptr) {}

	queueIterator& operator++()
	{
		m_ptr++;
		return *this;
	}

	queueIterator& operator++(int)
	{
		queueIterator iterator = *this;
		++(*this);
		return iterator;
	}

	queueIterator& operator--()
	{
		m_ptr--;
		return *this;
	}

	queueIterator& operator--(int)
	{
		queueIterator iterator = *this;
		--(*this);
		return iterator;
	}

	ValueType& operator[](int index)
	{
		return *(m_ptr[index]);
	}

	ValueType* operator->()
	{
		return m_ptr;
	}

	ValueType& operator*()
	{
		return *m_ptr;
	}

	bool operator==(const queueIterator& other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const queueIterator& other) const
	{
		return m_ptr != other.m_ptr;
	}
private:
	ValueType* m_ptr;
};

template<typename T>
struct queue
{
public:
	using ValueType = T;
	using Iterator = queueIterator<queue<T>>;

private:
	std::vector<T> q;
	unsigned int start;

public:
	bool reserve(unsigned int N)
	{
		q.reserve(N);
	}

	void push(T t)
	{
		q.push_back(t);
	}

	void pop()
	{
		start++;
	}

	T front()
	{
		return q[start];
	}

	bool empty()
	{
		return start == q.size();
	}

	void clear()
	{
		q.clear();
		start = 0;
	}

	// Iterator methods:

	Iterator begin()
	{
		return Iterator(&q[0] + start);
	}
	Iterator end()
	{
		return Iterator(&q[0] + q.size());
	}
};

