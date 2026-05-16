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
public:
    Wallet(Account *owner);
    Wallet(Account *owner, const string &currency);
    string GetWalletID();
    string GetCurrency();
    double GetBalance();
    void Deposit(const double &amount);
    void Withdraw(const double &amount);
};

#endif //PROJECT_OOP_WALLET_H