//
// Created by Luca Teodorescu on 20.03.2026.
//

#ifndef PROJECT_OOP_ACCOUNT_H
#define PROJECT_OOP_ACCOUNT_H

#include <iostream>
#include <vector>
#include <string>
#include "User.h"
#include "Wallet.h"
using namespace std;

class Wallet;

class Account {
    map<string, Wallet*> AllWallets;
    User* owner;
    string IBAN;
public:
    Account(User* owner);
    Account(User* owner, const string &currency);
    ~Account();
    void addWallet();
    void DeleteWallet();
    void DeleteWallet(const string &WalletID);
    string GetIBAN();
    Wallet *GetWallet(const string &WalletID);
};

#endif //PROJECT_OOP_ACCOUNT_H