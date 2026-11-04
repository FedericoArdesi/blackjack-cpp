// DEMO: il gioco vero, ma con le prime carte decise da noi.
// Serve per provare a mano situazioni rare (split, assicurazione, ...)
// senza aspettare che escano per caso. Non tocca niente in src/: ha un suo
// main() e usa gli stessi file del gioco (tutti tranne src/main.cpp).
//
// Compilare e avviare (dalla cartella del progetto):
//   make demo SCENARIO=split
//
// Scenari: split, assicurazione, blackjack, asso, resa

#include "logica/logica.h"
#include "partita/partita.h"
#include "terminale/terminale.h"

#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

// Le carte in ordine di pesca: giocatore, banco (scoperta), giocatore,
// banco (coperta), poi quelle pescate durante la mano.
// Si scrivono come sulle carte vere: 1 = Asso, 2-10, 11 = J, 12 = Q, 13 = K.
// Finite queste, il mazzo torna casuale. Nome sconosciuto = lista vuota.
static vector<int> scenario(const string& nome) {
    if (nome == "split")         return {8, 6, 8, 10, 3, 13, 10, 9};  // 8+8 contro 6
    if (nome == "assicurazione") return {10, 1, 7, 13};               // banco: asso + K
    if (nome == "blackjack")     return {1, 9, 13, 7};                // blackjack naturale
    if (nome == "asso")          return {1, 9, 5, 7, 13, 5};          // asso + 5
    if (nome == "resa")          return {10, 10, 6, 7};               // 16 contro 10
    return {};
}

int main(int argc, char* argv[]) {
    vector<int> carte;
    if (argc > 1) {
        carte = scenario(argv[1]);
    }
    if (carte.empty()) {
        cout << "Uso: demo <scenario>\n"
             << "Scenari: split, assicurazione, blackjack, asso, resa\n";
        return 1;
    }

    preparaTerminale();
    ios::sync_with_stdio(false);

    Sessione sessione;
    random_device rd;
    sessione.generatore.seed(rd());
    creaMazzo(sessione.mazzo, sessione.generatore);

    // pescaCarta prende l'ultima carta del mazzo: si aggiungono al contrario
    for (int i = static_cast<int>(carte.size()) - 1; i >= 0; i--) {
        Carta c;
        c.valore = carte[i] - 1;   // 1 (Asso) -> 0, ..., 13 (K) -> 12
        c.seme = static_cast<Seme>(i % NUMERO_SEMI);
        sessione.mazzo.push_back(c);
    }

    giocaSessione(sessione);
    return 0;
}