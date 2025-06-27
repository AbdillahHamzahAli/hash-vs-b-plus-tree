#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <ctype.h>
#include <iomanip>
#include <cmath>
#include "json.hpp"
#include <chrono>

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

void chronoBench(bench start, bench end, int totalData) {
    chrono::duration<double, milli> timeElapsed = end - start;
    double opsPerSec = (totalData * 1000.0) / timeElapsed.count();

    cout << "------------ChronoBench-----------" << endl;
    cout << "| Total ops  : " << setw(9) << totalData << setw(10) << "|" << endl;
    cout << "| Total time : " << setw(9) << timeElapsed.count() << " ms" << setw(7) << "|" << endl;
    cout << "| Throughput : " << setw(9) << static_cast<int>(floor(opsPerSec)) << " ops/sec" << setw(2) << "|" << endl;
    cout << "----------------------------------" << endl << endl;
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
                node temp;
                temp.id = data[i]["id"];
                temp.nama_produk = data[i]["nama_produk"];
                temp.kategori = data[i]["kategori"];
                temp.harga = data[i]["harga"];
                temp.stok = data[i]["stok"];
                temp.berat_gram = data[i]["berat_gram"];
                temp.deskripsi = data[i]["deskripsi"];
                temp.tersedia = data[i]["tersedia"];
                h->createItem(temp);
            } catch (const std::exception& e) {
                std::cerr << "Error processing item " << i << ": " << e.what() << std::endl;
                continue;
            }
        }      
        auto benchEnd = chrono::high_resolution_clock::now();
        chronoBench(benchStart, benchEnd, data.size());
        
        // WIP AFTER THIS COMMENT
        node bomb;
        bomb.id = 1;
        bomb.nama_produk = "a";
        bomb.kategori = "a";
        bomb.harga = 1.0;
        bomb.stok = 1;
        bomb.berat_gram = 1;
        bomb.deskripsi = "aa";
        bomb.tersedia = false;

        node wow;
        wow.id = 1;
        wow.nama_produk = "b";
        wow.kategori = "b";
        wow.harga = 2.0;
        wow.stok = 2;
        wow.berat_gram = 2;
        wow.deskripsi = "bb";
        wow.tersedia = true;

        h->createItem(bomb);
        h->readItem(bomb.id);
        h->updateItem(wow, wow.id);
        h->readItem(wow.id);
        h->deleteItem(bomb.id);
        h->readItem(bomb.id);
        h->readItem(101);

        file.close();
    }
}
