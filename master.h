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
#ifndef MASTER_H
#define MASTER_H

#include <time.h>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <mpi.h>

class master {
private:
//  Dichiaro Variabili
    int SIZE;
    int n_slaves;
    bool matrix[72][70];
    int row_for_slaves;
    unsigned count_loop = 0;
    unsigned limit;
    MPI_Datatype* segment_snd;
    MPI_Datatype* segment_rcv;
    MPI_Status* status;
//  Variabili di Allegro
    int n_celle;
    int res_monitor_x;
    int res_monitor_y;
    int DISPLAY_RES = 700;
    int l_cella;
    float res_x, res_y;
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_MONITOR_INFO info;
    ALLEGRO_TRANSFORM ridimensionamento;
//  Funzioni Private
    void All_init();
    
public:
//  Costruttore del Master
    master(int _SIZE, int _n_slaves, MPI_Status* _status, MPI_Datatype* _segment_snd, MPI_Datatype* _segment_rcv, bool bench, unsigned _limit);
//  Genera un numero random di cellule
    void random_cells_generator(unsigned number_of_cells);
//  Genenra un numero random di cellule basandosi su un seme dato
    void seed_cells_generator(unsigned seed, int number_of_cells);
//  Invia ogni porzione di matrice agli schiavi
    void send_matrix_to_salves();
//  Riceve tutte le porzioni e le ricompatta in un unica matrice
    void recive_matrix_from_slaves();
//  Ritorna il numero di cellule vive
    int cells_alive();
//  Stampa l'intera matrice (compresi i bordi) su terminale
    void print_all_matrix();
//  Avvia allegro e ritorna l'evento di chiusura
    bool run_allegro();
//  Avvia il benchamerk
    bool run_benchmark();
//  Ritorna la generazione corrente
    unsigned getGenerazione();
};

master::master(int _SIZE, int _n_slaves, MPI_Status* _status, MPI_Datatype* _segment_snd, MPI_Datatype* _segment_rcv, bool bench, unsigned _limit) {
    status = _status;
    segment_rcv = _segment_rcv;
    segment_snd = _segment_snd;
    SIZE = _SIZE;
    n_slaves = _n_slaves;
    row_for_slaves = SIZE / n_slaves;
    limit = _limit;
    if (!bench) All_init();
    for (int i = 0; i < SIZE + 2; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = false;
        }
    }
}

void master::random_cells_generator(unsigned number_of_cells) {
    srand(time(0));
    int i = 0;
    while (i <= number_of_cells) {
        unsigned x = rand() % SIZE + 1;
        unsigned y = rand() % SIZE;
        matrix[x][y] = true;
        i++;
    }
    std::cout << "Cellule vive generazione 0: " << cells_alive() << std::endl;
}

void master::seed_cells_generator(unsigned seed, int number_of_cells) {
    srand(seed);
    int i = 0;
    while (i <= number_of_cells) {
        unsigned x = rand() % SIZE + 1;
        unsigned y = rand() % SIZE;
        matrix[x][y] = true;
        i++;
    }
    std::cout << "Cellule vive generazione 0: " << cells_alive() << std::endl;
}

void master::send_matrix_to_salves() {
    int index = 0;
    for (int i = 1; i <= n_slaves; i++) {
        MPI_Send(&matrix[index][0], 1, *segment_snd, i, 0, MPI_COMM_WORLD);
        index += row_for_slaves;
    }
}

void master::recive_matrix_from_slaves() {
    int index = 1;
    for (int i = 1; i <= n_slaves; i++) {
        MPI_Recv(&matrix[index][0], SIZE * row_for_slaves, MPI_CXX_BOOL, i, 0, MPI_COMM_WORLD, status);
        index += row_for_slaves;
    }
    count_loop++;
}

void master::print_all_matrix() {
    for (int i = 0; i < SIZE + 2; i++) {
        for (int k = 0; k < SIZE; k++) {
            if (matrix[i][k])
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
    }
}

int master::cells_alive() {
    int cont = 0;
    for (int i = 0; i < SIZE + 2; i++)
        for (int k = 0; k < SIZE; k++)
            if (matrix[i][k])
                cont++;
    return cont;
}

bool master::run_benchmark() {
    return (count_loop > limit);
}

unsigned master::getGenerazione() {
    return count_loop - 1;
}

void master::All_init() {
    n_celle = SIZE;

    l_cella = DISPLAY_RES / (n_celle);

    assert(al_init());

    al_get_monitor_info(0, &info);
    res_monitor_x = info.x2 - info.x1;
    res_monitor_y = info.y2 - info.y1;
    res_x = (res_monitor_x / (float)DISPLAY_RES);
    res_y = (res_monitor_y / (float)DISPLAY_RES);

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    display = al_create_display(res_monitor_x, res_monitor_y);
    assert(display);
    al_set_window_title(display, "Game of Life");

    al_identity_transform(&ridimensionamento);
    al_scale_transform(&ridimensionamento, res_x, res_y);
    al_use_transform(&ridimensionamento);

    assert(al_install_keyboard());

    assert(al_init_primitives_addon());

    event_queue = al_create_event_queue();
    assert(event_queue);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_clear_to_color(al_map_rgb(70, 70, 70));
    for (int i = 0; i < DISPLAY_RES; i += l_cella) {
        al_draw_line(0, i, DISPLAY_RES, i, al_map_rgb(230, 230, 230), 1.0);
        al_draw_line(i, 0, i, DISPLAY_RES, al_map_rgb(230, 230, 230), 1.0);
    }
    al_flip_display();

    al_clear_to_color(al_map_rgb(70, 70, 70));
    for (int i = 0; i < DISPLAY_RES; i += l_cella) {
        al_draw_line(0, i, DISPLAY_RES, i, al_map_rgb(230, 230, 230), 1.0);
        al_draw_line(i, 0, i, DISPLAY_RES, al_map_rgb(230, 230, 230), 1.0);
    }
}

bool master::run_allegro() {
    al_rest(0.1);
    ALLEGRO_EVENT ev;
    while (!al_is_event_queue_empty(event_queue)) {
        al_wait_for_event(event_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return true;
        } else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            return true;
        }
    }
    for (int i = 1; i <= n_celle; i++) {
        for (int j = 0; j < n_celle; j++) {
            int al_x = i - 1;
            int al_y = j;
            if (matrix[i][j])
                al_draw_filled_rectangle(al_x * l_cella, al_y * l_cella, (al_x * l_cella) + l_cella - 1, (al_y * l_cella) + l_cella - 1, al_map_rgb(200, 70, 70));
            else
                al_draw_filled_rectangle(al_x * l_cella, al_y * l_cella, (al_x * l_cella) + l_cella - 1, (al_y * l_cella) + l_cella - 1, al_map_rgb(70, 70, 70));
        }
    }
    al_flip_display();
    return false;
}

#endif
