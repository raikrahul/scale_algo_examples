
#include <iostream>
#include <unordered_map>

namespace state_less_demo
{

    // Structure to represent the skier profile
    struct SkierProfile
    {
        std::string username;
        std::string email;
        std::string city;
    };

    // Class representing the service API
    class SkierService
    {
    private:
        std::unordered_map<std::string, SkierProfile> skierProfiles;

    public:
        // GET request handler to retrieve the skier profile
        SkierProfile GetSkierProfile(const std::string &skierID)
        {
            return skierProfiles[skierID];
        }

        // PUT request handler to update the skier profile
        void UpdateSkierProfile(const std::string &skierID, const SkierProfile &profile)
        {
            skierProfiles[skierID] = profile;
        }
    };

    int main()
    {
        SkierService service;

        // Simulating a GET request to retrieve the skier profile
        std::string skierID = "768934";
        SkierProfile skierProfile = service.GetSkierProfile(skierID);
        std::cout << "Skier Profile (GET):\n";
        std::cout << "Username: " << skierProfile.username << "\n";
        std::cout << "Email: " << skierProfile.email << "\n";
        std::cout << "City: " << skierProfile.city << "\n\n";

        // Simulating a PUT request to update the skier profile
        SkierProfile updatedProfile = skierProfile;
        updatedProfile.username = "Ian123";
        updatedProfile.email = "i.gorton@somewhere.com";
        updatedProfile.city = "Wenatchee";
        service.UpdateSkierProfile(skierID, updatedProfile);
        std::cout << "Skier Profile (PUT):\n";
        std::cout << "Username: " << updatedProfile.username << "\n";
        std::cout << "Email: " << updatedProfile.email << "\n";
        std::cout << "City: " << updatedProfile.city << "\n";

        return 0;
    }

}