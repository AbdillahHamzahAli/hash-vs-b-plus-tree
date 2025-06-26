#include <bits/stdc++.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct node
{
    int id,stok,berat_gram;
    float harga;
    string nama_produk,kategori,deskripsi;
    bool tersedia;
};

struct Hash {
    int BUCKET;
    vector<list<node>> table;

    Hash(int b) {
        BUCKET = b;
        table.resize(BUCKET);
    }

    void insertItem(node data){
        int index = hashFunction(data.id);
        table[index].push_back(data);
    }

    void deleteItem(int key){
        int index = hashFunction(key);
        
        list<node>::iterator i;
        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key)
                break;
        }
        
        if (i != table[index].end())
            table[index].erase(i);
    }

    int hashFunction(int x) {
        return (x % BUCKET);
    }

    node* searchItem(int key){
        int index = hashFunction(key);
        list<node>::iterator i;
        for (i = table[index].begin(); i != table[index].end(); i++) {
            if ((*i).id == key)
                return &(*i);
        }
        return nullptr;
    }
};

int main() {
    Hash h(10);

    ifstream file("produk_data-500.json"); 
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
            h.insertItem(temp);
        } catch (const std::exception& e) {
            std::cerr << "Error processing item " << i << ": " << e.what() << std::endl;
            continue;
        }
    }

    node* result = h.searchItem(294);
    if (result != nullptr) {
        cout << "ID: " << result->id << endl;
        cout << "Nama Produk: " << result->nama_produk << endl;
    } else {
        cout << "Item not found!" << endl;
    }

    return 0;
}
