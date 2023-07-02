
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>

namespace APIDemo
{

    class API
    {
    private:
        // std::unordered_map<std::string, std::string> skierProfiles;
        std::vector<std::unordered_map<std::string, std::string>> skierProfiles;

    public:
        API(int numInstances) : skierProfiles(numInstances) {}

        // POST operation - Create a new skier profile
        void createSkierProfile(const std::string &skierID, const std::string &profileData)
        {
            // demo a load balancer
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, skierProfiles.size() - 1);

            int selectedInstance = dis(gen);
            skierProfiles[selectedInstance][skierID] = profileData;
            // skierProfiles[skierID] = profileData;
            // std::cout << "New skier profile created for ID: " << skierID << std::endl;
            std::cout << "New skier profile created for ID: " << skierID << " on instance: " << selectedInstance << std::endl;
        }

        // GET operation - Retrieve skier profile information
        std::string getSkierProfile(const std::string &skierID)
        {

            for (const auto &profiles : skierProfiles)
            {
                if (profiles.count(skierID) > 0)
                {
                    std::cout << "Skier profile retrieved for ID: " << skierID << std::endl;
                    return profiles.at(skierID);
                }
            }

            std::cout << "Skier profile not found for ID: " << skierID << std::endl;
            return "";

            // if (skierProfiles.count(skierID) > 0)
            // {
            //     std::cout << "Skier profile retrieved for ID: " << skierID << std::endl;
            //     return skierProfiles[skierID];
            // }
            // else
            // {
            //     std::cout << "Skier profile not found for ID: " << skierID << std::endl;
            //     return "";
            // }
        }

        // PUT operation - Update skier profile
        void updateSkierProfile(const std::string &skierID, const std::string &updatedProfileData)
        {
            // if (skierProfiles.count(skierID) > 0)
            // {
            //     skierProfiles[skierID] = updatedProfileData;
            //     std::cout << "Skier profile updated for ID: " << skierID << std::endl;
            // }
            // else
            // {
            //     std::cout << "Skier profile not found for ID: " << skierID << std::endl;
            // }

            for (auto &profiles : skierProfiles)
            {
                if (profiles.count(skierID) > 0)
                {
                    profiles[skierID] = updatedProfileData;
                    std::cout << "Skier profile updated for ID: " << skierID << std::endl;
                    return;
                }
            }

            std::cout << "Skier profile not found for ID: " << skierID << std::endl;
        }

        // DELETE operation - Delete skier profile
        void deleteSkierProfile(const std::string &skierID)
        {
            // {
            //     if (skierProfiles.count(skierID) > 0)
            //     {
            //         skierProfiles.erase(skierID);
            //         std::cout << "Skier profile deleted for ID: " << skierID << std::endl;
            //     }
            //     else
            //     {
            //         std::cout << "Skier profile not found for ID: " << skierID << std::endl;
            //     }
            // }

            for (auto &profiles : skierProfiles)
            {
                if (profiles.count(skierID) > 0)
                {
                    profiles.erase(skierID);
                    std::cout << "Skier profile deleted for ID: " << skierID << std::endl;
                    return;
                }
            }

            std::cout << "Skier profile not found for ID: " << skierID << std::endl;
        }

        std::string processRequest(const std::string &method, const std::string &uri, const std::string &body)
        {
            if (method == "GET")
            {
                return getSkierProfile(uri);
            }
            else if (method == "PUT")
            {
                updateSkierProfile(uri, body);
                return getSkierProfile(uri);
            }
            else if (method == "DELETE")
            {
                deleteSkierProfile(uri);
                return "";
            }
            else if (method == "POST")
            {
                // Assuming the body contains skier ID and profile data in JSON format
                json requestBody = json::parse(body);
                std::string skierID = requestBody["skierID"];
                std::string profileData = requestBody["profileData"];
                createSkierProfile(skierID, profileData);
                return getSkierProfile(skierID);
            }

            return "";
        }
    };

    int main()
    {
        API api(3);

        // Usage example
        api.createSkierProfile("12345", R"({"username": "Ian123", "email": "i.gorton@somewhere.com", "city": "Seattle"})");
        std::cout << api.getSkierProfile("12345") << std::endl;

        api.updateSkierProfile("12345", R"({"username": "Ian123", "email": "i.gorton@somewhere.com", "city": "Wenatchee"})");
        std::cout << api.getSkierProfile("12345") << std::endl;

        api.deleteSkierProfile("12345");
        std::cout << api.getSkierProfile("12345") << std::endl;

        std::string method = "GET";
        std::string uri = "/skico.com/skiers/12345";
        std::string requestBody = "";

        std::string response = api.processRequest(method, uri, requestBody);
        std::cout << "Response: " << response << std::endl;

        return 0;
    }

} // namesapce APIDEMO
