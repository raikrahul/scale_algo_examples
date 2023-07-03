#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>

struct Alert {
    std::string instance;
    std::string eventType;
    std::chrono::system_clock::time_point timestamp;
};

class AlertManager {
public:
    void addAlert(const Alert& alert) {
        alerts.push_back(alert);
    }

    void mergeAlerts() {
        // Sort alerts by instance and timestamp
        std::sort(alerts.begin(), alerts.end(), [](const Alert& a, const Alert& b) {
            if (a.instance != b.instance) {
                return a.instance < b.instance;
            }
            return a.timestamp < b.timestamp;
            });

        // Merge and dedupe alerts
        std::unordered_map<std::string, Alert> mergedAlerts;
        for (const Alert& alert : alerts) {
            std::string key = alert.instance + "_" + alert.eventType;
            auto it = mergedAlerts.find(key);
            if (it != mergedAlerts.end()) {
                // Alert for the same instance and event type exists, update the timestamp
                it->second.timestamp = alert.timestamp;
            }
            else {
                // New alert, add it to the merged alerts
                mergedAlerts[key] = alert;
            }
        }

        // Print the merged alerts
        printMergedAlerts(mergedAlerts);
    }

private:
    std::vector<Alert> alerts;

    void printMergedAlerts(const std::unordered_map<std::string, Alert>& mergedAlerts) {
        std::cout << "Merged Alerts:" << std::endl;
        for (const auto& pair : mergedAlerts) {
            const Alert& alert = pair.second;
            std::cout << "Instance: " << alert.instance << ", EventType: " << alert.eventType
                << ", Timestamp: " << std::chrono::system_clock::to_time_t(alert.timestamp) << std::endl;
        }
    }
};

int main() {
    AlertManager alertManager;

    // Add example alerts
    alertManager.addAlert({ "Instance 1", "disk_usage > 90%", std::chrono::system_clock::now() });
    alertManager.addAlert({ "Instance 1", "disk_usage > 90%", std::chrono::system_clock::now() });
    alertManager.addAlert({ "Instance 1", "disk_usage > 90%", std::chrono::system_clock::now() });

    // Merge and dedupe the alerts
    alertManager.mergeAlerts();

    return 0;
}
