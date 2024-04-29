#include "Redis1.h"
#include <fstream>

MyRedis::MyRedis(bool enablePersistence) : isEnablePersistence(enablePersistence) {
    if (isEnablePersistence)
        loadFromDisk();
    lastCleanupTime = chrono::system_clock::now(); 
}

void MyRedis::set(const string& key, int value, chrono::seconds expiration) {
    lock_guard<mutex> lock(mtx);
    data[key] = make_pair(value, chrono::system_clock::now() + expiration);
    if (isEnablePersistence)
        saveToDisk();
    
    if (chrono::system_clock::now() - lastCleanupTime >= chrono::minutes(1)) {
        cleanup();
        lastCleanupTime = chrono::system_clock::now();
    }
}

int MyRedis::get(const string& key) {
    lock_guard<mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        auto expiration = it->second.second;
        if (expiration == chrono::time_point<chrono::system_clock>()) 
            return it->second.first;
        else if (expiration > chrono::system_clock::now())
            return it->second.first;
        else {
            data.erase(it); 
            return -1; 
        }
    }
    else {
        return -1; 
    }
}

void MyRedis::saveToDisk() {
    ofstream file("redis_state.txt");
    for (const auto& pair : data)
        file << pair.first << " " << pair.second.first << endl;
    file.close();
}

void MyRedis::loadFromDisk() {
    ifstream file("redis_state.txt");
    if (file) {
        string key;
        int value;
        while (file >> key >> value) {
            data[key] = make_pair(value, chrono::system_clock::now());
        }
        file.close();
    }
}

void MyRedis::cleanup() {
    auto current_time = chrono::system_clock::now();
    for (auto it = data.begin(); it != data.end();) {
        if (it->second.second < current_time) {
            it = data.erase(it);
        }
        else {
            ++it;
        }
    }
}
