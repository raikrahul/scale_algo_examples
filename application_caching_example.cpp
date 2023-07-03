#include <iostream>
#include <unordered_map>
#include <vector>
#include <deque>
#include <chrono>
#include <algorithm>

using namespace std;

class LiftWaitService {
private:
    unordered_map<string, pair<chrono::system_clock::time_point, vector<int>>> cache;
    deque<string> accessOrder;
    const int CACHE_SIZE = 100;  // Maximum cache size

public:
    vector<int> getLiftWaits(const string& resort) {
        if (cache.find(resort) != cache.end()) {
            const auto& cachedData = cache[resort];
            if (cachedData.first + chrono::seconds(300) > chrono::system_clock::now()) {
                cout << "Returning cached lift wait times for " << resort << endl;
                updateAccessOrder(resort);  // Update the access order
                return cachedData.second;
            }
        }

        vector<int> liftWaitTimes = getLiftWaitTimesFromAPI(resort);

        // Update cache with new data
        cache[resort] = { chrono::system_clock::now(), liftWaitTimes };
        updateAccessOrder(resort);  // Update the access order

        // Evict the least recently used item if the cache exceeds the maximum size
        if (cache.size() > CACHE_SIZE) {
            const string& evictKey = accessOrder.front();
            cache.erase(evictKey);
            accessOrder.pop_front();
            cout << "Evicted cache entry for " << evictKey << endl;
        }

        return liftWaitTimes;
    }

private:
    vector<int> getLiftWaitTimesFromAPI(const string& resort) {
        // Simulated implementation of expensive lift wait time calculations
        // In reality, this function would fetch the data from a downstream service or perform the calculations

        // Simulating random lift wait times
        vector<int> liftWaitTimes;
        for (int i = 0; i < 10; ++i) {
            int waitTime = rand() % 30 + 1;  // Random wait time between 1 and 30 minutes
            liftWaitTimes.push_back(waitTime);
        }

        cout << "Fetching lift wait times for " << resort << " from the API" << endl;

        return liftWaitTimes;
    }

    void updateAccessOrder(const string& resort) {
        // Update the access order by moving the resort to the back of the deque
        auto it = std::find(accessOrder.begin(), accessOrder.end(), resort);
        if (it != accessOrder.end()) {
            accessOrder.erase(it);
        }
        accessOrder.push_back(resort);
    }
};

int main() {
    LiftWaitService service;
    vector<int> waitTimes = service.getLiftWaits("resort1");
    // Use the waitTimes...

    return 0;
}
