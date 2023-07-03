#include <iostream>
#include <vector>
#include <string>
#include <queue>

// MetricsSource class representing a server being monitored
class MetricsSource
{
public:
    MetricsSource(const std::string &name) : name(name) {}

    void pushMetrics()
    {
        // Simulating pushing metrics to the metrics collector
        std::cout << "Pushing metrics from " << name << " to the MetricsCollector." << std::endl;
        // Push the metrics data to the MetricsCollector class
        metricsCollector.collectMetricsFromSource(name);
    }

private:
    std::string name;
    // Reference to the MetricsCollector class
    class MetricsCollector &metricsCollector;
};

// MetricsCollector class responsible for collecting metrics
class MetricsCollector
{
public:
    void addMetricsSource(MetricsSource *source)
    {
        sources.push_back(source);
    }

    void collectMetricsFromSource(const std::string &sourceName)
    {
        // Collect metrics from the specified source
        // Process the metrics, store in time series DB, send notifications, etc.
        std::cout << "Collecting metrics from " << sourceName << "." << std::endl;

        if (buffer.size() >= bufferSize)
        {
            // Handle the buffer overflow or send an error response to the sources
            std::cout << "Error: Buffer full, metrics from " << sourceName << " rejected." << std::endl;
        }
        else
        {
            // Push the metrics data to the buffer
            buffer.push(sourceName + " metrics");
        }
    }

    void collectMetricsFromAllSources()
    {
        for (MetricsSource *source : sources)
        {
            source->pushMetrics();
        }
    }


    void processMetricsBuffer() {
        // Process the metrics from the buffer
        while (!buffer.empty()) {
            std::string metrics = buffer.front();
            buffer.pop();
            // Process the metrics, store in time series DB, send notifications, etc.
            std::cout << "Processing metrics: " << metrics << std::endl;
        }
    }

private:
    std::vector<MetricsSource *> sources;
    std::queue<std::string> buffer;
    size_t bufferSize
};

// MetricsCollectionSystem class coordinating the metrics sources and collector
class MetricsCollectionSystem
{
public:
    MetricsCollectionSystem()
    {
        // Create and connect the required objects
        metricsCollector.addMetricsSource(&webServer);
        metricsCollector.addMetricsSource(&dbCluster);
        metricsCollector.addMetricsSource(&queueCluster);
        metricsCollector.addMetricsSource(&cacheCluster);
    }

    void run()
    {
        // Collect metrics from all sources
        metricsCollector.collectMetricsFromAllSources();
        // Process the metrics, store in time series DB, send notifications, etc.
        // emailNotification.sendNotification("Metrics collected successfully");
    }

private:
    MetricsCollector metricsCollector;
    MetricsSource webServer{"Web Server"};
    MetricsSource dbCluster{"DB Cluster"};
    MetricsSource queueCluster{"Queue Cluster"};
    MetricsSource cacheCluster{"Cache Cluster"};

    // Rest of the code...
};

int main()
{
    MetricsCollectionSystem system;
    system.run();

    return 0;
}
