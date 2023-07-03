#include <iostream>
#include <vector>
#include <algorithm>

class LogicalClock
{
public:
    LogicalClock() : localClock(0) {}

    void sendMessage(int message)
    {
        localClock++;
        std::cout << "Sending message: " << message << " (Local Clock: " << localClock << ")" << std::endl;
    }

    void receiveMessage(int message, int senderClock)
    {
        localClock = std::max(senderClock, localClock) + 1;
        std::cout << "Received message: " << message << " (Local Clock: " << localClock << ")" << std::endl;
    }

    int getLocalClock() const
    {
        return localClock;
    }

private:
    int localClock;
};

int main()
{
    LogicalClock process1;
    LogicalClock process2;

    process1.sendMessage(1);
    process2.receiveMessage(1, process1.getLocalClock());

    process2.sendMessage(2);
    process1.receiveMessage(2, process2.getLocalClock());

    return 0;
}
