//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "Bank.h"
#include "Account.h"
#include "CurrentAccount.h"
#include "SavingsAccount.h"
#include "ExchangeBuffer.h"
#include <iostream>
#include <set>
#include <algorithm>

void Bank::BackgroundFetcher() {
    vector<pair<string, string>> currencies = {
        {"EUR", "RON"}, {"USD", "RON"}, {"GBP", "RON"}, {"CHF", "RON"}
    };
    while (running.load(memory_order_acquire)) {
        for (auto currency : currencies) {
            ExchangeBuffer::RateUpdate update;
            update.from = currency.first;
            update.to = currency.second;
            double random_number = double(rand()) / double(RAND_MAX);
            update.rate = random_number;
            if (!exchangeBuffer.InsertProducer(update)) {
                break;
            }
        }
        this_thread::sleep_for(chrono::seconds(5));
    }
}

void Bank::RefreshRates() {
    while (exchangeBuffer.PopConsumer(this)) {

    }
}

void Bank::BackgroundAuditConsumer() {
    while (running.load(std::memory_order_acquire)) {
        if (!auditBuffer.PopConsumer(this)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

Bank::Bank() : exchangeBuffer(this, 1024), auditBuffer(this, 1024) {
    running.store(true, std::memory_order_release);
    rateUpdateThread = thread(&Bank::BackgroundFetcher, this);
    auditThread = std::thread(&Bank::BackgroundAuditConsumer, this);
}

Bank::~Bank() {
    running.store(false, std::memory_order_release);
    if (rateUpdateThread.joinable()) {
        rateUpdateThread.join();
    }
    if (auditThread.joinable()) {
        auditThread.join();
    }
    for (auto it = AllUsers.begin(); it != AllUsers.end(); it++) {
        delete it->second;
    }
    AllUsers.clear();
    GraphUsers.clear();
}

void Bank::CreateUser(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate) {
    if (AllUsers.find(CNP) != AllUsers.end()) {
        cout << "Eroare: Utilizatorul cu CNP-ul " << CNP << " exista deja!" << '\n';
        return;
    }
    User* new_client = new User(FirstName, LastName, CNP, BirthDate);
    AllUsers[CNP] = new_client;
}

void Bank::CreateUser(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &currency) {
    if (AllUsers.find(CNP) != AllUsers.end()) {
        cout << "Eroare: Utilizatorul cu CNP-ul " << CNP << " exista deja!" << '\n';
        return;
    }
    User* new_client = new User(FirstName, LastName, CNP, BirthDate, currency);
    AllUsers[CNP] = new_client;
}

void Bank::DeleteUser(const string &CNP) {
    auto it = AllUsers.find(CNP);
    if (it != AllUsers.end()) {
        User *client = it->second;
        if (GraphUsers.contains(CNP)) {
            for (User* prieten : GraphUsers[CNP]) {
                string prieten_cnp = prieten->GetCNP();
                GraphUsers[prieten_cnp].erase(client);
                prieten->EraseFriend(CNP);
            }
            GraphUsers.erase(CNP);
        }
        delete client;
        AllUsers.erase(it);
        cout << "Utilizatorul cu CNP-ul " << CNP << " a fost sters cu succes." << '\n';
    }
    else {
        cout << "Invalid CNP" << '\n';
    }
}

void Bank::CreateAccount(const string &CNP, const string &currency) {
    if (AllUsers.find(CNP) != AllUsers.end()) {
        User* client = AllUsers[CNP];
        client->AddAccount(currency);
    }
    else {
        cout << "Invalid CNP" << '\n';
    }
}

void Bank::DeleteAccount(const string &CNP, const string &IBAN) {
    auto it = AllUsers.find(CNP);
    if (it != AllUsers.end()) {
        User *client = it->second;
        client->DeleteAccount(IBAN);
        cout << "Solicitarea de stergere pentru contul " << IBAN << " a fost trimisa catre client." << '\n';
    }
    else {
        cout << "Eroare: Invalid CNP. Utilizatorul nu a fost gasit!" << '\n';
    }
}

void Bank::CreateWallet(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &IBAN) {
    if (AllUsers.find(CNP) != AllUsers.end()) {
        User* client = AllUsers[CNP];
        client->AddWallet(IBAN);
        cout << "Portofelul a fost adaugat cu succes in contul " << IBAN << '\n';
    }
    else {
        User* client = new User(FirstName, LastName, CNP, BirthDate);
        AllUsers[CNP] = client;
        cout << "Clientul nu exista. I s-a creat un cont" << '\n';
    }
}

void Bank::DeleteWallet(const string &CNP, const string &IBAN, const string &WalletID) {
    if (AllUsers.find(CNP) != AllUsers.end()) {
        User *client = AllUsers[CNP];
        client->DeleteWallet(IBAN, WalletID);
    }
    else {
        cout << "Invalid CNP" << '\n';
    }
}

bool Bank::Deposit(const string &CNP, const string &IBAN, const string &WalletID, const double &amount, const string &currency) {
    RefreshRates();
    if (AllUsers.find(CNP) != AllUsers.end()) {
        User *client = AllUsers[CNP];
        Account *account = client->GetAccount(IBAN);
        if (account == nullptr) {
            cout << "IBAN invalid" << '\n';
            return false;
        }
        account->Deposit(amount);
        cout << "Depunere reusita în contul " << IBAN << '\n';
        return true;
    }
    else {
        cout << "CNP invalid" << '\n';
        return false;
    }
}

bool Bank::Withdraw(const string &CNP, const string &IBAN, const string &WalletID, const double &amount, const string &currency) {
    RefreshRates();
    if (AllUsers.find(CNP) != AllUsers.end()) {
        User *client = AllUsers[CNP];
        Account *account = client->GetAccount(IBAN);
        if (account == nullptr) {
            cout << "IBAN invalid" << '\n';
            return false;
        }
        if (account->Withdraw(amount)) {
            cout << "Retragere reusita din contul " << IBAN << '\n';
            return true;
        }
        return false;
    }
    else {
        cout << "CNP invalid" << '\n';
        return false;
    }
}

void Bank::ChangeExchangeRates(const string &first_currency, const string &second_currency, const double &amount) {
    ExchangeRates[{first_currency, second_currency}] = amount;
    ExchangeRates[{second_currency, first_currency}] = 1.0 / amount;
}

double Bank::GetExchangeRates(const string &first_currency, const string &second_currency) {
    if (first_currency == second_currency) {
        return 1.0;
    }
    auto it = ExchangeRates.find({first_currency, second_currency});
    if (it != ExchangeRates.end()) {
        return it->second;
    }
    else {
        cout << "Eroare. Nu exista curs valutar intre " << first_currency << " si " << second_currency << "." << '\n';
        return 0.0;
    }
}

void Bank::Transfer(const string &Send_CNP, const string &Receive_CNP, const string &Send_IBAN, const string &Receive_IBAN, const string &Send_WalletID, const string &Receive_WalletID, const double &amount, const string &currency) {
    RefreshRates();
    try {
        AuditBuffer::USER tx_check;
        tx_check.SenderCNP = Send_CNP;
        tx_check.ReceiverCNP = Receive_CNP;
        tx_check.amount = amount;
        this->VerifyTransaction(tx_check);
        bool withdraw_ok = this->Withdraw(Send_CNP, Send_IBAN, Send_WalletID, amount, currency);
        if (withdraw_ok) {
            bool deposit_ok = this->Deposit(Receive_CNP, Receive_IBAN, Receive_WalletID, amount, currency);
            if (deposit_ok) {
                std::cout << "Tranzactia a fost acceptata" << '\n';
                if (!auditBuffer.InsertProducer(tx_check)) {
                    std::cout << "[Audit System] Buffer plin..." << '\n';
                }
            } else {
                std::cout << "Eroare la depunere" << '\n';
                this->Deposit(Send_CNP, Send_IBAN, Send_WalletID, amount, currency);
            }
        } else {
            std::cout << "Transfer anulat: Tranzactia de retragere a esuat" << '\n';
        }
    }
    catch (const std::exception &e) {
        std::cout << "\n[SISTEM SIGURANTA BANCA] S-a interceptat o problema critica in timpul transferului!" << std::endl;
        std::cout << "Detalii eroare: " << e.what() << std::endl;
        std::cout << "[SISTEM SIGURANTA BANCA] Operatiunea de transfer a fost BLOCATA complet pentru siguranta.\n" << std::endl;
    }
}

void Bank::SendFriendInvitation(const string &Send_CNP, const string &Receive_CNP) {
    if (Send_CNP != Receive_CNP) {
        if (AllUsers.find(Send_CNP) != AllUsers.end() && AllUsers.find(Receive_CNP) != AllUsers.end()) {
            User *client = AllUsers[Receive_CNP];
            client->SendFriendInvitation(Send_CNP);
        }
    }
    else {
        cout << "Invalid Send Invitation" << '\n';
    }
}

void Bank::AcceptFriendInvitation(const string &Send_CNP, const string &Receive_CNP) {
    if (Send_CNP != Receive_CNP) {
        if (AllUsers.find(Send_CNP) != AllUsers.end() && AllUsers.find(Receive_CNP) != AllUsers.end()) {
            User *first_client = AllUsers[Receive_CNP];
            if (first_client->AcceptFriendInvitation(Send_CNP)) {
                User *second_client = AllUsers[Receive_CNP];
                first_client->AddFriend(Receive_CNP);
                second_client->AddFriend(Receive_CNP);
                GraphUsers[Send_CNP].insert(first_client);
                User *SendClient = AllUsers[Send_CNP];
                GraphUsers[Receive_CNP].insert(SendClient);
            }
        }
    }
    else {
        cout << "Invalid Accept Invitation" << '\n';
    }
}

void Bank::DeleteFriend(const string &Send_CNP, const string &Receive_CNP) {
    if (Send_CNP != Receive_CNP) {
        if (AllUsers.find(Send_CNP) != AllUsers.end() && AllUsers.find(Receive_CNP) != AllUsers.end()) {
            User *first_client = AllUsers[Send_CNP];
            if (first_client->CheckFriend(Receive_CNP)) {
                User *second_client = AllUsers[Receive_CNP];
                first_client->EraseFriend(Receive_CNP);
                second_client->EraseFriend(Send_CNP);
                GraphUsers[Send_CNP].erase(second_client);
                GraphUsers[Receive_CNP].erase(first_client);
            }
        }
    }
    else {
        cout << "Invalid User" << '\n';
    }
}

bool Bank::DFSCycleCheck(const string &current, const string &target, double amount, set<string> &visited) {
    if (current == target) {
        return true;
    }
    visited.insert(current);
    for (const auto &edge : this->TransactionGraph[current]) {
        if (edge.amount == amount) {
            if (visited.find(edge.receiverCNP) == visited.end()) {
                if (DFSCycleCheck(edge.receiverCNP, target, amount, visited)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Bank::AddTransferVerification(const AuditBuffer::USER &userAudit, const bool &transfer) {
    if (transfer) {
        std::lock_guard<std::mutex> lock(this->graphMutex);
        AuditEdge edge;
        edge.receiverCNP = userAudit.ReceiverCNP;
        edge.amount = userAudit.amount;
        this->TransactionGraph[userAudit.SenderCNP].push_back(edge);
        std::cout << "S-a adaugat muchia: "
                  << userAudit.SenderCNP << " -> " << userAudit.ReceiverCNP
                  << " cu suma: " << userAudit.amount << '\n';
    }
}

bool Bank::VerifyTransaction(const AuditBuffer::USER &userAudit) {
    std::lock_guard<std::mutex> lock(this->graphMutex);
    std::cout << "Se analizeaza potentialul fraudulos pentru: "
              << userAudit.SenderCNP << " -> " << userAudit.ReceiverCNP << " (" << userAudit.amount << ")" << '\n';
    std::set<std::string> visited;
    if (DFSCycleCheck(userAudit.ReceiverCNP, userAudit.SenderCNP, userAudit.amount, visited)) {
        throw FraudDetectedException("Tranzactie circulara suspecta detectata intre CNP: "
                                     + userAudit.SenderCNP + " si CNP: " + userAudit.ReceiverCNP
                                     + " pentru suma de " + std::to_string(userAudit.amount) + " RON!");
    }
    std::cout << "Nu s-a detectat niciun pattern circular pentru aceasta suma." << '\n';
    return true;
}

void Bank::DistributeMonthlyInterest() {
    for (const auto& pair : AllUsers) {
        User* user = pair.second;
        for (const auto& accPair : user->GetAllAccounts()) {
            Account* acc = accPair.second;
            SavingsAccount* savings = dynamic_cast<SavingsAccount*>(acc);
            if (savings != nullptr) {
                savings->AddInterest();
            }
        }
    }
}

void Bank::AuditOverdraftBalances() {
    std::cout << "Conturi in overdraft:" << std::endl;
    for (const auto& pair : AllUsers) {
        User* user = pair.second;
        for (const auto& accPair : user->GetAllAccounts()) {
            Account* acc = accPair.second;
            CurrentAccount* current = dynamic_cast<CurrentAccount*>(acc);
            if (current != nullptr && current->GetBalance() < 0) {
                std::cout << "User CNP: " << pair.first
                          << " | Sold: " << current->GetBalance()
                          << " | Overdraft: " << current->GetOverdraftLimit() << std::endl;
            }
        }
    }
}
