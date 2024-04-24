#ifndef CQUEUE_H
#define CQUEUE_H

#include <iostream>
#include <mutex>
#include <queue>

template <typename T>
class ConcurrentQueue {
private:
    std::queue<T> queue;
    std::mutex m_mutex;

public:
    T pop() {
        std::unique_lock<std::mutex> _lock;
        auto front = queue.front();
        queue.pop();
        return front;
    }

    void push(const T& value) {
        std::unique_lock<std::mutex> _lock;
        queue.push(value);
    }

    T& front() {
        std::unique_lock<std::mutex> _lock;
        auto front = queue.front();
        return front;
    }

    bool empty() {
        std::unique_lock<std::mutex> _lock;
        auto empty = queue.empty();
        return empty;
    }
};

#endif // CQUEUE_H
