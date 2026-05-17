//
// Created by Luca Teodorescu on 20.03.2026.
//

#ifndef PROJECT_OOP_WALLET_H
#define PROJECT_OOP_WALLET_H

#include "Account.h"
#include "Bank.h"
#include <iostream>
#include <string>
using namespace std;

class Wallet {
    Account *owner;
    string currency, WalletID;
    double balance;
    static map<std::string, double> GlobalTransactionVolume;
public:
    Wallet();
    ~Wallet();
    explicit Wallet(Account *owner);
    Wallet(Account *owner, const string &currency);
    Wallet(const Wallet &other);
    Wallet& operator=(const Wallet &other);
    string GetWalletID() const;
    string GetCurrency() const;
    double GetBalance() const;
    void Deposit(const double &amount);
    void Withdraw(const double &amount);
    static void UpdateVolume(const std::string& curr, double amount);
    static void PrintGlobalVolume();
    bool operator==(const Wallet &other) const;
    friend std::ostream& operator<<(std::ostream &os, const Wallet &wallet);
    friend std::istream& operator>>(std::istream &is, Wallet &wallet);
};

Wallet operator+(const Wallet &w1, const Wallet &w2);

#endif //PROJECT_OOP_WALLET_H