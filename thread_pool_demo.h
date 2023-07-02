
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace thread_pool_demo
{

    class ThreadPool
    {
    public:
        explicit ThreadPool(size_t numThreads)
            : stop(false)
        {
            for (size_t i = 0; i < numThreads; ++i)
            {
                threads.emplace_back([this]
                                     {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] {
                            return stop || !taskQueue.empty();
                        });

                        if (stop && taskQueue.empty())
                            return;

                        task = std::move(taskQueue.front());
                        taskQueue.pop();
                    }

                    task();
                } });
            }
        }

        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                stop = true;
            }

            condition.notify_all();

            for (std::thread &thread : threads)
                thread.join();
        }

        template <typename Func, typename... Args>
        void enqueue(Func &&func, Args &&...args)
        {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                taskQueue.emplace([func, args = std::make_tuple(std::forward<Args>(args)...)]() mutable
                                  { std::apply(std::move(func), std::move(args)); });
            }

            condition.notify_one();
        }

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> taskQueue;

        std::mutex queueMutex;
        std::condition_variable condition;

        bool stop;
    };

    // Example usage
    void printNumber(int number)
    {
        std::cout << "Number: " << number << std::endl;
    }

    int main()
    {
        // Create a thread pool with 4 threads
        ThreadPool pool(4);

        // Enqueue tasks
        for (int i = 0; i < 10; ++i)
        {
            pool.enqueue(printNumber, i);
        }

        // Wait for tasks to complete
        std::this_thread::sleep_for(std::chrono::seconds(1));

        return 0;
    }

}