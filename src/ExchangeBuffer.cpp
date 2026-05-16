//
// Created by Luca Teodorescu on 27.03.2026.
//

#include "ExchangeBuffer.h"
#include "Bank.h"
#include <iostream>
#include <vector>
#include <atomic>

using namespace std;

ExchangeBuffer::ExchangeBuffer(Bank *bank, int BufferSize) {
    this->owner = bank;
    this->BufferSize = BufferSize + 1;
    this->buffer.resize(this->BufferSize);
    this->local_head = 0;
    this->local_tail = 0;
    this->cached_head = 0;
    this->cached_tail = 0;
    this->head.store(0, std::memory_order_relaxed);
    this->tail.store(0, std::memory_order_relaxed);
}

void ExchangeBuffer::ResizeBuffer() {
    int AuxiliaryBufferSize = this->BufferSize;
    this->BufferSize *= 2;
    vector<RateUpdate> AuxiliaryBuffer(this->BufferSize);

    int current_head = head.load(std::memory_order_acquire);
    int current_tail = tail.load(std::memory_order_acquire);
    int counter = 0;

    if (current_head > current_tail) {
        for (int i = current_tail; i < current_head; i++) {
            AuxiliaryBuffer[counter++] = buffer[i];
        }
    }
    else if (current_head < current_tail) {
        for (int i = current_tail; i < AuxiliaryBufferSize; i++) {
            AuxiliaryBuffer[counter++] = buffer[i];
        }
        for (int i = 0; i < current_head; i++) {
            AuxiliaryBuffer[counter++] = buffer[i];
        }
    }

    this->buffer = std::move(AuxiliaryBuffer);

    this->local_head = counter;
    this->local_tail = 0;
    this->cached_head = counter;
    this->cached_tail = 0;

    head.store(counter, std::memory_order_release);
    tail.store(0, std::memory_order_release);
}

bool ExchangeBuffer::InsertProducer(const RateUpdate &update) {
    int next_head = (this->local_head + 1) % this->BufferSize;

    if (next_head == this->cached_tail) {
        this->cached_tail = tail.load(std::memory_order_acquire);
        if (next_head == this->cached_tail) {
            return false;
        }
    }

    buffer[this->local_head] = update;
    this->local_head = next_head;
    head.store(next_head, std::memory_order_release);
    return true;
}

bool ExchangeBuffer::PopConsumer(Bank *bank) {
    if (this->local_tail == this->cached_head) {
        this->cached_head = head.load(std::memory_order_acquire);
        if (this->local_tail == this->cached_head) {
            return false;
        }
    }

    bank->ChangeExchangeRates(buffer[this->local_tail].from, buffer[this->local_tail].to, buffer[this->local_tail].rate);

    int next_tail = (this->local_tail + 1) % this->BufferSize;
    this->local_tail = next_tail;
    tail.store(next_tail, std::memory_order_release);
    return true;
}
