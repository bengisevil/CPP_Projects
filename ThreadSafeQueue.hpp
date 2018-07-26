#include <queue>
#include <mutex>
#include <string>
#include <iostream>
#include <condition_variable>
#include <thread>
//Thread Safe Queue Implementation

template<typename T>
class ThreadSafeQueue {
public:
	void push(const T & value);
	bool empty() const;
	bool try_pop(T& popped_value);
	//void wait_and_pop(T& popped_value);
private:
	std::queue<T> the_queue;
	mutable std::mutex the_mutex;
	std::condition_variable the_condition_variable;
};

template<typename T>
inline void ThreadSafeQueue<T>::push(const T & value)
{
	std::unique_lock<std::mutex> lock(the_mutex);
	the_queue.push(value);
	lock.unlock();
	the_condition_variable.notify_one();
}

template<typename T>
inline bool ThreadSafeQueue<T>::empty() const
{
	std::lock_guard<std::mutex> lock(the_mutex);
	return the_queue.empty();
}

template<typename T>
inline bool ThreadSafeQueue<T>::try_pop(T & popped_value)
{
	std::lock_guard<std::mutex> lock(the_mutex);
	if (the_queue.empty()) {
		return false;
	}
	popped_value = the_queue.front();
	the_queue.pop();
	return true;
}
/*
template<typename T>
inline void ThreadSafeQueue<T>::wait_and_pop(T & popped_value)
{
	std::unique_lock<std::mutex> lock(the_mutex);
	while (the_queue.empty()) {
		the_condition_variable.wait(lock);
	}
	popped_value = the_queue.front();
	the_queue.pop();
}
*/