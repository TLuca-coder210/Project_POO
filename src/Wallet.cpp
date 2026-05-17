//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "Wallet.h"
#include "Account.h"
#include "Bank.h"
#include <iostream>
#include <random>
#include <string>

Wallet::Wallet() {
    this->owner = nullptr;
    this->currency = "RON";
    this->WalletID = "UNKNOWN";
    this->balance = 0.0;
}

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

Wallet::Wallet(const Wallet &other) {
    this->owner = other.owner;
    this->currency = other.currency;
    this->WalletID = other.WalletID + "_COPIED";
    this->balance = other.balance;
}

Wallet& Wallet::operator=(const Wallet &other) {
    if (this != &other) {
        this->owner = other.owner;
        this->currency = other.currency;
        this->WalletID = other.WalletID;
        this->balance = other.balance;
    }
    return *this;
}

Wallet::~Wallet() {

}

string Wallet::GetWalletID() const {
    return this->WalletID;
}

string Wallet::GetCurrency() const {
    return this->currency;
}

double Wallet::GetBalance() const {
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

bool Wallet::operator==(const Wallet &other) const {
    if (this->WalletID == other.WalletID) {
        return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream &os, const Wallet &wallet) {
    os << "Wallet ID " << wallet.WalletID
       << " | Currency: " << wallet.currency
       << " | Current balance: " << wallet.balance << '\n';
    return os;
}

std::istream& operator>>(std::istream &is, Wallet &wallet) {
    std::cout << "ID Portofel: ";
    is >> wallet.WalletID;
    std::cout << "Moneda (ex: EUR, USD): ";
    is >> wallet.currency;
    std::cout << "Soldul Inițial: ";
    is >> wallet.balance;
    wallet.owner = nullptr;
    return is;
}

Wallet operator+(const Wallet &w1, const Wallet &w2) {
    Wallet result;
    result.Deposit(w1.GetBalance() + w2.GetBalance());
    return result;
}