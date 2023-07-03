#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class DatabaseObjectBuilder;
class DatabaseObject {
public:
  DatabaseObject(const std::string &data = "", int version = 1)
      : data(data), version(version) {}
  friend class DatabaseObjectBuilder;
  int getVersion() const { return version; }

  void update(const std::string &newData, int newVersion) {
    if (newVersion == version) {
      data = newData;
      version++;
      std::cout << "Update successful. New version: " << version << std::endl;
    } else {
      std::cout << "Conflict detected! Requested version (" << newVersion
                << ") does not match current version (" << version << ")."
                << std::endl;
    }
  }

  void read() const {
    std::cout << "Data: " << data << ", Version: " << version << std::endl;
  }

private:
  std::string data;
  int version;
};
class DatabaseObjectBuilder {
public:
  DatabaseObjectBuilder &setData(const std::string &data) {
    object.data = data;
    return *this;
  }

  DatabaseObjectBuilder &setVersion(int version) {
    object.version = version;
    return *this;
  }

  std::unique_ptr<DatabaseObject> build() const {
    return std::make_unique<DatabaseObject>(object.data, object.version);
  }

private:
  DatabaseObject object;
};
class Database {
public:
  static Database &getInstance() {
    static Database instance; // Guaranteed to be initialized only once
    return instance;
  }
  bool addObject(const std::string &key, const std::string &data) {
    if (objects.find(key) != objects.end()) {
      return false; // Object with the same key already exists
    }
    objects[key] = DatabaseObject(data, 1);
    return true;
  }

  bool addObject(const std::string &key, std::unique_ptr<DatabaseObject> db_object) {
    if (objects.find(key) != objects.end()) {
      return false; // Object with the same key already exists
    }
    objects[key] = db_object;
    return true;
  }
  bool updateObject(const std::string &key, const std::string &newData,
                    int newVersion) {
    auto it = objects.find(key);
    if (it != objects.end()) {
      it->second.update(newData, newVersion);
      return true;
    }
    return false; // Object with the specified key not found
  }

  void readObject(const std::string &key) const {
    auto it = objects.find(key);
    if (it != objects.end()) {
      it->second.read();
    } else {
      std::cout << "Object with key '" << key << "' not found." << std::endl;
    }
  }

  int getLatestVersion(const std::string &key) const {
    auto it = objects.find(key);
    if (it != objects.end()) {
      return it->second.getVersion();
    }
    return -1; // Object with the specified key not found
  }

private:
  std::unordered_map<std::string, DatabaseObject> objects;
  Database() {} // Private constructor to prevent instantiation
};

int main() {
  Database db = Database::getInstance();

  std::unique_ptr<DatabaseObject>  obj =
      DatabaseObjectBuilder().setData("John Doe").setVersion(2).build();

  db.addObject("user123", "John Doe");

  db.addObject("password123", std::move(obj));

  db.readObject("user123"); // Output: Data: John Doe, Version: 1

  db.updateObject("user123", "Jane Doe",
                  1); // Output: Update successful. New version: 2
  db.updateObject("user123", "Alice Smith",
                  1); // Output: Conflict detected! Requested version (1) does
                      // not match current version (2).

  db.readObject("user123"); // Output: Data: Jane Doe, Version: 2

  int latestVersion = db.getLatestVersion("user123");
  if (latestVersion != -1) {
    std::cout << "Latest version of 'user123': " << latestVersion << std::endl;
  } else {
    std::cout << "Object with key 'user123' not found." << std::endl;
  }

  return 0;
}
