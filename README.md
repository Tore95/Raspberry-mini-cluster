# Il Gioco della Vita
## Cluster di RaspBerryPi

This is a project for "Parallel Algorithms and Distributed Systems". It's a RaspberryPi mini-cluster that runs a small simulation program using the MPI parallelization library for C++ called "Game of Life"
Questo è un proggetto universitario per "Algoritmi Paralleli e Sistemi Distribuiti". Si tratta di un mini-cluster di RaspBerryPi che fa girare un piccolo programma di simulazione chiamato "Il gioco della Vita", utilizzando la libreria di parallelizzazione per C++ MPI.

![Immaggine di copertina](https://firebasestorage.googleapis.com/v0/b/personal-ee8a6.appspot.com/o/mini-cluster.png?alt=media&token=20e8cb2b-92d2-4b03-bded-fe30921af726)

## Il gioco della Vita:
L’automa cellulare che abbiamo realizzato è la versione standard del gioco della vita realizzato
seguendo le seguenti regole di vicinato:

-Qualsiasi cellula viva con meno di due cellule vive adiacenti muore, come per effetto d'iso-
lamento
-Qualsiasi cellula viva con due o tre cellule vive adiacenti sopravvive alla generazione suc-
cessiva
-Qualsiasi cellula viva con più di tre cellule vive adiacenti muore, come per effetto di sovrap-
popolazione
-Qualsiasi cellula morta con esattamente tre cellule vive adiacenti diventa una cellula viva,
come per effetto di riproduzione

(il vicinato è calcolato sulle otto posizioni adiacenti alla cellula)

## Il Codice:
Per lo sviluppo dell’automa sono state utilizzate le seguenti librerie:
- MPICH 3
- Allegro 5

Il programma è stato scritto ad oggetti utilizzando il linguaggio C++ standard 11 e compilato con le
librerie allegro, sono stati creati solo due oggetti:
- Master.h
- Slaves.h

Ed un Main:
- Main.cpp

In seguito, compilati con il compilatore per C++ di mpich più il link alle librerie di allegro:
```shell
mpic++ <Nome_Codice.cpp> -o <Nome_Output.out> -lallegro -lallegro_primitives
```

## Il comportamento del Main:
Il Main si preoccupa di generare le corrette istanze per ogni processo, darà di default al processo “0” l’istanza del master e a tutti gli altri quella degli slave.
```c++
if (id == 0) {
  start_time = MPI_Wtime();
}
```
