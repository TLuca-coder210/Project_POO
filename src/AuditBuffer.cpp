//
// Created by Luca Teodorescu on 01.04.2026.
//

#include "AuditBuffer.h"
#include "Bank.h"

using namespace std;

AuditBuffer::AuditBuffer(Bank *bank, int BufferSize) {
    this->owner = bank;
    this->AuditBufferSize = BufferSize + 1;
    this->buffer.resize(this->AuditBufferSize);
    this->local_head = 0;
    this->local_tail = 0;
    this->cached_head = 0;
    this->cached_tail = 0;
    this->head.store(0, std::memory_order_relaxed);
    this->tail.store(0, std::memory_order_relaxed);
}

void AuditBuffer::ResizeBuffer() {
    int AuxiliaryBufferSize = this->AuditBufferSize;
    this->AuditBufferSize *= 2;
    vector<USER> AuxiliaryBuffer(this->AuditBufferSize);

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

bool AuditBuffer::InsertProducer(const USER &update) {
    int next_head = (this->local_head + 1) % this->AuditBufferSize;

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

bool AuditBuffer::PopConsumer(Bank *bank) {
    if (this->local_tail == this->cached_head) {
        this->cached_head = head.load(std::memory_order_acquire);
        if (this->local_tail == this->cached_head) {
            return false;
        }
    }

    USER update = buffer[this->local_tail];
    bool ok = bank->VerifyTransaction(update);
    bank->AddTransferVerification(update, ok);

    int next_tail = (this->local_tail + 1) % this->AuditBufferSize;
    this->local_tail = next_tail;
    tail.store(next_tail, std::memory_order_release);
    return true;
}
