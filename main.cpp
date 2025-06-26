#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <ctype.h>
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

    void updateItem(node data) {
        // later
    }

    void createItem(node data) {
        int index = hashFunction(data.id);

        for (auto& item : table[index]) {
            if (data.id == item.id) {
                char choice;

                cout << "ID " << data.id << " already exists. Do you want to update it? (y/n): ";
                cin >> choice;

                choice = tolower(choice);

                if (choice == 'y') { updateItem(data); }
                else { cout << "Item not inserted." << endl; }
                return;
            }
        }
        table[index].push_back(data);
    }

    node* readItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;

        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key) return &(*i);
        }
        return nullptr;
    }

    void deleteItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;

        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key) break;
        }
        if (i != table[index].end()) table[index].erase(i);
    }
};

int main() {
    char chooseCase;
    Hash* h = nullptr;
    ifstream file;

    while (true) {
        cout << endl
             << "CHOOSE THE SAMPLE:" << endl
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
            std::cerr << "Gagal membuka file JSON!" << std::endl;
            return 1;
        }
        json data;

        try {
            file >> data;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return 1;
        }

        for(int i = 0; i < data.size(); i++) {
            try {
                node temp;
                temp.id = data[i]["id"];
                temp.nama_produk = data[i]["nama_produk"];
                temp.kategori = data[i]["kategori"];
                temp.deskripsi = data[i]["deskripsi"];
                temp.harga = data[i]["harga"];
                temp.stok = data[i]["stok"];
                temp.berat_gram = data[i]["berat_gram"];
                temp.tersedia = data[i]["tersedia"];
                h->createItem(temp);
            } catch (const std::exception& e) {
                std::cerr << "Error processing item " << i << ": " << e.what() << std::endl;
                continue;
            }
        }
        node* result = h->readItem(1);

        if (result != nullptr) {
            cout << "ID: " << result->id << endl;
            cout << "Nama Produk: " << result->nama_produk << endl;
        } else {
            cout << "Item not found!" << endl;
        }
         file.close();
    }
}