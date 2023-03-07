#pragma once
#include <memory>
#include <queue>
#include <mutex>

template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;

public:
    size_t size() const
    {
        std::lock_guard<std::mutex> lockQ(mutex_);
        return queue_.size();
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lockQ(mutex_);
        return queue_.empty();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lockQ(mutex_);
        cond_.wait(lockQ, [this]()
                   { return !queue_.empty(); });

        T topElement = std::move(queue_.front());
        queue_.pop();
        return topElement;
    }
    void push(const T &newElement)
    {
        std::unique_lock<std::mutex> lockQ(mutex_);
        queue_.push(newElement);
        cond_.notify_one();
    }
    void push(T &&newElement)
    {
        std::unique_lock<std::mutex> lockQ(mutex_);
        queue_.push(std::move(newElement));
        cond_.notify_one();
    }
};
