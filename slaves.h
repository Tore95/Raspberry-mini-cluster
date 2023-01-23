/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Algoritmi Paralleli e Sistemi Distribuiti A.A. 2017/2018    *
 *                                                             *
 * Progetto Automa Cellulare:                                  *
 *                                                             *
 *                      Gioco della Vita                       *
 *                  su cluster di RaspBerry                    *
 *                                                             *
 * Realizzato Da:                                              *
 *                                                             *
 * Davide Impiombato       Mat:166750                          *
 * Salvatore Spagnuolo     Mat:182801                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if !defined(SLAVES_H)
#define SLAVES_H

#include <mpi.h>
#include <iostream>

class slaves {
private:
//  Dichiaro Variabili della classe
    int SIZE;
    int row;
    int n_slaves;
    bool matrix[12][70];
    bool matrix_tmp[10][70];
    MPI_Datatype* segment_snd;
    MPI_Datatype* segment_rcv;
    MPI_Status* status;
//  Funzioni prvate
    void game_rules(int r, int c);
public:
//  Costruttore della classe Slaves
    slaves(int _SIZE,int _n_slaves, MPI_Status* _status, MPI_Datatype* _segment_snd, MPI_Datatype* _segment_rcv);
//  Riceve la propria porzione dal master
    void recive_from_master();
//  Invia la porzione rielaborata al master
    void send_to_master();
//  Stampa tutta la matrice compresi i bordi su terminale
    void print_matrix();
//  Elabora la propria porzione di matrice
    void compute_matrix();
};

slaves::slaves(int _SIZE,int _n_slaves, MPI_Status* _status, MPI_Datatype* _segment_snd, MPI_Datatype* _segment_rcv) {
    status = _status;
    segment_rcv = _segment_rcv;
    segment_snd = _segment_snd;
    SIZE = _SIZE;
    n_slaves = _n_slaves;
    row = SIZE / n_slaves;
}

void slaves::recive_from_master() {
    MPI_Recv(matrix, SIZE * (row + 2), MPI_CXX_BOOL, 0, 0, MPI_COMM_WORLD, status);
}

void slaves::send_to_master() {
    MPI_Send(matrix_tmp, 1, *segment_rcv, 0, 0, MPI_COMM_WORLD);
}

void slaves::print_matrix() {
    for (int i = 0; i < (row + 2); i++) {
        for (int k = 0; k < SIZE; k++) {
            if (matrix[i][k])
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
    }
}

void slaves::compute_matrix() {
    for(int i = 1; i <= row; i++) {
        for(int k = 0; k < SIZE; k++)
            game_rules(i,k);
    }
}

void slaves::game_rules(int r, int c) {
  int cont = 0;

  if (c == 0) {
    //if (matrix[r - 1][c - 1]) cont++;
    if (matrix[r - 1][c]) cont++;
    if (matrix[r - 1][c + 1]) cont++;

    //if (matrix[r][c - 1]) cont++;
    if (matrix[r][c + 1]) cont++;

    //if (matrix[r + 1][c - 1]) cont++;
    if (matrix[r + 1][c]) cont++;
    if (matrix[r + 1][c + 1]) cont++;
  }

  else if (c == SIZE - 1) {
    if (matrix[r - 1][c - 1]) cont++;
    if (matrix[r - 1][c]) cont++;
    //if (matrix[r - 1][c + 1]) cont++;

    if (matrix[r][c - 1]) cont++;
    //if (matrix[r][c + 1]) cont++;

    if (matrix[r + 1][c - 1]) cont++;
    if (matrix[r + 1][c]) cont++;
    //if (matrix[r + 1][c + 1]) cont++;
  }

  else {
    if (matrix[r - 1][c - 1]) cont++;
    if (matrix[r - 1][c]) cont++;
    if (matrix[r - 1][c + 1]) cont++;

    if (matrix[r][c - 1]) cont++;
    if (matrix[r][c + 1]) cont++;

    if (matrix[r + 1][c - 1]) cont++;
    if (matrix[r + 1][c]) cont++;
    if (matrix[r + 1][c + 1]) cont++;
  }

  r -= 1;

  switch (cont) {
    case 0:
    case 1: matrix_tmp[r][c] = false; break;
    case 2: break;
    case 3: matrix_tmp[r][c] = true; break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8: matrix_tmp[r][c] = false; break;
  }
}


#endif // SLAVES_H
