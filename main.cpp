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
#include "master.h"
#include "slaves.h"
//  Inizio Main
int main(int argc, char *argv[]) {
//  Variavili da poter modificare
    unsigned const SIZE = 70;
    bool benchmark = false;
    unsigned bench_loop;
// Variabili Globali
    int id;
    int numProc;
    bool finish = false;
    unsigned slaves_number;
    unsigned row_for_slaves;
    MPI_Status status;
    MPI_Datatype segment_snd;
    MPI_Datatype segment_rcv;
    master* master_ptr;
    slaves* slave_ptr;
    double start_time;
    double finish_time;
//  Inizializzo MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    slaves_number = numProc - 1;
    row_for_slaves = SIZE / slaves_number;
//  Imposto i DataType
    MPI_Type_contiguous(SIZE * (row_for_slaves + 2), MPI_CXX_BOOL, &segment_snd);
    MPI_Type_commit(&segment_snd);
    MPI_Type_contiguous(SIZE * row_for_slaves, MPI_CXX_BOOL, &segment_rcv);
    MPI_Type_commit(&segment_rcv);
//  Creo l'istanza del master
    if (id == 0) {
        if (benchmark) {
            std::cout << "Benchmark mode: Insert number of generations" << std::endl;
            std::cin >> bench_loop;
        }
        start_time = MPI_Wtime();
        master_ptr = new master(SIZE, slaves_number, &status, &segment_snd, &segment_rcv, benchmark, bench_loop);
        if (benchmark) master_ptr->seed_cells_generator(24241, ((SIZE * SIZE) * 0.4));
        else master_ptr->random_cells_generator((SIZE * SIZE) * 0.4);
    }
//  Creo l'istanza degli schiavi
    else {
        slave_ptr = new slaves(SIZE, slaves_number, &status, &segment_snd, &segment_rcv);
    }
//  Inizia il ciclo delle generazioni dell'automa cellulare
    while(!finish){
        if(id == 0) {
            master_ptr->send_matrix_to_salves();
            master_ptr->recive_matrix_from_slaves();
            if (benchmark) finish = master_ptr->run_benchmark();
            else finish = master_ptr->run_allegro();
        }
        else {
            slave_ptr->recive_from_master();
            slave_ptr->compute_matrix();
            slave_ptr->send_to_master();
        }
//  Comunico a tutti i processi lo stato della variabile finish
        MPI_Bcast(&finish, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
    }
//  La barrier aspetta che tutti i processi escano dal loop
    MPI_Barrier(MPI_COMM_WORLD);
//  Stampa le statistiche della sessione e libera la memoria
    if (id == 0) {
        finish_time = MPI_Wtime();
        std::cout   << "Generazione: " << master_ptr->getGenerazione()  << std::endl
                    << "Cellule ancora vive: " << master_ptr->cells_alive() << std::endl
                    << "Tempo di esecuzione in secondi: " << (finish_time - start_time) << std::endl;
        delete master_ptr;
    }
    else delete slave_ptr;
//  Finalizzo MPI
    MPI_Finalize();
    return 0;
} // Main
