//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "User.h"
#include "Account.h"
#include "CurrentAccount.h"
#include "SavingsAccount.h"
#include "AccountFactory.h"
#include <iostream>

User::User(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate) {
    this->FirstName = FirstName;
    this->LastName = LastName;
    this->CNP = CNP;
    this->BirthDate = BirthDate;

    string initialIban = "RO98BANC" + CNP.substr(0, 4) + "RON";
    Account *new_account = AccountFactory::CreateAccount("RON", initialIban, 0.0);
    string IBAN = new_account->GetIBAN();

    AllAccounts[IBAN] = new_account;
}

User::User(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &currency) {
    this->FirstName = FirstName;
    this->LastName = LastName;
    this->CNP = CNP;
    this->BirthDate = BirthDate;

    string initialIban = "RO98BANC" + CNP.substr(0, 4) + currency;
    Account *new_account = AccountFactory::CreateAccount(currency, initialIban, 0.0);
    string IBAN = new_account->GetIBAN();
    AllAccounts[IBAN] = new_account;
}

string User::GetBirthDate() {
    return this->BirthDate;
}

string User::GetFirstName() {
    return this->FirstName;
}

string User::GetLastName() {
    return this->LastName;
}

string User::GetCNP() {
    return this->CNP;
}

void User::AddAccount(const string &currency) {
    string newIban = "RO98BANC" + this->CNP.substr(0, 4) + currency + to_string(rand() % 100);
    Account *new_account = AccountFactory::CreateAccount(currency, newIban, 0.0);
    AllAccounts[newIban] = new_account;
}

void User::DeleteAccount() {
    for (const auto &it : AllAccounts) {
        delete it.second;
    }
    AllAccounts.clear();
}

void User::DeleteAccount(const string &IBAN) {
    auto it = AllAccounts.find(IBAN);
    if (it != AllAccounts.end()) {
        delete it->second;
        AllAccounts.erase(it);
        cout << "Contul cu IBAN-ul " << IBAN << " a fost sters cu succes din sistem.\n";
    }
    else {
        cout << "Eroare: Acest IBAN nu apartine utilizatorului curent!\n";
    }
}

void User::AddWallet(const string &IBAN) {
    auto it = AllAccounts.find(IBAN);
    if (it != AllAccounts.end()) {
        it->second->Deposit(0.0);
    }
}

void User::DeleteWallet(const string &IBAN, const string &) {
    auto it = AllAccounts.find(IBAN);
    if (it != AllAccounts.end()) {
        it->second->Withdraw(0.0);
    }
}

Account *User::GetAccount(const string &IBAN) {
    if (AllAccounts.find(IBAN) != AllAccounts.end()) {
        return AllAccounts[IBAN];
    }
    return nullptr;
}

map<string, Account*> User::GetAllAccounts() const {
    return AllAccounts;
}

void User::SendFriendInvitation(const string &CNP_) {
    if (SentFriendInvitations.find(CNP_) == SentFriendInvitations.end()) {
        SentFriendInvitations[CNP_] = true;
    }
    else {
        cout << "Invitation already exists" << endl;
    }
}

bool User::AcceptFriendInvitation(const string &CNP_) {
    if (AcceptedFriendInvitations.find(CNP_) == AcceptedFriendInvitations.end()) {
        AcceptedFriendInvitations[CNP_] = true;
        return true;
    }
    else {
        cout << "Invitation already accepted" << '\n';
        return false;
    }
}

void User::AddFriend(const string &CNP_) {
    Friends[CNP_] = true;
}

void User::EraseFriend(const string &CNP_) {
    Friends.erase(CNP_);
}

bool User::CheckFriend(const string &CNP_) {
    if (Friends.find(CNP_) != Friends.end()) {
        return true;
    }
    return false;
}

User::~User() {
    for (const auto &it : AllAccounts) {
        delete it.second;
    }
    AllAccounts.clear();
}
