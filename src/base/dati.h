// DATI: enum e struct con cui il programma rappresenta il gioco.
// Solo tipi, nessuna funzione: per questo non esiste un dati.cpp.
#pragma once

#include <random>   // mt19937
#include <vector>

#include "base/costanti.h"

enum class Seme { CUORI, QUADRI, FIORI, PICCHE };

// Le scelte del giocatore. I numeri del menu esistono solo nella grafica.
enum class Azione { CARTA, STAI, RADDOPPIA, DIVIDI, RESA };

enum class Risultato {
    DOPPIO_BLACKJACK,      // blackjack naturale per entrambi: pareggio
    BLACKJACK_GIOCATORE,   // paga 3:2
    BLACKJACK_BANCO,
    GIOCATORE_SFORA,
    BANCO_SFORA,
    VITTORIA,
    SCONFITTA,
    PAREGGIO,
    RESA                   // il giocatore si arrende: perde metà puntata
};

struct Carta {
    int valore;   // 0 = Asso, 1-9 = carte 2-10, 10 = Fante, 11 = Donna, 12 = Re
    Seme seme;
};

// Una mano del giocatore. Senza split ce n'è una sola, con lo split fino a MAX_MANI.
struct Mano {
    std::vector<Carta> carte;
    int puntata = 0;
    bool daSplit = false;    // nata da uno split: un 21 non è blackjack naturale
    bool conclusa = false;   // ha detto "stai", ha raddoppiato o è un asso diviso
    bool resa = false;
};

// Le carte in tavola. Le fiches stanno solo in Sessione.
struct Tavolo {
    std::vector<Carta> banco;
    std::vector<Mano> mani;
    int manoAttiva = 0;      // indice della mano che sta giocando; -1 = nessuna
    int assicurazione = 0;   // fiches puntate sull'assicurazione (0 = non presa)
    int numeroMano = 1;
};

struct Statistiche {
    int maniGiocate = 0;                   // ogni mano divisa conta a sé
    int maniVinte = 0;
    int maniPerse = 0;
    int maniPareggiate = 0;
    int blackjackNaturali = 0;
    int serieVittorie = 0;                 // vittorie consecutive in corso
    int migliorSerie = 0;
    int fichesMassime = FICHES_INIZIALI;
    int fichesMinime = FICHES_INIZIALI;
};

// Cosa è successo a una mano. La logica restituisce questo, la grafica
// lo trasforma in testo.
struct Esito {
    Risultato risultato = Risultato::PAREGGIO;
    int variazione = 0;   // fiches guadagnate (+) o perse (-)
};

// Esiti di tutto il tavolo: una voce per mano, più l'assicurazione
struct EsitoTavolo {
    std::vector<Esito> mani;
    int assicurazione = 0;   // 0 se non presa
    int totale = 0;          // somma di tutto
};

// Tutto ciò che dura per l'intera partita
struct Sessione {
    std::vector<Carta> mazzo;
    std::mt19937 generatore;
    int fiches = FICHES_INIZIALI;
    Statistiche stat;
};
