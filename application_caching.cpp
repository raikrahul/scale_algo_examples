#include <iostream>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <atomic>
#include <stdexcept>

template <typename Key, typename Value>
class Cache {
public:
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    Cache(std::chrono::seconds expiryTime, std::function<Value(const Key&)> fetchFunc, std::size_t maxCacheSize)
        : expiryTime_(expiryTime), fetchFunc_(fetchFunc), maxCacheSize_(maxCacheSize), circuitBreakerOpen_(false)
    {
    }

    Value get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = cache_.find(key);
        if (it != cache_.end() && std::chrono::steady_clock::now() < it->second.first) {
            return it->second.second;
        }

        if (!circuitBreakerOpen_.load(std::memory_order_acquire)) {
            try {
                std::future<Value> future = std::async(std::launch::async, fetchFunc_, key);
                std::future_status status = future.wait_for(std::chrono::seconds(1));

                if (status == std::future_status::ready) {
                    Value value = future.get();
                    cache_[key] = std::make_pair(std::chrono::steady_clock::now() + expiryTime_, value);
                    cleanupCache();
                    return value;
                }
                else if (status == std::future_status::timeout) {
                    circuitBreakerOpen_.store(true, std::memory_order_release);
                    throw std::runtime_error("Fetch function timed out.");
                }
            }
            catch (const std::exception& e) {
                // Handle fetch function exception
                std::cerr << "Fetch function error: " << e.what() << std::endl;
                circuitBreakerOpen_.store(true, std::memory_order_release);
                throw;
            }
        }

        return Value(); // Return default value if fetch function failed or circuit breaker is open
    }

    void remove(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(key);
    }

private:
    std::unordered_map<Key, std::pair<TimePoint, Value>> cache_;
    std::chrono::seconds expiryTime_;
    std::function<Value(const Key&)> fetchFunc_;
    std::size_t maxCacheSize_;
    std::atomic<bool> circuitBreakerOpen_;
    std::mutex mutex_;

    void cleanupCache() {
        if (cache_.size() > maxCacheSize_) {
            TimePoint now = std::chrono::steady_clock::now();
            auto it = cache_.begin();
            while (it != cache_.end()) {
                if (it->second.first < now) {
                    it = cache_.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }
};

// Example usage
int main() {
    // Simulating a slow fetch function
    auto slowFetchFunc = [](const std::string& key) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return key + " value";
    };

    // Create a cache with 5 seconds expiry time, the slow fetch function, and a maximum cache size of 10
    Cache<std::string, std::string> cache(std::chrono::seconds(5), slowFetchFunc, 10);

    try {
        // Retrieve values from the cache
        std::cout << cache.get("key1") << std::endl; // Fetches and caches the value
        std::cout << cache.get("key1") << std::endl; // Returns the cached value

        std::cout << cache.get("key2") << std::endl; // Fetches and caches the value
        std::cout << cache.get("key2") << std::endl; // Returns the cached value

        // Wait for cache expiration
        std::this_thread::sleep_for(std::chrono::seconds(6));

        std::cout << cache.get("key1") << std::endl; // Fetches and caches the value (expired)
        std::cout << cache.get("key2") << std::endl; // Fetches and caches the value (expired)
    }
    catch (const std::exception& e) {
        // Handle cache exception
        std::cerr << "Cache error: " << e.what() << std::endl;
    }

    return 0;
}
