#include <algorithm>
#include <functional>
#include <iostream>
#include <limits.h>
#include <memory>
#include <thread>
#include <vector>

namespace load_balance_service {
class LoadDistributionPolicy;

class Service {
public:
   Service(const std::string& ip, int port, int capacity)
        : ip_(ip), port_(port), load_(0), capacity_(capacity) {}
  std::string getIP() const { return ip_; }
  int getPort() const { return port_; }
  int getLoad() const { return load_; }

  void increaseLoad() { ++load_; }
  void decreaseLoad() { --load_; }
  int getCapacity() const { return capacity_; }

private:
  std::string ip_;
  int port_;
  int load_;
  int capacity_;

}; // end of class Service

class LoadDistributionPolicy {
public:
  virtual Service *chooseService(const std::vector<Service> &services) = 0;
};

class RoundRobinPolicy : public LoadDistributionPolicy {
public:
  Service *chooseService(const std::vector<Service> &services) override {
    if (services.empty())
      return nullptr;

    currentIndex_ = (currentIndex_ + 1) % services.size();
    return const_cast<Service *>(&services[currentIndex_]);
  }

private:
  size_t currentIndex_ = 0;
};

class WeightedRoundRobinPolicy : public LoadDistributionPolicy {
public:
  Service *chooseService(const std::vector<Service> &services) override {
    if (services.empty())
      return nullptr;

    int minLoad = INT_MAX;
    Service *selectedService = nullptr;

    for (auto &service : services) {
      auto current_load = service.getLoad();
      if (current_load < minLoad) {
        minLoad = service.getLoad();
        selectedService = const_cast<Service *>(&service);
      }
    }

    return selectedService;
  }
};

class HealthMonitoring {
public:
  void monitorLoad(const std::vector<Service> &services) {
    for (const auto &service : services) {
      double loadPercentage =
          (static_cast<double>(service.getLoad()) / service.getCapacity()) *
          100.0;
      if (loadPercentage >= 75.0) {
        std::cout << "High load detected in service " << service.getIP() << ":"
                  << service.getPort() << std::endl;
      }
    }
  }

  void monitorRequests(int totalRequests, int capacity) {
    if (totalRequests > capacity) {
      std::cout << "More requests coming to the load balancer than the total "
                   "capacity of all the services"
                << std::endl;
    }
  }

  void monitorRequestTime(int millisecondsThreshold) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(millisecondsThreshold));
    std::cout << "A service is taking too much time to complete a request"
              << std::endl;
  }
};

class LoadBalancer {
public:
  void addService(const Service &service) { services_.push_back(service); }

  void removeService(const Service &service) {
    auto it = std::find(services_.begin(), services_.end(), service);
    if (it != services_.end()) {
      services_.erase(it);
    }
  }

  Service *chooseService() {
    if (loadDistributionPolicy_) {
      return loadDistributionPolicy_->chooseService(services_);
    }

    return nullptr;
  }



   Service* getNextTargetService(const std::string& sessionId) {
        // Check if the session has a target service assigned
        auto it = sessionAffinityMap.find(sessionId);
        if (it != sessionAffinityMap.end()) {
            // Return the assigned target service
            return it->second;
        }

        // Apply load balancing policy if session affinity not found
        // (Round-robin policy used as an example)
        Service* target = services.front();
        services.erase(services.begin());
        services.push_back(target);

        return target;
    }


     void processRequest(const std::string& sessionId) {
        Service* targetService = getNextTargetService(sessionId);

        // Simulating request processing
        std::cout << "Processing request with session ID: " << sessionId << " at service "
                  << targetService->getIP() << ":" << targetService->getPort() << std::endl;

        // Update session affinity map with the assigned target service
        sessionAffinityMap[sessionId] = targetService;
    }
  void
  setLoadDistributionPolicy(std::unique_ptr<LoadDistributionPolicy> policy) {
    loadDistributionPolicy_ = std::move(policy);
  }


    void monitorLoad() {
        healthMonitoring_.monitorLoad(services_);
    }

    void monitorRequests(int totalRequests) {
        int capacity = 0;
        for (const auto& service : services_) {
            capacity += service.getCapacity();
        }

        healthMonitoring_.monitorRequests(totalRequests, capacity);
    }

    void monitorRequestTime(int millisecondsThreshold) {
        healthMonitoring_.monitorRequestTime(millisecondsThreshold);
    }


     void scaleUp() {
        // Logic to add more replicas (services) based on load
        int maxCapacity = 0;
        for (const auto& service : services_) {
            maxCapacity = std::max(maxCapacity, service.getCapacity());
        }

        Service newService("192.168.0.4", 8080, maxCapacity);
        services_.push_back(newService);
        std::cout << "Added a new replica to handle increased load" << std::endl;
    }

private:
  std::vector<Service> services_;
  std::vector<Service*> services;
  size_t currentIndex_ = 0;
  std::unique_ptr<LoadDistributionPolicy> loadDistributionPolicy_;
  HealthMonitoring healthMonitoring_;
  std::unordered_map<std::string, Service*> sessionAffinityMap;
};

int main() {
  LoadBalancer loadBalancer;

  // Add services to the load balancer
  Service service1("192.168.0.1", 8080, 100 );
  Service service2("192.168.0.2", 8080, 150);
  Service service3("192.168.0.3", 8080, 200);

  loadBalancer.addService(service1);
  loadBalancer.addService(service2);
  loadBalancer.addService(service3);

  // Choose a service and simulate load increase
  Service *chosenService = loadBalancer.chooseService();
  loadBalancer.setLoadDistributionPolicy(std::make_unique<RoundRobinPolicy>());
  if (chosenService != nullptr) {
    chosenService->increaseLoad();
    std::cout << "Chosen service: " << chosenService->getIP() << ":"
              << chosenService->getPort() << std::endl;
  } else {
    std::cout << "No service available." << std::endl;
  }

  // Simulating high load in service1
    for (int i = 0; i < 75; ++i) {
        service1.increaseLoad();
    }

    // Monitoring load
    loadBalancer.monitorLoad();

    // Monitoring requests
    int totalRequests = 500;
    loadBalancer.monitorRequests(totalRequests);

    // Monitoring request time
    int millisecondsThreshold = 3000;
    loadBalancer.monitorRequestTime(millisecondsThreshold);
     // Simulating increased load
    loadBalancer.scaleUp();

    loadBalancer.processRequest("session1");
    loadBalancer.processRequest("session2");
    loadBalancer.processRequest("session1");
    loadBalancer.processRequest("session3");
    loadBalancer.processRequest("session2");
  return 0;
}
} // namespace load_balance_service

// end of namespace