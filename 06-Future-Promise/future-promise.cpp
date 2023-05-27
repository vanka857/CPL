#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>


template<typename T>
class Future;

template<typename T>
class Promise : public std::enable_shared_from_this<Promise<T> >
{
    friend class Future<T>;
private:
    bool completed;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::shared_ptr<T> m_value;
    std::shared_ptr<std::exception> m_exception;
    
public:
    explicit Promise()
    {
        completed = false;
    }

    void SetValue(const T& value)
    {
        std::scoped_lock lock(m_mutex);
        if (completed) 
        {
            throw std::runtime_error("Promise already set");
        }
        completed = true;
        m_value = std::make_shared<T>(value);
        m_condition.notify_one();
    }

    template<typename E>
    void SetException(const E& exception)
    {
        std::scoped_lock lock(m_mutex);
        if (completed)
        {
            throw std::runtime_error("Promise already set");
        }
        completed = true;
        m_exception = std::make_shared<std::exception>(exception);
        m_condition.notify_one();
    }

    Future<T> GetFuture()
    {
        return Future<T>(std::enable_shared_from_this<Promise<T> >::shared_from_this());
    }

    ~Promise() = default;
};

template<typename T>
class Future
{
private:
    std::shared_ptr<Promise<T> > promise;
    bool completed;

public:
    explicit Future()
    {
        completed = false;
    }

    explicit Future(std::shared_ptr<Promise<T> > promise)
    {
        completed = false;
        this->promise = promise;
    }

    Future(const Future& other)
    {
        completed = false;
        promise = other.promise;
    }

    T& Get()
    {
        std::unique_lock lock(promise->m_mutex);
        if (!completed)
        {
            promise->m_condition.wait(lock);
        }
        if (promise->m_exception)
        {
            std::string message = promise->m_exception->what();
            throw std::runtime_error(message);
        }
        return *promise->m_value;
    }

    bool TryGet(T& value)
    {
        std::scoped_lock lock(promise->m_mutex);
        if (completed)
        {
            if (promise->m_exception)
            {
                std::string message = promise->m_exception->what();
                throw std::runtime_error(message);
            }
            value = *promise->m_value;
        }
        return completed;
    }

    ~Future() = default;
};