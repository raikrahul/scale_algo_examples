#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

template <typename... Args>
struct ContainsStringArgs;

template <typename First, typename... Rest>
struct ContainsStringArgs<First, Rest...>
{
    static constexpr bool value =
        std::is_same_v<std::decay_t<First>, std::string> ||
        ContainsStringArgs<Rest...>::value;
};

template <>
struct ContainsStringArgs<>
{
    static constexpr bool value = false;
};

class ThreadPool
{
public:
    explicit ThreadPool(size_t numThreads) : stop(false), stringTasksCounter(0)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([this]
                                 {
        while (true) {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !taskQueue.empty(); });

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

    //   template <typename Func, typename... Args>
    //   auto enqueue(Func &&func, Args &&...args) ->
    //   std::future<std::conditional_t<
    //       std::is_same_v<void, std::invoke_result_t<Func, Args...>>, int,
    //       std::invoke_result_t<Func, Args...>>> {
    //     using return_type = std::conditional_t<
    //         std::is_same_v<void, std::invoke_result_t<Func, Args...>>, int,
    //         std::invoke_result_t<Func, Args...>>;

    //     auto task = std::make_shared<std::packaged_task<return_type()>>(
    //         std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

    //     std::future<return_type> result = task->get_future();

    //     {
    //       std::unique_lock<std::mutex> lock(queueMutex);
    //       taskQueue.emplace([task, this]() {
    //         if (ContainsStringArgs<Args...>::value) {
    //           std::unique_lock<std::mutex> counterLock(counterMutex);
    //           ++stringTasksCounter;

    //           std::cout << "\nEnque a taks which contains string\n";
    //         }

    //         (*task)();
    //       });
    //     }

    //     condition.notify_one();

    //     return result;
    //   }

    template <typename Func, typename... Args>
    auto enqueue(Func &&func, Args &&...args) -> std::future<std::conditional_t<
        std::is_same_v<void, std::invoke_result_t<Func, Args...>>, int,
        std::invoke_result_t<Func, Args...>>>

    {
        using return_type = std::conditional_t<
            std::is_same_v<void, std::invoke_result_t<Func, Args...>>, int,
            std::invoke_result_t<Func, Args...>>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

        std::future<return_type> result = task->get_future();

        if constexpr (std::is_same_v<void, return_type>) {

             std::cout << "Detected void return type." << std::endl;
        }


        {
            std::unique_lock<std::mutex> lock(queueMutex);
            taskQueue.emplace(
                [task, this, args = std::make_tuple(std::forward<Args>(args)...)]()
                {
                    if constexpr (ContainsStringArgs<Args...>::value)
                    {
                        std::unique_lock<std::mutex> counterLock(counterMutex);
                        ++stringTasksCounter;

                        std::cout << "Enqueued a task that contains a string argument."
                                  << std::endl;
                    }

                    std::apply([&task]()
                               { (*task)(); },
                               args);
                });
        }

        condition.notify_one();

        return result;
    }

    int getStringTasksCounter()
    {
        std::unique_lock<std::mutex> lock(counterMutex);
        return stringTasksCounter;
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> taskQueue;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::mutex counterMutex;
    int stringTasksCounter;
    bool stop;
};

// Example usage
template <typename Return, typename... Args>
Return printNumber(Args &&...args)
{

    using CommonType = typename std::common_type<Args...>::type;
    std::cout << "\n the arguments are as \n";
    ((std::cout << std::forward<Args>(args) << " "), ...);
    std::cout << std::endl;
    return Return();
}

int main()
{
    // Create a thread pool with 4 threads
    ThreadPool pool(4);

    std::function<std::string()> func1X = []()
    {
        return printNumber<std::string>(1, 2, 3, 3);
    };

    std::function<std::string()> func2X = []()
    {
        return printNumber<std::string>("helloX", "worldX");
    };
    
    std::function<void()> func3 = []()
    {
        std::cout << "\nHello World Void\n" << std::endl;
    };
    auto result1 =
        pool.enqueue([]()
                     { return printNumber<std::string>(1, 2, 3, 3); });
    auto result2 =
        pool.enqueue([]()
                     { return printNumber<std::string>("hello", "world"); });

    auto result1X = pool.enqueue(func1X);
    auto result2X = pool.enqueue(func2X);

    auto resultVoid = pool.enqueue(func3);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    try
    {
        // Get the results of the tasks
        std::cout << "Result 1: " << result1.get() << std::endl;
        std::cout << "Result 2: " << result2.get() << std::endl;
        std::cout << "Result 2x: " << result2X.get() << std::endl;
        std::cout << "Result 1X: " << result1X.get() << std::endl;
        std::cout << "Result VOID: " << resultVoid.get() << std::endl;
        int stringTasksCounter = pool.getStringTasksCounter();
        std::cout << "Number of tasks with string arguments: " << stringTasksCounter
                  << std::endl;
    }
    catch (const std::exception &ex)
    {
        // Handle exceptions thrown by tasks
        // Log or handle the exception as needed
    }

    return 0;
}