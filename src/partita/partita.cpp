// PARTITA: collega grafica e logica.
// La grafica disegna e legge la scelta, la logica dice cosa è permesso
// e cosa succede. Le funzioni "static" sono private di questo file:
// fuori serve solo giocaSessione (dichiarata in partita.h).

#include "partita/partita.h"
#include "base/costanti.h"
#include "grafica/grafica.h"
#include "logica/logica.h"
#include "terminale/terminale.h"

#include <iostream>
#include <string>

using namespace std;

// Fa giocare UNA mano del giocatore, finché è finita.
// A ogni giro del ciclo:
//   1. rilegge la mano attiva
//   2. se la mano è finita (resa, conclusa, sforata) esce
//   3. se ha 21 si ferma da sola ed esce
//   4. disegna il tavolo
//   5. chiede l'azione: la logica dice cosa è permesso, la grafica lo mostra (riga 42)
//   6. applica l'azione con uno switch
// Carta e Dividi continuano il ciclo; Stai, Raddoppia e Resa escono.
static void giocaManoAttiva(Sessione& sessione, Tavolo& tavolo, string& messaggio) {
    while (true) {
        // La mano va letta ogni volta: lo split cambia il vettore
        Mano& mano = tavolo.mani[tavolo.manoAttiva];

        if (mano.resa || mano.conclusa || haSforato(mano.carte)) {
            return;
        }
        if (haVentuno(mano.carte)) {
            disegnaTavolo(sessione, tavolo, true,
                          colora("21! Ti fermi automaticamente.", VERDE));
            pausa(PAUSA_ANIMAZIONI);
            return;
        }

        disegnaTavolo(sessione, tavolo, true, messaggio);
        Azione azione = chiediAzione(azioniDisponibili(tavolo, sessione.fiches));

        switch (azione) {
            case Azione::CARTA:
                mano.carte.push_back(pescaCarta(sessione));
                messaggio = "Hai pescato una carta. Tocca ancora a te.";
                break;
            case Azione::STAI:
                return;
            case Azione::RADDOPPIA:
                mano.puntata *= 2;
                mano.carte.push_back(pescaCarta(sessione));
                mano.conclusa = true;
                disegnaTavolo(sessione, tavolo, true,
                              colora("Raddoppio! Una sola carta.", GIALLO));
                pausa(PAUSA_ANIMAZIONI);
                return;
            case Azione::DIVIDI: {
                dividiMano(tavolo);   // da qui "mano" non vale più
                int i = tavolo.manoAttiva;
                tavolo.mani[i].carte.push_back(pescaCarta(sessione));
                disegnaTavolo(sessione, tavolo, true, "Dividi: una carta alla prima mano...");
                pausa(PAUSA_ANIMAZIONI);
                tavolo.mani[i + 1].carte.push_back(pescaCarta(sessione));
                disegnaTavolo(sessione, tavolo, true, "Dividi: una carta alla seconda mano...");
                pausa(PAUSA_ANIMAZIONI);

                if (tavolo.mani[i].conclusa) {
                    messaggio = colora("Assi divisi: una sola carta per mano.", GIALLO);
                    disegnaTavolo(sessione, tavolo, true, messaggio);
                    pausa(PAUSA_ANIMAZIONI);
                } else {
                    messaggio = "Hai diviso la coppia. Giochi la mano " + to_string(i + 1) + ".";
                }
                break;
            }
            case Azione::RESA:
                mano.resa = true;
                return;
        }
    }
}

// Turno del giocatore: le mani una dopo l'altra (anche quelle nate da split
// durante il turno). Restituisce true se il banco deve giocare.
static bool turnoGiocatore(Sessione& sessione, Tavolo& tavolo, const string& messaggioIniziale) {
    string messaggio = messaggioIniziale;
    tavolo.manoAttiva = 0;

    while (tavolo.manoAttiva < static_cast<int>(tavolo.mani.size())) {
        giocaManoAttiva(sessione, tavolo, messaggio);
        tavolo.manoAttiva++;
        if (tavolo.manoAttiva < static_cast<int>(tavolo.mani.size())) {
            messaggio = "Tocca alla mano " + to_string(tavolo.manoAttiva + 1) + ".";
        }
    }

    tavolo.manoAttiva = -1;   // nessuna mano attiva: il tavolo le mostra tutte in riepilogo
    return bancoDeveGiocare(tavolo);
}

// Il banco scopre la carta coperta e pesca finché la logica lo richiede
static void turnoBanco(Sessione& sessione, Tavolo& tavolo) {
    disegnaTavolo(sessione, tavolo, false, "Il banco scopre la carta coperta...");
    pausa(PAUSA_ANIMAZIONI);

    while (bancoDevePescare(tavolo)) {
        tavolo.banco.push_back(pescaCarta(sessione));
        disegnaTavolo(sessione, tavolo, false, "Il banco pesca una carta...");
        pausa(PAUSA_ANIMAZIONI);
    }
}

// Una mano: puntata, distribuzione, assicurazione, turno del giocatore,
// turno del banco, esito. Restituisce false se il giocatore torna al menu
// senza giocare.
static bool giocaMano(Sessione& sessione) {
    int puntata = chiediPuntata(sessione.fiches, sessione.stat);
    if (puntata == 0) {
        return false;
    }

    Tavolo tavolo;
    tavolo.numeroMano = sessione.stat.maniGiocate + 1;
    Mano prima;
    prima.puntata = puntata;
    tavolo.mani.push_back(prima);

    // L'avviso va nel messaggio del tavolo, altrimenti la pulizia lo cancella
    string avviso = "";
    if (mazzoDaRimescolare(sessione.mazzo)) {
        creaMazzo(sessione.mazzo, sessione.generatore);
        avviso = "(Mazzo rimescolato) ";
    }

    // Distribuzione alternata: giocatore, banco, giocatore, banco
    for (int i = 0; i < CARTE_INIZIALI * 2; i++) {
        if (i % 2 == 0) {
            tavolo.mani[0].carte.push_back(pescaCarta(sessione));
        } else {
            tavolo.banco.push_back(pescaCarta(sessione));
        }
        disegnaTavolo(sessione, tavolo, true, avviso + "Distribuzione delle carte...");
        pausa(PAUSA_ANIMAZIONI / 2);
    }

    // Il banco mostra un asso: si offre l'assicurazione, prima di tutto il resto
    string messaggio = avviso + "Tocca a te.";
    if (assicurazioneDisponibile(tavolo, sessione.fiches)) {
        disegnaTavolo(sessione, tavolo, true, avviso + "Il banco mostra un asso.");
        if (chiediAssicurazione(importoAssicurazione(tavolo))) {
            tavolo.assicurazione = importoAssicurazione(tavolo);
            if (!haBlackjack(tavolo.banco)) {
                messaggio = "Il banco non ha blackjack: assicurazione persa. Tocca a te.";
            }
        }
    }

    // Con un blackjack naturale non si gioca. Se tutte le mani sforano o il
    // giocatore si arrende, il banco non gioca.
    bool naturali = blackjackGiocatore(tavolo.mani[0]) || haBlackjack(tavolo.banco);
    if (!naturali) {
        bool bancoGioca = turnoGiocatore(sessione, tavolo, messaggio);
        if (bancoGioca) {
            turnoBanco(sessione, tavolo);
        }
    }

    EsitoTavolo esiti = risolviTavolo(tavolo);
    applicaEsito(sessione, esiti);
    disegnaTavolo(sessione, tavolo, false, "");
    stampaEsito(esiti);
    return true;
}

// Più mani di fila, senza tornare ogni volta al menu
void giocaSessione(Sessione& sessione) {
    while (true) {
        if (!giocaMano(sessione)) {
            return;   // puntata 0: torna al menu
        }

        if (fichesInsufficienti(sessione.fiches)) {
            cout << colora("Non hai fiches per la puntata minima.", ROSSO) << "\n";
            aspettaInvio();
            return;   // il game over lo mostra il main
        }

        cout << "[1] Prossima mano   [0] Menu principale\n";
        if (leggiIntero(0, 1) == 0) {
            return;
        }
    }
}
