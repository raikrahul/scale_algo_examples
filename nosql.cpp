#include <iostream>
#include <string>
#include <vector>

struct SnowSportPerson {
    int ssp_id;
    std::string ssp_name;
    std::string address;
    std::string dob;
    // ... other attributes
};

struct Resort {
    int resort_id;
    std::string resort_name;
    std::string location;
    // ... other attributes
};

struct VisitDay {
    std::string date;
    std::string resort_name;
    int ssp_id;
    std::string ssp_name;
    int numLifts;
    int vertical;
    int maxtemp;
    int mintemp;
    int wind;
};

std::vector<SnowSportPerson> snowSportPersons;
std::vector<Resort> resorts;
std::vector<VisitDay> visitDays;

void generateVisits(int sspId) {
    std::cout << "Summary for SnowSportPerson ID: " << sspId << std::endl;
    int totalDays = 0;

    for (const auto& visitDay : visitDays) {
        if (visitDay.ssp_id == sspId) {
            totalDays++;
            std::cout << visitDay.date << "\t"
                      << visitDay.resort_name << "\t"
                      << visitDay.numLifts << "\t"
                      << visitDay.vertical << "\t"
                      << visitDay.maxtemp << "/" << visitDay.mintemp << "\t"
                      << visitDay.wind << std::endl;
        }
    }

    std::cout << "Number of days: " << totalDays << std::endl;
}

int main() {
    // Populate SnowSportPerson, Resort, and VisitDay data
    snowSportPersons.push_back({ 1, "Ian Gorton", "123 Main St", "1980-01-01" });
    resorts.push_back({ 1, "49 Degrees North", "Washington" });
    resorts.push_back({ 2, "Silver Mt.", "Idaho" });
    visitDays.push_back({ "Dec 2nd 2021", "49 Degrees North", 1, "Ian Gorton", 17, 27200, 27, 19, 11 });
    visitDays.push_back({ "Dec 9th", "Silver Mt.", 1, "Ian Gorton", 14, 22007, 32, 16, 3 });

    // Generate visits for a specific SnowSportPerson (using ID)
    generateVisits(1);

    return 0;
}
