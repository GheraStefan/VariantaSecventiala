#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#define NUM_ELEMENTS 1000

using namespace std;

void random_numbers() {
    srand(std::time(nullptr));

    ofstream fin("input.txt");

    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        fin << rand() % 100 + 1 << " ";
    }

    fin.close();
}

void quicksort(vector<int>& v, int left, int right) {
    if (left >= right) {
        return;
    }
    int pivot = v[(left + right) / 2];
    int i = left;
    int j = right;
    while (i <= j) {
        while (v[i] < pivot) {
            i++;
        }
        while (v[j] > pivot) {
            j--;
        }
        if (i <= j) {
            swap(v[i], v[j]);
            i++;
            j--;
        }
    }
    quicksort(v, left, j);
    quicksort(v, i, right);
}

int main() {
    random_numbers();

    vector<int> v;
    ifstream fin("input.txt");
    if (fin.is_open()) {
        int x;
        while (fin >> x) {
            v.push_back(x);
        }
        fin.close();
    }
    else {
        cout << "Nu s-a putut deschide fisierul de intrare." << endl;
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();
    quicksort(v, 0, v.size() - 1);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    ofstream fout("output.txt");
    if (fout.is_open()) {
        for (int i = 0; i < v.size(); i++) {
            fout << v[i] << endl;
        }
        fout.close();
    }
    else {
        cout << "Nu s-a putut deschide fisierul de iesire." << endl;
        return 1;
    }

    cout << "Timpul de executie: " << duration.count() << " microsecunde" << endl;
    cout << "Memoria alocata: " << sizeof(v[0]) * v.size() << " bytes" << endl;
    cout << " ";
    return 0;
}