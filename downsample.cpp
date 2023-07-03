#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct MetricData {
    int timestamp;
    std::string hostname;
    std::string metric;
    int value;
};

class Downsampler {
public:
    void addMetricData(const MetricData& data) {
        metricData.push_back(data);
    }

    void downsample() {
        // Sort metric data by timestamp in ascending order
        std::sort(metricData.begin(), metricData.end(), [](const MetricData& a, const MetricData& b) {
            return a.timestamp < b.timestamp;
            });

        // Downsample the metric data based on defined rules
        downsampleToMinuteResolution();
        downsampleToHourResolution();

        // Print the downsampled metric data
        printDownsampledData();
    }

private:
    std::vector<MetricData> metricData;
    std::unordered_map<std::string, std::vector<MetricData>> downsampledData;

    void downsampleToMinuteResolution() {
        for (const MetricData& data : metricData) {
            if (data.timestamp <= 30 * 24 * 60 * 60) { // Retention: 30 days
                // Downsample to 1-minute resolution
                std::string key = data.hostname + "_" + data.metric;
                downsampledData[key].push_back(data);
            }
        }
    }

    void downsampleToHourResolution() {
        for (auto& pair : downsampledData) {
            const std::string& key = pair.first;
            std::vector<MetricData>& metricList = pair.second;

            if (metricList.size() > 0 && metricList.back().timestamp > 365 * 24 * 60 * 60) { // Retention: 1 year
                // Downsample to 1-hour resolution
                std::vector<MetricData> downsampled;
                int currentHour = metricList[0].timestamp / (60 * 60);
                int sum = 0;
                int count = 0;

                for (const MetricData& data : metricList) {
                    int hour = data.timestamp / (60 * 60);
                    if (hour == currentHour) {
                        sum += data.value;
                        count++;
                    }
                    else {
                        MetricData downsampledData;
                        downsampledData.timestamp = currentHour * (60 * 60);
                        downsampledData.hostname = data.hostname;
                        downsampledData.metric = data.metric;
                        downsampledData.value = sum / count;
                        downsampled.push_back(downsampledData);

                        currentHour = hour;
                        sum = data.value;
                        count = 1;
                    }
                }

                metricList = downsampled;
            }
        }
    }

    void printDownsampledData() {
        std::cout << "Downsampled Metrics:" << std::endl;
        for (const auto& pair : downsampledData) {
            const std::string& key = pair.first;
            const std::vector<MetricData>& metricList = pair.second;

            std::cout << "Key: " << key << std::endl;
            for (const MetricData& data : metricList) {
                std::cout << "Timestamp: " << data.timestamp << ", Value: " << data.value << std::endl;
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    Downsampler downsampler;

    // Add metric data
    downsampler.addMetricData({ 10, "host-a", "cpu", 16 });
    downsampler.addMetricData({ 20, "host-a", "cpu", 20 });
    downsampler.addMetricData({ 30, "host-a", "cpu", 30 });

    // Downsample the metric data
    downsampler.downsample();

    return 0;
}
