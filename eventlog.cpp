#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

// Event structure
struct Event {
    std::string timestamp;
    std::string message;
};

// Event Log class
class EventLog {
public:
    EventLog(const std::string& logFileName) : logFileName(logFileName) {}

    // Append an event to the log
    void appendEvent(const std::string& message) {
        Event event;
        event.timestamp = getTimestamp();
        event.message = message;
        events.push_back(event);

        // Append event to the log file
        std::ofstream logFile(logFileName, std::ios::app);
        if (logFile.is_open()) {
            logFile << event.timestamp << " - " << event.message << "\n";
            logFile.close();
        }
    }

    // Retrieve all events from the log
    std::vector<Event> getAllEvents() const {
        return events;
    }

    // Replay events from the log
    void replayEvents() const {
        std::ifstream logFile(logFileName);
        std::string line;
        while (std::getline(logFile, line)) {
            std::cout << line << std::endl;
        }
        logFile.close();
    }

private:
    std::string getTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::ctime(&now_time);
        return ss.str();
    }

    std::vector<Event> events;
    std::string logFileName;
};

int main() {
    // Create an event log instance with a file name
    EventLog eventLog("event_log.txt");

    // Append events to the log
    eventLog.appendEvent("Event 1");
    eventLog.appendEvent("Event 2");
    eventLog.appendEvent("Event 3");

    // Retrieve all events from the log
    std::vector<Event> allEvents = eventLog.getAllEvents();
    std::cout << "All Events:" << std::endl;
    for (const auto& event : allEvents) {
        std::cout << event.timestamp << " - " << event.message << std::endl;
    }

    // Replay events from the log
    std::cout << "Replaying Events:" << std::endl;
    eventLog.replayEvents();

    return 0;
}
