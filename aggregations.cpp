#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>

class MetricsAggregator {
public:
    void aggregate(const std::string& metricName, int value) {
        // Aggregate the metric value
        if (aggregations.find(metricName) == aggregations.end()) {
            // If metricName not found, initialize with the value
            aggregations[metricName] = value;
        } else {
            // If metricName already exists, update the aggregation
            aggregations[metricName] += value;
        }
    }

    void printAggregations() {
        std::cout << "Aggregated Metrics:" << std::endl;
        for (const auto& pair : aggregations) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }

private:
    std::unordered_map<std::string, int> aggregations;
};

class CollectionAgent {
public:
    CollectionAgent(MetricsAggregator& aggregator) : aggregator(aggregator) {}

    void collectMetrics() {
        // Simulating collecting metrics from different sources
        collectMetricA();
        collectMetricB();
        collectMetricC();
    }

    void collectMetricA() {
        // Simulating collection of metric A
        int value = 5; // Metric A value
        aggregator.aggregate("Metric A", value);
    }

    void collectMetricB() {
        // Simulating collection of metric B
        int value = 8; // Metric B value
        aggregator.aggregate("Metric B", value);
    }

    void collectMetricC() {
        // Simulating collection of metric C
        int value = 3; // Metric C value
        aggregator.aggregate("Metric C", value);
    }

private:
    MetricsAggregator& aggregator;
};

int main() {
    MetricsAggregator aggregator;
    CollectionAgent agent(aggregator);

    // Simulating continuous metric collection
    while (true) {
        agent.collectMetrics();
        aggregator.printAggregations();

        // Sleep for a specific time interval
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    return 0;
}
