//
// Created by Luca Teodorescu on 01.04.2026.
//

#ifndef PROJECT_OOP_AUDITBUFFER_H
#define PROJECT_OOP_AUDITBUFFER_H

#include <iostream>
#include <vector>
#include <atomic>
#include <string>

class Bank;

class AuditBuffer {
public:
    struct USER {
        std::string SenderCNP, SenderIBAN, SenderWalletID;
        std::string ReceiverCNP, ReceiverIBAN, ReceiverWalletID;
        double amount;
        bool operator<(const USER &other) const {
            if (amount != other.amount) {
                return amount > other.amount;
            }
            if (SenderCNP != other.SenderCNP) {
                return SenderCNP > other.SenderCNP;
            }
            if (ReceiverCNP != other.ReceiverCNP) {
                return ReceiverCNP > other.ReceiverCNP;
            }
            if (SenderIBAN != other.SenderIBAN) {
                return SenderIBAN > other.SenderIBAN;
            }
            if (ReceiverIBAN != other.ReceiverIBAN) {
                return ReceiverIBAN > other.ReceiverIBAN;
            }
            if (SenderWalletID != other.SenderWalletID) {
                return SenderWalletID > other.SenderWalletID;
            }
            return ReceiverWalletID > other.ReceiverWalletID;
        }
    };

private:
    Bank *owner;
    int AuditBufferSize;
    std::vector<USER> buffer;
    int local_head, local_tail, cached_head, cached_tail;
    std::atomic<int> head{0};
    std::atomic<int> tail{0};

public:
    AuditBuffer(Bank *bank, int BufferSize);
    void ResizeBuffer();
    bool InsertProducer(const USER &update);
    bool PopConsumer(Bank *bank);
};

#endif //PROJECT_OOP_AUDITBUFFER_H