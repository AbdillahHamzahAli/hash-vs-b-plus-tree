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

    void createItem(node data) {
        int index = hashFunction(data.id);

        for (auto& item : table[index]) {
            if (data.id == item.id) {
                char choice;

                cout << "ID " << data.id << " already exists. Do you want to update it? (y/n)" << endl << "> ";
                cin >> choice;

                choice = tolower(choice);

                if (choice == 'y') { updateItem(data, index); }
                else { cout << endl << "Item not inserted." << endl << endl; }
                return;
            }
        }
        table[index].push_back(data);
    }

    void readItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;

        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key) {
                cout << "ID: " << (*i).id << endl;
                cout << "Nama Produk: " << (*i).nama_produk << endl;
                cout << "Kategori: " << (*i).kategori << endl;
                cout << "Harga: Rp" << fixed << setprecision(2) << (*i).harga << endl;
                cout << "Stok: " << (*i).stok << endl;
                cout << "Berat: " << (*i).berat_gram << " gr" << endl;
                cout << "Deskripsi: " << (*i).deskripsi << endl;
                cout << "Tersedia: " << (((*i).tersedia) ? "Ya" : "Tidak") << endl << endl;
                return;
            }
        }
        cout << "Item not found!" << endl << endl;;
        return;
    }

    void updateItem(node data, int key) {
        deleteItem(key);
        createItem(data);
    }

    void deleteItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;

        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key) break;
        }
        if (i != table[index].end()) table[index].erase(i);
        cout << endl;
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

    cout << "ID (bulat)      : ";
    cin >> id;
    add.id = id;
    cout << "Nama Produk     : ";
    cin >> nama_produk;
    add.nama_produk = nama_produk;
    cout << "Kategori        : ";
    cin >> kategori;
    add.kategori = kategori;
    cout << "Harga (desimal) : ";
    cin >> harga;
    add.harga = harga;
    cout << "Stok (bulat)    : ";
    cin >> stok;
    add.stok = stok;
    cout << "Berat (gram)    : ";
    cin >> berat_gram;
    add.berat_gram = berat_gram;
    cout << "Deskripsi       : ";
    cin >> deskripsi;
    add.deskripsi = deskripsi;
    cout << "Tersedia (0/1)  : ";
    cin >> tersedia;
    cout << endl;
    add.tersedia = tersedia;

    auto benchStart = chrono::high_resolution_clock::now();
    h->createItem(add);
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData, "hash-map");
}

void hashRead(Hash* h, int totalData) {
    char searchMode;
    int id;
    
    cout << "CHOOSE SEARCH MODE:" << endl
         << "(1) Single data" << endl
         << "(2) Data range" << endl
         << "(other) cancel" << endl
         << "> ";
    cin >> searchMode;

    if (searchMode == '1') {
        cin >> id;

        auto benchStart = chrono::high_resolution_clock::now();
        h->readItem(id);
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, totalData, "hash-map");
    } else if (searchMode == '2') {
        // NOT YET DONE
        auto benchStart = chrono::high_resolution_clock::now();
        
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, totalData, "hash-map");
    }
    return;
}

void hashUpdate(Hash* h, int totalData) {
    auto benchStart = chrono::high_resolution_clock::now();
    
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData, "hash-map");
}

void hashDelete(Hash* h, int totalData) {
    auto benchStart = chrono::high_resolution_clock::now();
    
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData, "hash-map");
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
                cout << "No CRUD operation running, workflow is now closed." << endl << endl;
                file.close();
            }
        } while ((chooseCase == '1') || (chooseCase == '2') || (chooseCase == '3') || (chooseCase == '4'));
    }
}
