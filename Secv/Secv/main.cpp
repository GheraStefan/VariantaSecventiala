#include <iostream>
#include <fstream>
#include <vector>
#include "mpi.h"
#include <string>

using namespace std;

void quicksort_parallel(vector<int>& v, int left, int right);

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            cout << "Usage: " << argv[0] << " filename" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    vector<int> input_sizes = { 50, 100, 1000, 10000 }; // dimensiunile fisierelor de intrare
    string input_prefix = "input"; // prefixul fisierelor de intrare
    string output_prefix = "output"; // prefixul fisierelor de iesire

    // Sortam fiecare set de date
    for (int i = 0; i < input_sizes.size(); i++) {
        vector<int> v;
        int n = 0;

        // Citim datele din fisier
        if (rank == 0) {
            string input_filename = input_prefix + to_string(input_sizes[i]);
            ifstream fin(input_filename);
            fin >> n;
            v.resize(n);
            for (int i = 0; i < n; i++) {
                fin >> v[i];
            }
            fin.close();
        }

        // Trimitem dimensiunea vectorului catre toate procesele
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Calculam dimensiunea bucății și trimitem bucățile de date la fiecare proces
        int chunk_size = n / size;
        vector<int> local_v(chunk_size);
        MPI_Scatter(&v[0], chunk_size, MPI_INT, &local_v[0], chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

        // Sortam bucata locala
        quicksort_parallel(local_v, 0, chunk_size - 1);

        // Adunam bucata sortata la procesul master
        MPI_Gather(&local_v[0], chunk_size, MPI_INT, &v[0], chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

        // Procesul master afiseaza datele sortate in fisierul de iesire corespunzator
        if (rank == 0) {
            string output_filename = output_prefix + to_string(input_sizes[i]);
            ofstream fout(output_filename);
            for (int i = 0; i < n; i++) {
                fout << v[i] << " ";
            }
            fout.close();
        }
    }

    MPI_Finalize();
    return 0;
}


void quicksort_parallel(vector<int>& v, int left, int right) {
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

    // Recursiv sortam cele doua parti ale vectorului in paralel
    if (left < j) {
#pragma omp task
        quicksort_parallel(v, left, j);
    }
    if (i < right) {
#pragma omp task
        quicksort_parallel(v, i, right);
    }
#pragma omp taskwait
}