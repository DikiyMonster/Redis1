#pragma once
#ifndef REDIS_H
#define REDIS_H

#include <unordered_map>
#include <mutex>
#include <chrono>

using namespace std;

class MyRedis {
private:
    unordered_map<string, pair<int, chrono::system_clock::time_point>> data;
    mutex mtx;
    bool isEnablePersistence;
    chrono::system_clock::time_point lastCleanupTime; // Added member for last cleanup time

public:
    MyRedis(bool enablePersistence = true);

    void set(const string& key, int value, chrono::seconds expiration = chrono::seconds(0));

    int get(const string& key);

private:
    void saveToDisk();

    void loadFromDisk();

    void cleanup(); 
};

#endif