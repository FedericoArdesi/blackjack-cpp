// Blackjack da terminale: punto d'ingresso.
// Il programma è diviso in strati (vedi README.md e docs/diagrammi.html):
// base, terminale, logica, grafica, partita.

#include "grafica/grafica.h"
#include "logica/logica.h"
#include "partita/partita.h"
#include "terminale/terminale.h"

#include <iostream>
#include <random>

using namespace std;

int main() {
    preparaTerminale();

    // Cout più veloce e meno sfarfallio. Prima di ogni lettura da cin il
    // buffer di cout viene svuotato da solo.
    ios::sync_with_stdio(false);

    Sessione sessione;

    random_device rd;
    sessione.generatore.seed(rd());
    creaMazzo(sessione.mazzo, sessione.generatore);

    int scelta;
    do {
        schermataMenu(sessione.fiches, sessione.stat);
        scelta = leggiIntero(0, 3);

        switch (scelta) {
            case 1:
                giocaSessione(sessione);
                break;
            case 2:
                schermataStatistiche(sessione.stat, sessione.fiches);
                break;
            case 3:
                schermataRegole();
                break;
            case 0:
                break;
        }

        if (fichesInsufficienti(sessione.fiches)) {
            schermataFinale(sessione.stat, sessione.fiches, true);
            scelta = 0;
        } else if (scelta == 0) {
            schermataFinale(sessione.stat, sessione.fiches, false);
        }
    } while (scelta != 0);

    return 0;
}
