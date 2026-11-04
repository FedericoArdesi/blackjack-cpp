// TERMINALE: colori, larghezza del testo, pulizia schermo, pause.
// I colori sono codici ANSI ("\033[31m" = rosso, "\033[0m" = reset).
// Se USA_COLORI è false diventano stringhe vuote.

#include "terminale/terminale.h"
#include "base/costanti.h"

#include <iostream>
#include <string>

// windows.h va incluso dopo le altre librerie: definisce macro (min, max)
// che romperebbero <random>. Qui non serve <random>, e il suo uso resta
// confinato in questo file: gli altri .cpp non vedono mai windows.h.
#ifdef _WIN32
  #include <windows.h>
#else
  #include <thread>
  #include <chrono>
#endif

using namespace std;

// Il header ha già dichiarato questi nomi "extern": qui li definiamo.
const string RESET     = USA_COLORI ? "\033[0m"  : "";
const string GRASSETTO = USA_COLORI ? "\033[1m"  : "";
const string ROSSO     = USA_COLORI ? "\033[31m" : "";
const string VERDE     = USA_COLORI ? "\033[32m" : "";
const string GIALLO    = USA_COLORI ? "\033[33m" : "";
const string CIANO     = USA_COLORI ? "\033[36m" : "";

// Colora il testo e rimette i colori normali alla fine.
// Gli stili si sommano: colora("21!", GRASSETTO + VERDE)
string colora(const string& testo, const string& stile) {
    return stile + testo + RESET;
}

string ripeti(const string& simbolo, int volte) {
    string risultato = "";
    for (int i = 0; i < volte; i++) {
        risultato += simbolo;
    }
    return risultato;
}

// Colonne occupate sullo schermo. size() conta i byte, ma "à" ne pesa 2 e
// "═" ne pesa 3: si conta solo il primo byte di ogni carattere UTF-8
// (ASCII < 128, oppure byte iniziale >= 192). I codici colore (da ESC
// fino alla 'm') non si vedono e si saltano.
int larghezza(const string& testo) {
    int totale = 0;
    bool dentroCodice = false;
    for (char c : testo) {
        unsigned char b = static_cast<unsigned char>(c);
        if (dentroCodice) {
            if (c == 'm') dentroCodice = false;
        } else if (b == 27) {                  // ESC
            dentroCodice = true;
        } else if (b < 128 || b >= 192) {
            totale++;
        }
    }
    return totale;
}

// Cancella lo schermo e porta il cursore in alto a sinistra
void pulisci() {
    cout << "\033[2J\033[3J\033[H" << flush;
}

void pausa(int millisecondi) {
    cout << flush;
#ifdef _WIN32
    Sleep(millisecondi);
#else
    this_thread::sleep_for(chrono::milliseconds(millisecondi));
#endif
}

// Da chiamare all'inizio del main: su Windows attiva UTF-8 e colori ANSI.
void preparaTerminale() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modalita = 0;
    if (GetConsoleMode(console, &modalita)) {
        SetConsoleMode(console, modalita | 0x0004);   // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    }
#endif
}
