#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <chrono>
#include <fstream>
#include <iomanip> // Untuk setw
#include "json.hpp" // Pastikan file json.hpp ada di direktori yang sama

using namespace std;
using json = nlohmann::json;

#define ORDER 4

// Struct untuk data produk
struct Product {
    int id,
        stok,
        berat_gram;
    float harga;
    string nama_produk,
           kategori,
           deskripsi;
    bool tersedia;
    
    // Fungsi untuk mencetak detail produk
    void print() const {
        cout << "-------------------------" << endl;
        cout << "ID: " << id << endl;
        cout << "Nama Produk: " << nama_produk << endl;
        cout << "Harga: Rp" << fixed << setprecision(2) << harga << endl;
        cout << "Stok: " << stok << endl;
        cout << "Berat (gram): " << berat_gram << endl;
        cout << "Kategori: " << kategori << endl;
        cout << "Deskripsi: " << deskripsi << endl;
        cout << "Tersedia: " << (tersedia ? "Ya" : "Tidak") << endl;
        cout << "-------------------------" << endl;
    }
};

// Struct tunggal yang bisa menjadi Leaf Node atau Internal Node
struct BPTreeNode {
    bool is_leaf;
    int *keys;
    int size;
    BPTreeNode* parent;
    union {
        BPTreeNode** children;
        Product** data_pointers;
    };
    BPTreeNode* next;
};

// Struct utama yang merepresentasikan pohon
struct BPlusTree {
    BPTreeNode* root;
    int order;
};

// --- DEKLARASI FUNGSI ---

// Fungsi Create / Destroy
BPlusTree* create_bplustree();
BPTreeNode* create_leaf_node();
BPTreeNode* create_internal_node();
void destroy_node(BPTreeNode* node);
void destroy_bplustree(BPlusTree* tree);
Product* create_product(int id, string nama, float harga, int stok, int berat_gram, string kategori, string deskripsi);

// Fungsi Operasi Utama
void insert(BPlusTree* tree, Product* value);
Product* search(BPlusTree* tree, int key);
Product* update_product(BPlusTree* tree, int id, string nama, float harga, int stok, int berat_gram, string kategori, string deskripsi);
void delete_key(BPlusTree* tree, int key);

// Fungsi Helper
void print_tree(BPlusTree* tree);
void search_range(BPlusTree* tree, int start_key, int end_key);
void insert_internal(BPlusTree* tree, int key, BPTreeNode* right_child, BPTreeNode* left_child);
void remove_entry_from_node(BPTreeNode* node, int key, BPTreeNode* child_to_remove);
void handle_underflow(BPlusTree* tree, BPTreeNode* node);
void chronoBench(chrono::high_resolution_clock::time_point start, chrono::high_resolution_clock::time_point end, int totalData);

// --- FUNGSI INTERAKTIF UNTUK MENU CRUD ---
void insert_data(BPlusTree* tree);
void read_data(BPlusTree* tree);
void update_data_interactive(BPlusTree* tree);
void delete_data(BPlusTree* tree);

// --- IMPLEMENTASI FUNGSI (Tidak berubah dari kode Anda, hanya ditambahkan fungsi baru) ---
BPlusTree* create_bplustree() {
    BPlusTree* tree = new BPlusTree();
    tree->root = nullptr;
    tree->order = ORDER;
    return tree;
}

BPTreeNode* create_leaf_node() {
    BPTreeNode* node = new BPTreeNode();
    node->is_leaf = true;
    node->keys = new int[ORDER];
    node->size = 0;
    node->parent = nullptr;
    node->data_pointers = new Product*[ORDER];
    node->next = nullptr;
    return node;
}

BPTreeNode* create_internal_node() {
    BPTreeNode* node = new BPTreeNode();
    node->is_leaf = false;
    node->keys = new int[ORDER];
    node->size = 0;
    node->parent = nullptr;
    node->children = new BPTreeNode*[ORDER + 1];
    return node;
}

void destroy_node(BPTreeNode* node) {
    if (node == nullptr) return;
    if (node->is_leaf) {
        for (int i = 0; i < node->size; ++i) delete node->data_pointers[i];
        delete[] node->data_pointers;
    } else {
        for (int i = 0; i <= node->size; ++i) destroy_node(node->children[i]);
        delete[] node->children;
    }
    delete[] node->keys;
    delete node;
}

void destroy_bplustree(BPlusTree* tree) {
    if (tree) {
        destroy_node(tree->root);
        delete tree;
    }
}

Product* create_product(int id, string nama, float harga, int stok, int berat_gram, string kategori, string deskripsi) {
    Product* p = new Product();
    p->id = id; p->nama_produk = nama; p->harga = harga; p->stok = stok; p->berat_gram = berat_gram; p->kategori = kategori; p->deskripsi = deskripsi;
    p->tersedia = stok > 0;
    return p;
}

void insert_internal(BPlusTree* tree, int key, BPTreeNode* right_child, BPTreeNode* left_child);
void split_internal_node(BPlusTree* tree, BPTreeNode* node);

void insert(BPlusTree* tree, Product* value) {
    int key = value->id;
    if (tree->root == nullptr) {
        tree->root = create_leaf_node();
        tree->root->keys[0] = key;
        tree->root->data_pointers[0] = value;
        tree->root->size = 1;
        return;
    }
    BPTreeNode* cursor = tree->root;
    while (!cursor->is_leaf) {
        int i = 0;
        while (i < cursor->size && key >= cursor->keys[i]) i++;
        cursor = cursor->children[i];
    }
    int i = cursor->size - 1;
    while (i >= 0 && cursor->keys[i] > key) {
        cursor->keys[i + 1] = cursor->keys[i];
        cursor->data_pointers[i + 1] = cursor->data_pointers[i];
        i--;
    }
    cursor->keys[i + 1] = key;
    cursor->data_pointers[i + 1] = value;
    cursor->size++;
    if (cursor->size == tree->order) {
        BPTreeNode* new_leaf = create_leaf_node();
        int mid_point = tree->order / 2;
        for (int j = mid_point; j < tree->order; ++j) {
            new_leaf->keys[j - mid_point] = cursor->keys[j];
            new_leaf->data_pointers[j - mid_point] = cursor->data_pointers[j];
            new_leaf->size++;
        }
        cursor->size = mid_point;
        new_leaf->next = cursor->next;
        cursor->next = new_leaf;
        insert_internal(tree, new_leaf->keys[0], new_leaf, cursor);
    }
}

void insert_internal(BPlusTree* tree, int key, BPTreeNode* right_child, BPTreeNode* left_child) {
    BPTreeNode* parent = left_child->parent;
    if (parent == nullptr) {
        parent = create_internal_node();
        parent->keys[0] = key;
        parent->children[0] = left_child;
        parent->children[1] = right_child;
        parent->size = 1;
        tree->root = parent;
        left_child->parent = parent;
        right_child->parent = parent;
        return;
    }
    int i = parent->size - 1;
    while(i >= 0 && parent->keys[i] > key) {
        parent->keys[i+1] = parent->keys[i];
        parent->children[i+2] = parent->children[i+1];
        i--;
    }
    parent->keys[i+1] = key;
    parent->children[i+2] = right_child;
    parent->size++;
    right_child->parent = parent;
    if (parent->size == tree->order) {
        split_internal_node(tree, parent);
    }
}

void split_internal_node(BPlusTree* tree, BPTreeNode* node) {
    BPTreeNode* new_internal = create_internal_node();
    int mid_point = tree->order / 2;
    for (int i = mid_point + 1; i < tree->order; ++i) {
        new_internal->keys[i - (mid_point + 1)] = node->keys[i];
        new_internal->size++;
    }
    for (int i = mid_point + 1; i <= tree->order; ++i) {
        new_internal->children[i - (mid_point + 1)] = node->children[i];
        node->children[i]->parent = new_internal;
    }
    int promoted_key = node->keys[mid_point];
    node->size = mid_point;
    insert_internal(tree, promoted_key, new_internal, node);
}

Product* search(BPlusTree* tree, int key) {
    if (tree->root == nullptr) return nullptr;
    BPTreeNode* cursor = tree->root;
    while (!cursor->is_leaf) {
        int i = 0;
        while (i < cursor->size && key >= cursor->keys[i]) i++;
        cursor = cursor->children[i];
    }
    for (int i = 0; i < cursor->size; ++i) {
        if (cursor->keys[i] == key) return cursor->data_pointers[i];
    }
    return nullptr;
}

Product* update_product(BPlusTree* tree, int id, string nama, float harga, int stok, int berat_gram, string kategori, string deskripsi) {
    Product* product_to_update = search(tree, id);
    if (product_to_update != nullptr) {
        product_to_update->harga = harga;
        product_to_update->stok = stok;
        product_to_update->tersedia = stok > 0;
        product_to_update->nama_produk = nama;
        product_to_update->berat_gram = berat_gram;
        product_to_update->kategori = kategori;
        product_to_update->deskripsi = deskripsi;
        return product_to_update;
    }
    return nullptr;
}

void delete_key(BPlusTree* tree, int key) {
    if (tree->root == nullptr) return;
    BPTreeNode* cursor = tree->root;
    while (!cursor->is_leaf) {
        int i = 0;
        while (i < cursor->size && key >= cursor->keys[i]) i++;
        cursor = cursor->children[i];
    }
    remove_entry_from_node(cursor, key, nullptr);
    handle_underflow(tree, cursor);
}

void remove_entry_from_node(BPTreeNode* node, int key, BPTreeNode* child_to_remove) {
    int key_idx = 0;
    while (key_idx < node->size && node->keys[key_idx] != key) {
        key_idx++;
    }
    if (key_idx < node->size) {
        if (node->is_leaf) {
            delete node->data_pointers[key_idx];
            for (int i = key_idx; i < node->size - 1; ++i) {
                node->keys[i] = node->keys[i + 1];
                node->data_pointers[i] = node->data_pointers[i + 1];
            }
        } else {
            int child_idx = 0;
            while(child_idx <= node->size && node->children[child_idx] != child_to_remove) {
                child_idx++;
            }
            for (int i = key_idx; i < node->size - 1; ++i) node->keys[i] = node->keys[i+1];
            for (int i = child_idx; i < node->size; ++i) node->children[i] = node->children[i+1];
        }
        node->size--;
    }
}

void handle_underflow(BPlusTree* tree, BPTreeNode* node) {
    if (node == tree->root || node->size >= tree->order / 2) {
        if (node == tree->root && node->size == 0 && !node->is_leaf) {
            tree->root = node->children[0];
            tree->root->parent = nullptr;
            delete node;
        }
        return;
    }

    BPTreeNode* parent = node->parent;
    int child_idx = 0;
    while (child_idx <= parent->size && parent->children[child_idx] != node) {
        child_idx++;
    }

    if (child_idx > 0) {
        BPTreeNode* left_sibling = parent->children[child_idx - 1];
        if (left_sibling->size > tree->order / 2) {
            for (int i = node->size; i > 0; --i) node->keys[i] = node->keys[i - 1];
            if (!node->is_leaf) for (int i = node->size + 1; i > 0; --i) node->children[i] = node->children[i - 1];
            else for (int i = node->size; i > 0; --i) node->data_pointers[i] = node->data_pointers[i - 1];

            if (node->is_leaf) {
                node->keys[0] = left_sibling->keys[left_sibling->size - 1];
                node->data_pointers[0] = left_sibling->data_pointers[left_sibling->size - 1];
                parent->keys[child_idx - 1] = node->keys[0];
            } else {
                node->keys[0] = parent->keys[child_idx - 1];
                node->children[0] = left_sibling->children[left_sibling->size];
                node->children[0]->parent = node;
                parent->keys[child_idx-1] = left_sibling->keys[left_sibling->size - 1];
            }
            node->size++;
            left_sibling->size--;
            return;
        }
    }

    if (child_idx < parent->size) {
        BPTreeNode* right_sibling = parent->children[child_idx + 1];
        if (right_sibling->size > tree->order / 2) {
            if (node->is_leaf) {
                node->keys[node->size] = right_sibling->keys[0];
                node->data_pointers[node->size] = right_sibling->data_pointers[0];
                parent->keys[child_idx] = right_sibling->keys[1];
                for (int i = 0; i < right_sibling->size - 1; ++i) right_sibling->keys[i] = right_sibling->keys[i + 1];
                for (int i = 0; i < right_sibling->size - 1; ++i) right_sibling->data_pointers[i] = right_sibling->data_pointers[i + 1];
            } else {
                 node->keys[node->size] = parent->keys[child_idx];
                 node->children[node->size+1] = right_sibling->children[0];
                 node->children[node->size+1]->parent = node;
                 parent->keys[child_idx] = right_sibling->keys[0];
                 for (int i=0; i<right_sibling->size-1; ++i) right_sibling->keys[i] = right_sibling->keys[i+1];
                 for (int i=0; i<right_sibling->size; ++i) right_sibling->children[i] = right_sibling->children[i+1];
            }
            node->size++;
            right_sibling->size--;
            return;
        }
    }

    if (child_idx > 0) {
        BPTreeNode* left_sibling = parent->children[child_idx - 1];
        for (int i = 0; i < node->size; ++i) {
            left_sibling->keys[left_sibling->size] = node->keys[i];
            if (node->is_leaf) left_sibling->data_pointers[left_sibling->size] = node->data_pointers[i];
            left_sibling->size++;
        }
        if (!node->is_leaf) {
            left_sibling->keys[left_sibling->size] = parent->keys[child_idx - 1];
            left_sibling->size++;
             for(int i = 0; i <= node->size; ++i) {
                left_sibling->children[left_sibling->size] = node->children[i];
                node->children[i]->parent = left_sibling;
             }
        }
        
        left_sibling->next = node->next;
        remove_entry_from_node(parent, parent->keys[child_idx - 1], node);
        delete node;
    } else {
        BPTreeNode* right_sibling = parent->children[child_idx + 1];
        for (int i = 0; i < right_sibling->size; ++i) {
            node->keys[node->size] = right_sibling->keys[i];
            if (node->is_leaf) node->data_pointers[node->size] = right_sibling->data_pointers[i];
            node->size++;
        }
        if (!node->is_leaf) {
             node->keys[node->size] = parent->keys[child_idx];
             node->size++;
             for(int i=0; i <= right_sibling->size; ++i) {
                node->children[node->size] = right_sibling->children[i];
                right_sibling->children[i]->parent = node;
             }
        }
        node->next = right_sibling->next;
        remove_entry_from_node(parent, parent->keys[child_idx], right_sibling);
        delete right_sibling;
    }

    handle_underflow(tree, parent);
}

void print_tree(BPlusTree* tree) {
    if (tree->root == nullptr) { cout << "Pohon kosong." << endl; return; }
    queue<BPTreeNode*> q;
    q.push(tree->root);
    while(!q.empty()){
        int levelSize = q.size();
        for(int i = 0; i < levelSize; ++i){
            BPTreeNode* curr = q.front(); q.pop();
            cout << "[";
            for(int j=0; j<curr->size; ++j) cout << curr->keys[j] << (j == curr->size-1 ? "" : ",");
            cout << "] ";
            if(!curr->is_leaf) for(int j=0; j<=curr->size; ++j) q.push(curr->children[j]);
        }
        cout << endl;
    }
}

void search_range(BPlusTree* tree, int start_key, int end_key, int totalData) {
    if (tree->root == nullptr) { cout << "Pohon kosong." << endl; return; }
    BPTreeNode* cursor = tree->root;
    auto benchStart = chrono::high_resolution_clock::now();
    while (!cursor->is_leaf) {
        int i = 0;
        while (i < cursor->size && start_key >= cursor->keys[i]) i++;
        cursor = cursor->children[i];
    }
    bool found = false;
    while (cursor != nullptr) {
        for (int i = 0; i < cursor->size; ++i) {
            if (cursor->keys[i] >= start_key && cursor->keys[i] <= end_key) {
                cursor->data_pointers[i]->print();
                found = true;
            }
        }
        if (cursor->keys[cursor->size-1] >= end_key && (cursor->next == nullptr || cursor->next->keys[0] > end_key)) break;
        cursor = cursor->next;
    }

    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData);
    if (!found) cout << "Tidak ada produk dalam rentang ID " << start_key << " hingga " << end_key << endl;
}

void chronoBench(chrono::high_resolution_clock::time_point start, chrono::high_resolution_clock::time_point end, int totalData) {
    chrono::duration<double, milli> timeElapsed = end - start;
    double opsPerSec = (totalData * 1000.0) / timeElapsed.count();

    cout << "------------ChronoBench-----------" << endl;
    cout << "| Total ops  : " << setw(9) << totalData << setw(10) << "|" << endl;
    cout << "| Total time : " << setw(9) << timeElapsed.count() << " ms" << setw(7) << "|" << endl;
    cout << "| Throughput : " << setw(9) << static_cast<int>(floor(opsPerSec)) << " ops/sec" << setw(2) << "|" << endl;
    cout << "----------------------------------" << endl << endl;
}

// --- IMPLEMENTASI FUNGSI INTERAKTIF BARU ---

// Fungsi untuk menangani input Create dari pengguna
void insert_data(BPlusTree* tree, int totalData) {
    int id, stok, berat_gram;
    float harga;
    string nama_produk, kategori, deskripsi;

    cout << "Masukkan ID Produk: "; cin >> id;
    if (search(tree, id) != nullptr) {
        cout << "Error: Produk dengan ID " << id << " sudah ada." << endl;
        return;
    }
    cout << "Masukkan Nama Produk: "; cin.ignore(); getline(cin, nama_produk);
    cout << "Masukkan Kategori: "; getline(cin, kategori);
    cout << "Masukkan Deskripsi: "; getline(cin, deskripsi);
    cout << "Masukkan Harga: "; cin >> harga;
    cout << "Masukkan Stok: "; cin >> stok;
    cout << "Masukkan Berat (gram): "; cin >> berat_gram;

    auto benchStart = chrono::high_resolution_clock::now();
    insert(tree, create_product(id, nama_produk, harga, stok, berat_gram, kategori, deskripsi));
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData);

    cout << "Produk baru dengan ID " << id << " berhasil ditambahkan." << endl;
}

// Fungsi untuk menangani input Read dari pengguna
void read_data(BPlusTree* tree, int totalData) {
    int id;
    cout << "Masukkan ID Produk yang ingin dicari: ";
    cin >> id;

    auto benchStart = chrono::high_resolution_clock::now();
    Product* result = search(tree, id);

    if (result != nullptr) {
        cout << "Produk ditemukan:" << endl;
        result->print();
    } else {
        cout << "Produk dengan ID " << id << " tidak ditemukan." << endl;
    }

    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData);

}

// Fungsi untuk menangani input Update dari pengguna
void update_data_interactive(BPlusTree* tree, int totalData) {
    int id, stok, berat_gram;
    float harga;
    string nama_produk, kategori, deskripsi;

    cout << "Masukkan ID Produk yang ingin diupdate: ";
    cin >> id;

    if (search(tree, id) == nullptr) {
        cout << "Error: Produk dengan ID " << id << " tidak ditemukan." << endl;
        return;
    }

    cout << "Masukkan Nama Produk Baru: "; cin.ignore(); getline(cin, nama_produk);
    cout << "Masukkan Kategori Baru: "; getline(cin, kategori);
    cout << "Masukkan Deskripsi Baru: "; getline(cin, deskripsi);
    cout << "Masukkan Harga Baru: "; cin >> harga;
    cout << "Masukkan Stok Baru: "; cin >> stok;
    cout << "Masukkan Berat (gram) Baru: "; cin >> berat_gram;

    auto benchStart = chrono::high_resolution_clock::now();
    update_product(tree, id, nama_produk, harga, stok, berat_gram, kategori, deskripsi);
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData);

    cout << "Produk dengan ID " << id << " berhasil diupdate." << endl;
}

// Fungsi untuk menangani input Delete dari pengguna
void delete_data(BPlusTree* tree, int totalData) {
    int id;
    cout << "Masukkan ID Produk yang ingin dihapus: ";
    cin >> id;

    if (search(tree, id) == nullptr) {
        cout << "Error: Produk dengan ID " << id << " tidak ditemukan." << endl;
        return;
    }
    auto benchStart = chrono::high_resolution_clock::now();
    delete_key(tree, id);
    auto benchEnd = chrono::high_resolution_clock::now();
    chronoBench(benchStart, benchEnd, totalData);

    cout << "Produk dengan ID " << id << " berhasil dihapus." << endl;
}


// --- FUNGSI MAIN YANG DIPERBAIKI ---
int main() {
    char chooseCase;
    ifstream file;
    
    // Loop utama program
    while(true){
        BPlusTree* tree = create_bplustree();

        cout << "==============================" << endl;
        cout << "      B+ TREE DATABASE        " << endl;
        cout << "==============================" << endl;
        cout << "PILIH SAMPEL DATA UNTUK DIMUAT:" << endl
             << "(a) 100 data" << endl
             << "(b) 500 data" << endl
             << "(c) 1000 data" << endl
             << "(d) Lanjutkan tanpa memuat data (pohon kosong)" << endl
             << "(lainnya) Keluar" << endl
             << "> ";
        cin >> chooseCase;
        cout << endl;

        chooseCase = tolower(chooseCase);
        string filename;

        switch(chooseCase) {
            case 'a': filename = "produk_data-100.json"; break;
            case 'b': filename = "produk_data-500.json"; break;
            case 'c': filename = "produk_data-1000.json"; break;
            case 'd': filename = ""; break;
            default:  cout << "Program Selesai." << endl; return 0;
        }

        json data;

        if (!filename.empty()) {
            file.open(filename);
            if (!file.is_open()) {
                cerr << "Gagal membuka file JSON: " << filename << endl;
                return 1;
            }
            
            try {
                file >> data;
            } catch (const exception& e) {
                cerr << "Error parsing JSON: " << e.what() << endl;
                return 1;
            }

            auto benchStart = chrono::high_resolution_clock::now();

            for(const auto& item : data) {
                insert(tree, create_product(
                    item["id"],
                    item["nama_produk"],
                    item["harga"],
                    item["stok"],
                    item["berat_gram"],
                    item["kategori"],
                    item["deskripsi"]
                ));
            }

            auto benchEnd = chrono::high_resolution_clock::now();
            cout << "Data dari " << filename << " berhasil dimuat." << endl;
            chronoBench(benchStart, benchEnd, data.size());
            file.close();
        } else {
            cout << "Pohon kosong dibuat. Silakan gunakan operasi Create." << endl;
        }

        // Loop untuk operasi CRUD setelah data dimuat
        do {
            cout << "\n--- MENU OPERASI CRUD ---" << endl
                 << "(1) Create (Tambah Produk)" << endl
                 << "(2) Read (Cari Produk)" << endl
                 << "(3) Update (Ubah Produk)" << endl
                 << "(4) Delete (Hapus Produk)" << endl
                 << "(5) Tampilkan Struktur Pohon" << endl
                 << "(6) Cari Produk dalam Rentang ID" << endl
                 << "(lainnya) Kembali ke Menu Utama" << endl
                 << "> ";
            cin >> chooseCase;
            cout << endl;
            
            if (chooseCase == '1') {
                insert_data(tree, data.size());;
            }
            else if (chooseCase == '2') {
                read_data(tree,data.size());
            }
            else if (chooseCase == '3') {
                update_data_interactive(tree, data.size());
            }
            else if (chooseCase == '4') {
                delete_data(tree, data.size());
            }
            else if (chooseCase == '5') {
                cout << "Struktur Pohon Saat Ini (Level by Level):" << endl;
                print_tree(tree);
            }
            else if (chooseCase == '6') {
                int start_id, end_id;
                cout << "Masukkan ID Awal: "; cin >> start_id;
                cout << "Masukkan ID Akhir: "; cin >> end_id;
                search_range(tree, start_id, end_id, data.size());
            }
            else {
                cout << "Operasi CRUD selesai. Kembali ke menu utama..." << endl << endl;
                break;
            }
        } while (true);

        cout << "Membersihkan memori pohon saat ini..." << endl;
        destroy_bplustree(tree);
        cout << "==============================" << endl << endl;
    }
    
    return 0;
}