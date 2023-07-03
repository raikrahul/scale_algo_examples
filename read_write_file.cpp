#include <iostream>
#include <fstream>
#include <string>

class DistributedLogStorage {
private:
    std::fstream logFile;  // File to store the log

public:
    DistributedLogStorage(const std::string& logFilePath) {
        logFile.open(logFilePath, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
    }

    void appendEvent(const std::string& event) {
        logFile.write(event.data(), event.size());  // Append the event to the log file
        logFile.put('\n');  // Add a newline character to separate events
        logFile.flush();  // Flush the buffer to ensure the event is written immediately
    }

    std::string readEvent(size_t offset) {
        std::string event;
        logFile.seekg(offset, std::ios::beg);  // Set the read position to the desired offset
        std::getline(logFile, event);  // Read the event from the log file
        return event;
    }
};

int main() {
    DistributedLogStorage logStorage("log.txt");

    // Append events to the log
    logStorage.appendEvent("Event 1");
    logStorage.appendEvent("Event 2");
    logStorage.appendEvent("Event 3");

    // Read events from the log
    std::string event1 = logStorage.readEvent(0);
    std::string event2 = logStorage.readEvent(10);  // Read from offset 10
    std::string event3 = logStorage.readEvent(20);  // Read from offset 20

    std::cout << "Event 1: " << event1 << std::endl;
    std::cout << "Event 2: " << event2 << std::endl;
    std::cout << "Event 3: " << event3 << std::endl;

    return 0;
}
