#pragma once

/**
 * Implementation of a FIFO queue used by SweepAndPrune1DSolver
 * 
 * For best usage, reserve the appropriate amount of space
 * using reserve(...), and reset the queue using clear() once 
 * the queue is empty
 */

template <typename Queue>
class QueueIterator
{
public:
	using ValueType = typename Queue::ValueType;
public:
	QueueIterator(ValueType* ptr)
		: m_ptr(ptr) {}

	QueueIterator& operator++()
	{
		m_ptr++;
		return *this;
	}

	QueueIterator& operator++(int)
	{
		QueueIterator iterator = *this;
		++(*this);
		return iterator;
	}

	QueueIterator& operator--()
	{
		m_ptr--;
		return *this;
	}

	QueueIterator& operator--(int)
	{
		QueueIterator iterator = *this;
		--(*this);
		return iterator;
	}

	ValueType& operator[](std::size_t index)
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

	bool operator==(const QueueIterator& other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const QueueIterator& other) const
	{
		return m_ptr != other.m_ptr;
	}
private:
	ValueType* m_ptr;
};

template<typename T>
struct Queue
{
public:
	using ValueType = T;
	using Iterator = QueueIterator<Queue<T>>;

private:
	std::vector<T> q;
	std::size_t start;

public:
	Queue()
		: start(0) {}

	void reserve(std::size_t N)
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

	// Iterator methods

	Iterator begin()
	{
		return Iterator(q.data() + start);
	}

	Iterator end()
	{
		return Iterator(q.data() + q.size());
	}
};

