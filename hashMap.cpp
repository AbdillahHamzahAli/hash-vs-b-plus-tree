#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <ctype.h>
#include <chrono>
#include <string>
#include <iomanip>
#include <cmath>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct node {
    int id,
        stok,
        berat_gram;
    float harga;
    string nama_produk,
           kategori,
           deskripsi;
    bool tersedia;
};

struct Hash {
    int BUCKET;
    vector<list<node>> table;

    Hash(int b) {
        BUCKET = b;
        table.resize(BUCKET);
    }

    int hashFunction(int x) {
        return (x % BUCKET);
    }

    int isDuplicate(int x) {
        int index = hashFunction(x);

        for (auto& item : table[index]) if (x == item.id) return 1;
        return 0;
    }

    void createItem(node data) {
        int index = hashFunction(data.id);

        table[index].push_back(data);
        return;
    }

    void readItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;

        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key) {
                cout << "ID           : " << (*i).id << endl;
                cout << "Product Name : " << (*i).nama_produk << endl;
                cout << "Category     : " << (*i).kategori << endl;
                cout << "Price        : Rp" << fixed << setprecision(2) << (*i).harga << endl;
                cout << "Stock        : " << (*i).stok << endl;
                cout << "Weight       : " << (*i).berat_gram << " gr" << endl;
                cout << "Description  : " << (*i).deskripsi << endl;
                cout << "Available    : " << (((*i).tersedia) ? "Ya" : "Tidak") << endl << endl;
                return;
            }
        }
        cout << "Item not found!" << endl << endl;;
        return;
    }

    void updateItem(node data, int key) {
        deleteItem(key);
        createItem(data);
        return;
    }

    void deleteItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;

        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key) break;
        }
        if (i != table[index].end()) table[index].erase(i);
        else cout << "ID " << (*i).id << " doesn't exist." << endl;
        cout << endl;
        return;
    }
};
template<typename bench>

void chronoBench(bench start, bench end, int totalData, string method) {
    chrono::duration<double, milli> timeElapsed = end - start;
    double opsPerSec = (totalData * 1000.0) / timeElapsed.count();

    cout << "------------ChronoBench-----------" << endl;
    cout << "| Method     : " << setw(9) << method << setw(10) << "|" << endl;
    cout << "| Total ops  : " << setw(9) << totalData << setw(10) << "|" << endl;
    cout << "| Total time : " << setw(9) << timeElapsed.count() << " ms" << setw(7) << "|" << endl;
    cout << "| Throughput : " << setw(9) << static_cast<int>(floor(opsPerSec)) << " ops/sec" << setw(2) << "|" << endl;
    cout << "----------------------------------" << endl << endl;
    return;
}

void hashCreate(Hash* h, int totalData) {
    int id,
        stok,
        berat_gram;
    float harga;
    string nama_produk,
           kategori,
           deskripsi;
    bool tersedia;
    node add;

    cout << "Enter the ID (integer) : ";
    cin >> id;
    add.id = id;

    if (h->isDuplicate(add.id)) {
        char choice;

        cout << endl << "ID " << add.id << " already exists. Do you want to update it? (y/n)" << endl << "> ";
        cin >> choice;

        choice = tolower(choice);

        if (choice != 'y') {
            cout << endl << "Item not inserted." << endl << endl;
            return;
        }
    }
    cout << endl << "Product Name    : ";
    cin >> nama_produk;
    add.nama_produk = nama_produk;
    cout << "Category        : ";
    cin >> kategori;
    add.kategori = kategori;
    cout << "Price (decimal) : ";
    cin >> harga;
    add.harga = harga;
    cout << "Stock (integer) : ";
    cin >> stok;
    add.stok = stok;
    cout << "Weight (gram)   : ";
    cin >> berat_gram;
    add.berat_gram = berat_gram;
    cout << "Description     : ";
    cin >> deskripsi;
    add.deskripsi = deskripsi;
    cout << "Available (0/1) : ";
    cin >> tersedia;
    cout << endl;
    add.tersedia = tersedia;

    auto benchStart = chrono::high_resolution_clock::now();

    if (h->isDuplicate(id)) h->updateItem(add, h->hashFunction(add.id));
    else h->createItem(add);
    
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData, "hash-map");
    return;
}

void hashRead(Hash* h, int totalData) {
    char searchMode;
    int id,
        startId,
        endId;
    
    cout << "CHOOSE SEARCH MODE:" << endl
         << "(1) Single data" << endl
         << "(2) Data range" << endl
         << "(other) cancel" << endl
         << "> ";
    cin >> searchMode;
    cout << endl;

    if (searchMode == '1') {
        cout << "Enter the ID : ";
        cin >> id;
        cout << endl;

        auto benchStart = chrono::high_resolution_clock::now();
        h->readItem(id);
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, totalData, "hash-map");
    } else if (searchMode == '2') {
        cout << "Enter start ID : ";
        cin >> startId;
        cout << "Enter end ID   : ";
        cin >> endId;
        cout << endl;

        auto benchStart = chrono::high_resolution_clock::now();

        for (int temp_id = startId; temp_id <= endId; temp_id++) h->readItem(temp_id);

        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, totalData, "hash-map");
    }
    return;
}

void hashUpdate(Hash* h, int totalData) {
    int id,
        stok,
        berat_gram;
    float harga;
    string nama_produk,
           kategori,
           deskripsi;
    bool tersedia;
    node update;

    cout << "Enter the ID (integer) : ";
    cin >> id;
    update.id = id;
    cout << endl;

    if (h->isDuplicate(update.id)) {
        cout << "Product Name    : ";
        cin >> nama_produk;
        update.nama_produk = nama_produk;
        cout << "Category        : ";
        cin >> kategori;
        update.kategori = kategori;
        cout << "Price (decimal) : ";
        cin >> harga;
        update.harga = harga;
        cout << "Stock (integer) : ";
        cin >> stok;
        update.stok = stok;
        cout << "Weight (gram)   : ";
        cin >> berat_gram;
        update.berat_gram = berat_gram;
        cout << "Description     : ";
        cin >> deskripsi;
        update.deskripsi = deskripsi;
        cout << "Available (0/1) : ";
        cin >> tersedia;
        cout << endl;
        update.tersedia = tersedia;

        auto benchStart = chrono::high_resolution_clock::now();
        h->updateItem(update, h->hashFunction(update.id));
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, totalData, "hash-map");
        return;
    }
    cout << "ID " << update.id << " doesn't exist." << endl << endl;
    return;
}

void hashDelete(Hash* h, int totalData) {
    int id;
    char choice;

    cout << "Enter the ID (integer) : ";
    cin >> id;
    cout << endl << "===============" << endl << endl;

    h->readItem(id);

    cout << "===============" << endl << "Are you sure to delete all the data above? (y/n)" << endl << "> ";
    cin >> choice;
    cout << endl;

    choice = tolower(choice);

    if ((h->isDuplicate(id)) && (choice == 'y')) {
        auto benchStart = chrono::high_resolution_clock::now();
        h->deleteItem(id);
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, totalData, "hash-map");
        return;
    }
    cout << "Revoking the deletion operation." << endl << endl;
    return;
}

int main() {
    char chooseCase;
    Hash* h = nullptr;
    ifstream file;

    while (true) {
        cout << "CHOOSE THE SAMPLE:" << endl
             << "(a) 100 data" << endl
             << "(b) 500 data" << endl
             << "(c) 1000 data" << endl
             << "(other) exit" << endl
             << "> ";
        cin >> chooseCase;
        cout << endl;

        chooseCase = tolower(chooseCase);

        switch(chooseCase) {
            case 'a'    : { h = new Hash(101); file.open("produk_data-100.json"); break; }
            case 'b'    : { h = new Hash(503); file.open("produk_data-500.json"); break; }
            case 'c'    : { h = new Hash(1009); file.open("produk_data-1000.json"); break; }
            default     : { cout << "End of program." << endl; return 0; }
        }

        if (!file.is_open()) {
            cerr << "Failed to open JSON file!" << endl;
            return 1;
        }
        json data;

        try {
            file >> data;
        } catch (const exception& e) {
            cerr << "Error parsing JSON: " << e.what() << endl;
            return 1;
        }
        auto benchStart = chrono::high_resolution_clock::now();

        for(int i = 0; i < data.size(); i++) {
            try {
                node parse;
                parse.id = data[i]["id"];
                parse.nama_produk = data[i]["nama_produk"];
                parse.kategori = data[i]["kategori"];
                parse.harga = data[i]["harga"];
                parse.stok = data[i]["stok"];
                parse.berat_gram = data[i]["berat_gram"];
                parse.deskripsi = data[i]["deskripsi"];
                parse.tersedia = data[i]["tersedia"];
                h->createItem(parse);
            } catch (const std::exception& e) {
                std::cerr << "Error processing item " << i << ": " << e.what() << std::endl;
                continue;
            }
        }      
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, data.size(), "hash-map");

        do {
            cout << "WHICH CRUD OPERATION DO YOU WANT TO RUN?" << endl
                 << "(1) Create" << endl
                 << "(2) Read" << endl
                 << "(3) Update" << endl
                 << "(4) Delete" << endl
                 << "(other) cancel" << endl
                 << "> ";
            cin >> chooseCase;
            cout << endl;
            
            if (chooseCase == '1') hashCreate(h, data.size());
            else if (chooseCase == '2') hashRead(h, data.size());    
            else if (chooseCase == '3') hashUpdate(h, data.size());
            else if (chooseCase == '4') hashDelete(h, data.size());
            else {
                cout << "No CRUD operations are running, the workflow is now closed." << endl << endl;
                file.close();
            }
        } while ((chooseCase == '1') || (chooseCase == '2') || (chooseCase == '3') || (chooseCase == '4'));
    }
}
