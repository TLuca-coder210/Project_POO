//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "Account.h"
#include "Wallet.h"
#include <iostream>
#include <string>
#include <random>

Account::Account(User *account_owner) {
    this->owner = account_owner;
    string birth_date = account_owner->GetBirthDate();
    int random_number = rand() % 900000 + 100000;
    this->IBAN = "RO" + birth_date + "BANK" + to_string(random_number);
    Wallet* default_wallet = new Wallet(this);
    string WalletID = default_wallet->GetWalletID();
    if (!AllWallets.contains(WalletID)) {
        AllWallets[WalletID] = default_wallet;
    }
    else {
        delete default_wallet;
        cout << "Eroare: Portofel cu ID duplicat!" << '\n';
    }
}

Account::Account(User *account_owner, const string &currency) {
    this->owner = account_owner;
    string birth_date = account_owner->GetBirthDate();
    int random_number = rand() % 900000 + 100000;
    this->IBAN = "RO" + birth_date + "BANK" + to_string(random_number);
    Wallet* default_wallet = new Wallet(this, currency);
    string WalletID = default_wallet->GetWalletID();
    if (!AllWallets.contains(WalletID)) {
        AllWallets[WalletID] = default_wallet;
    }
    else {
        delete default_wallet;
        cout << "Eroare: Portofel cu ID duplicat!" << '\n';
    }
}

string Account::GetIBAN() {
    return this->IBAN;
}

void Account::addWallet() {
    Wallet *new_wallet = new Wallet(this);
    string WalletID = new_wallet->GetWalletID();
    if (AllWallets.find(WalletID) != AllWallets.end()) {
        AllWallets[WalletID] = new_wallet;
    }
}

void Account::DeleteWallet() {
    AllWallets.clear();
}

void Account::DeleteWallet(const string &WalletID) {
    auto it = AllWallets.find(WalletID);
    if (it != AllWallets.end()) {
        delete it->second;
        AllWallets.erase(it);
        cout << "Portofelul cu ID-ul " << WalletID << " a fost sters cu succes.\n";
    }
    else {
        cout << "Eroare: Portofelul cu ID-ul " << WalletID << " nu a fost gasit in acest cont!\n";
    }
}

Wallet *Account::GetWallet(const string &WalletID) {
    if (AllWallets.find(WalletID) != AllWallets.end()) {
        return AllWallets[WalletID];
    }
    return nullptr;
}

Account::~Account() {
    for (auto &it : AllWallets) {
        delete it.second;
    }
    AllWallets.clear();
}
