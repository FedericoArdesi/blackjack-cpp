// TERMINALE: colori, larghezza del testo, pulizia schermo, pause.
// Nessuna regola di gioco. Dipende solo da costanti.h (in terminale.cpp).
#pragma once

#include <string>

// Codici ANSI per i colori. Sono stringhe vuote se USA_COLORI è false.
// "extern" = qui si dichiara soltanto, la definizione sta in terminale.cpp.
extern const std::string RESET;
extern const std::string GRASSETTO;
extern const std::string ROSSO;
extern const std::string VERDE;
extern const std::string GIALLO;
extern const std::string CIANO;

// Colora il testo e rimette i colori normali alla fine.
std::string colora(const std::string& testo, const std::string& stile);

// Ripete un simbolo "volte" volte.
std::string ripeti(const std::string& simbolo, int volte);

// Colonne occupate sullo schermo (UTF-8 e codici colore compresi).
int larghezza(const std::string& testo);

// Cancella lo schermo e porta il cursore in alto a sinistra.
void pulisci();

// Svuota il buffer e aspetta.
void pausa(int millisecondi);

// Da chiamare all'inizio del main: su Windows attiva UTF-8 e colori ANSI.
void preparaTerminale();
