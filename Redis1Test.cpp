#include "Redis1.h"
#include <thread>
#include <iostream>

using namespace std;

void testRedis() {
    MyRedis redis;

    auto setFunc = [&redis](const std::string& key, int value, chrono::seconds expiration) {
        redis.set(key, value, expiration);
        };

    auto getFunc = [&redis](const string& key) {
        return redis.get(key);
        };

    thread t1(setFunc, "key1", 10, chrono::seconds(3)); // 
    thread t2(getFunc, "key1");

    thread t3(setFunc, "key2", 20, chrono::seconds(5)); 
    thread t4(setFunc, "key3", 30, chrono::seconds(7)); 

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    this_thread::sleep_for(std::chrono::seconds(8)); 

    int value_key1 = getFunc("key1");
    int value_key2 = getFunc("key2");
    int value_key3 = getFunc("key3");

    if (value_key1 == -1 && value_key2 == -1 && value_key3 == -1) {
        cout << "Ключи с истекшим сроком действия успешно удалены." << endl;
    }
    else {
        cout << "Тест не пройден: ключи с истекшим сроком действия не были удалены." << endl;
    }
}

int main() {
    testRedis();
    return 0;
}