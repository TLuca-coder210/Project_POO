//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "Wallet.h"
#include "Account.h"
#include "Bank.h"
#include <iostream>
#include <random>
#include <string>

Wallet::Wallet(Account *owner) {
    this->owner = owner;
    this->currency = "RON";
    this->balance = 0.0;
    int random_number = rand() % 900000 + 100000;
    string owner_iban = owner->GetIBAN();
    string iban_suffix = owner_iban.substr(owner_iban.length() - 4);
    this->WalletID = "W_" + iban_suffix + "_" + to_string(random_number);
}

Wallet::Wallet(Account *owner, const string &currency) {
    this->owner = owner;
    this->currency = currency;
    this->balance = 0.0;
    int random_number = rand() % 900000 + 100000;
    string owner_iban = owner->GetIBAN();
    string iban_suffix = owner_iban.substr(owner_iban.length() - 4);
    this->WalletID = "W_" + iban_suffix + "_" + to_string(random_number);
}

string Wallet::GetWalletID() {
    return this->WalletID;
}

string Wallet::GetCurrency() {
    return this->currency;
}

double Wallet::GetBalance() {
    return this->balance;
}

void Wallet::Deposit(const double &amount) {
    this->balance += amount;
}

void Wallet::Withdraw(const double &amount) {
    if (this->balance < amount) {
        cout << "Insufficient balance" << '\n';
    }
    else {
        this->balance -= amount;
    }
}
