//
//  main.cpp
//  EventEmitter
//
//  Created by BlueCocoa on 2016/8/23.
//  Copyright © 2016 BlueCocoa. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "EventEmitter.hpp"

using namespace std;

class emitter : public EventEmitter {};

int main(int argc, const char * argv[]) {
    emitter emitter;
    emitter.on("event", [&emitter](int data) {
        ostringstream osstream;
        osstream << "[Listener 1] data: " << data << '\n';
        std::cout<<osstream.str();
    });
    emitter.on("event", [&emitter](int data) {
        ostringstream osstream;
        osstream << "[Listener 2] data: " << data << '\n';
        std::cout<<osstream.str();
    });
    emitter.once("event", [&emitter](int data) {
        ostringstream osstream;
        osstream << "[Once Listener] data: " << data << '\n';
        std::cout<<osstream.str();
    });
    
    std::cout<<emitter.listener_count("event")<<" listeners for 'event'\n";
    
    vector<thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&emitter, i]() {
            emitter.emit("event", i);
        });
    }
    
    for (auto &t : threads) t.join();
    
    std::cout<<emitter.listener_count("event")<<" listeners for 'event'\n";
}