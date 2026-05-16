//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "Bank.h"
#include "Account.h"
#include "ExchangeBuffer.h"
#include <iostream>
#include <set>

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
        Wallet *wallet = account->GetWallet(WalletID);
        if (wallet == nullptr) {
            cout << "WalletID invalid" << '\n';
            return false;
        }
        string wallet_currency = wallet->GetCurrency();
        double ExchangeRate = this->GetExchangeRates(currency, wallet_currency);
        wallet->Deposit(amount * ExchangeRate);
        cout << "Depunere reusita! S-au adaugat " << amount * ExchangeRate << " " << wallet_currency << " in cont." << '\n';
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
        Wallet *wallet = account->GetWallet(WalletID);
        if (wallet == nullptr) {
            cout << "WalletID invalid" << '\n';
            return false;
        }
        string wallet_currency = wallet->GetCurrency();
        double ExchangeRate = this->GetExchangeRates(currency, wallet_currency);
        if (wallet->GetBalance() < ExchangeRate * amount) {
            cout << "Fonduri insuficiente" << '\n';
            return false;
        }
        wallet->Withdraw(amount * ExchangeRate);
        cout << "Retragere reusita! S-au extras " << amount * ExchangeRate << " " << wallet_currency << " in cont." << '\n';
        return true;
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
        cout << "Eroare. Nu exista curs valutar intre " << first_currency << "si " << second_currency << "." << '\n';
        return 0.0;
    }
}

void Bank::Transfer(const string &Send_CNP, const string &Receive_CNP, const string &Send_IBAN, const string &Receive_IBAN, const string &Send_WalletID, const string &Receive_WalletID, const double &amount, const string &currency) {
    RefreshRates();
    bool withdraw = this->Withdraw(Send_CNP, Send_IBAN, Send_WalletID, amount, currency);
    if (withdraw) {
        bool deposit = this->Deposit(Receive_CNP, Receive_IBAN, Receive_WalletID, amount, currency);
        if (deposit) {
            cout << "Tranzactia a fost acceptata" << '\n';
            AuditBuffer::USER tx;
            tx.SenderCNP = Send_CNP;
            tx.SenderIBAN = Send_IBAN;
            tx.SenderWalletID = Send_WalletID;
            tx.ReceiverCNP = Receive_CNP;
            tx.ReceiverIBAN = Receive_IBAN;
            tx.ReceiverWalletID = Receive_WalletID;
            tx.amount = amount;
            if (!auditBuffer.InsertProducer(tx)) {
                cout << "[Audit System] Buffer plin, se auto-redimensioneaza in fundal..." << '\n';
            }
        }
        else {
            cout << "Eroare la depunere" << '\n';
            this->Deposit(Send_CNP, Send_IBAN, Send_WalletID, amount, currency);
        }
    }
    else {
        cout << "Transfer anulat: Tranzactia de retragere a esuat" << '\n';
    }
}

void Bank::SendFriendInvitation(const string &Send_CNP, const string &Receive_CNP) {
    if (Send_CNP != Receive_CNP) {
        if (AllUsers.find(Send_CNP) != AllUsers.end() and AllUsers.find(Receive_CNP) != AllUsers.end()) {
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
        if (AllUsers.find(Send_CNP) != AllUsers.end() and AllUsers.find(Receive_CNP) != AllUsers.end()) {
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
        if (AllUsers.find(Send_CNP) != AllUsers.end() and AllUsers.find(Receive_CNP) != AllUsers.end()) {
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
        std::cout << "S-a detectat un pattern circular de tranzactionare! "
                  << "Suma de " << userAudit.amount << " RON se invarte in cerc." << '\n';
        return false;
    }
    std::cout << "Nu s-a detectat niciun pattern circular pentru aceasta suma." << '\n';
    return true;
}
