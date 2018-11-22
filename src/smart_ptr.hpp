#pragma once

template <typename T>
class smart_ptr
{
public:
	typedef int counter_type;
	typedef T object_type;
public:
	smart_ptr()
		: m_ptr(0)
	{
	}

	~smart_ptr()
	{
		destroy();
	}

	smart_ptr(const smart_ptr& other)
		: m_ptr(other.m_ptr)
	{
		increment();
	}

	smart_ptr(smart_ptr&& other)
		: m_ptr(other.m_ptr)
	{
		other.m_ptr = 0;
	}

	smart_ptr& operator =(const smart_ptr& rhs)
	{
		if (this != &rhs && m_ptr != rhs.m_ptr) {
			destroy();
			m_ptr = rhs.m_ptr;
			increment();
		}
		return *this;
	}

	object_type& operator *()
	{
		return *get();
	}
	
	const object_type& operator *() const
	{
		return *get();
	}
	
	object_type* operator ->()
	{
		return get();
	}
	
	const object_type* operator ->() const
	{
		return get();
	}

	bool operator ==(const smart_ptr& rhs) const
	{
		return m_ptr == rhs.m_ptr;
	}

	bool operator !=(const smart_ptr& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator <(const smart_ptr& rhs) const
	{
		return m_ptr < rhs.m_ptr;
	}

	bool operator <=(const smart_ptr& rhs) const
	{
		return (*this < rhs) || (*this == rhs);
	}

	bool operator >(const smart_ptr& rhs) const
	{
		return !(*this <= rhs);
	}

	bool operator >=(const smart_ptr& rhs) const
	{
		return (*this > rhs) || (*this == rhs);
	}

	operator bool() const
	{
		return 0 != m_ptr;
	}

	counter_type use_count() const
	{
		return 0 != m_ptr ? *address_of_counter(m_ptr) : 0;
	}

	object_type* get()
	{
		return address_of_object(m_ptr);
	}
	
	const object_type* get() const
	{
		return address_of_object(m_ptr);
	}

	bool is_unique() const
	{
		return 1 == use_count();
	}

	void reset()
	{
		destroy();
	}

	void swap(smart_ptr& other)
	{
		std::swap(m_ptr, other.m_ptr);
	}
public:
	template <typename ...Args>
	static smart_ptr create(Args&& ...args)
	{
		smart_ptr sp;
		sp.m_ptr = malloc(sizeof(object_type) + sizeof(counter_type));
		new (address_of_object(sp.m_ptr)) object_type(std::forward<Args&&>(args)...);
		new (address_of_counter(sp.m_ptr)) counter_type(1);
		return sp;
	}
private:
	void increment()
	{
		if (0 != m_ptr) {
			++*address_of_counter(m_ptr);
		}
	}

	void decrement()
	{
		if (0 != m_ptr) {
			--*address_of_counter(m_ptr);
		}
	}

	void destroy()
	{
		if (1 >= use_count()) {
			free(m_ptr);
		} else {
			decrement();
		}
		m_ptr = 0;
	}
private:
	static object_type* address_of_object(void* p)
	{
		return static_cast<object_type*>(p);
	}

	static counter_type* address_of_counter(void* p)
	{
		return reinterpret_cast<counter_type*>(static_cast<char*>(p) + sizeof(object_type));
	}
private:
	void* m_ptr;
};
