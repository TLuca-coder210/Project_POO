//
// Created by Luca Teodorescu on 27.03.2026.
//

#ifndef PROJECT_OOP_EXCHANGEBUFFER_H
#define PROJECT_OOP_EXCHANGEBUFFER_H

#include <iostream>
#include <string>
#include <vector>
#include <atomic>

class Bank;

class ExchangeBuffer {
public:
    struct RateUpdate {
        std::string from, to;
        double rate;
    };

private:
    Bank *owner;
    int BufferSize;
    std::vector<RateUpdate> buffer;
    int local_head, local_tail, cached_head, cached_tail;
    std::atomic<int> head{0};
    std::atomic<int> tail{0};

public:
    ExchangeBuffer(Bank *bank, int BufferSize);
    void ResizeBuffer();
    bool InsertProducer(const RateUpdate &update);
    bool PopConsumer(Bank *bank);
};

#endif //PROJECT_OOP_EXCHANGEBUFFER_H