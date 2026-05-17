//
// Created by Luca Teodorescu on 20.03.2026.
//

#ifndef PROJECT_OOP_BANK_H
#define PROJECT_OOP_BANK_H

#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_set>
#include <mutex>
#include <set>
#include "Account.h"
#include "ExchangeBuffer.h"
#include "AuditBuffer.h"
#include "User.h"
using namespace std;

struct Currency {
    string first_currency, second_currency;
    bool operator<(const Currency &other) const{
        if (first_currency != other.first_currency) {
            return first_currency < other.first_currency;
        }
        return second_currency < other.second_currency;
    }
};

class Bank {
    struct AuditEdge {
        std::string receiverCNP;
        double amount;
    };
    map<string, User*> AllUsers;
    map<Currency, double> ExchangeRates;
    map<string, unordered_set<User*>> GraphUsers;
    map<AuditBuffer::USER, bool> TransferVerification;

    ExchangeBuffer exchangeBuffer;
    AuditBuffer auditBuffer;
    thread rateUpdateThread;
    std::thread auditThread;
    std::map<std::string, std::vector<AuditEdge>> TransactionGraph;
    std::mutex graphMutex;
    void BackgroundAuditConsumer();
    atomic<bool> running{true};

    void BackgroundFetcher();
public:
    Bank();
    ~Bank();
    void RefreshRates();
    void CreateUser(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate);
    void CreateUser(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &currency);
    void DeleteUser(const string &CNP);
    void CreateAccount(const string &CNP, const string &currency);
    void DeleteAccount(const string &CNP, const string &IBAN);
    void CreateWallet(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &IBAN);
    void DeleteWallet(const string &CNP, const string &IBAN, const string &WalletID);
    bool Deposit(const string &CNP, const string &IBAN, const string &WalletID, const double &amount, const string &currency);
    bool Withdraw(const string &CNP, const string &IBAN, const string &WalletID, const double &amount, const string &currency);
    void ChangeExchangeRates(const string &first_currency, const string &second_currency, const double &amount);
    double GetExchangeRates(const string &first_currency, const string &second_currency);
    void Transfer(const string &Send_CNP, const string &Receive_CNP, const string &Send_IBAN, const string &Receive_IBAN, const string &Send_WalletID, const string &Receive_WalletID, const double &amount, const string &currency);
    void SendFriendInvitation(const string &Send_CNP, const string &Receive_CNP);
    void AcceptFriendInvitation(const string &Send_CNP, const string &Receive_CNP);
    void DeleteFriend(const string &Send_CNP, const string &Receive_CNP);
    bool VerifyTransaction(const AuditBuffer::USER &userAudit);
    void AddTransferVerification(const AuditBuffer::USER &userAudit, const bool &transfer);
    bool DFSCycleCheck(const std::string &current, const std::string &target, double amount, set<string> &visited);

    void DistributeMonthlyInterest();
    void AuditOverdraftBalances();
};

#endif //PROJECT_OOP_BANK_H