// GRAFICA: disegna lo schermo e legge l'input.
// Non decide le regole e non cambia lo stato di gioco: usa la logica solo
// per leggere (punteggi, blackjack). Le funzioni "static" sono private di
// questo file e non compaiono in grafica.h.

#include "grafica/grafica.h"
#include "base/costanti.h"
#include "logica/logica.h"
#include "terminale/terminale.h"

#include <cstdlib>   // std::exit
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ---------------------------------------------------------------------
//  COSTANTI DEL DISEGNO (private di questo file)
// ---------------------------------------------------------------------

// Grafica
const int LARGHEZZA_BOX = 54;           // larghezza interna dei riquadri
const int ALTEZZA_CARTA = 9;
const int MAX_CARTE_INTERE = 6;         // oltre, le carte si sovrappongono
const string SEPARATORE = "---";        // riga di un riquadro = linea di separazione

// Posizione dei simboli sulle carte numeriche (asso, 2-10).
// 5 righe da 3 posti (sinistra, centro, destra), separate da uno spazio:
// 'x' = simbolo, '.' = vuoto. Si legge con PIP[valore][riga * 4 + colonna].
const string PIP[10] = {
    "... ... .x. ... ...",   // Asso
    ".x. ... ... ... .x.",   // 2
    ".x. ... .x. ... .x.",   // 3
    "x.x ... ... ... x.x",   // 4
    "x.x ... .x. ... x.x",   // 5
    "x.x ... x.x ... x.x",   // 6
    "x.x .x. x.x ... x.x",   // 7
    "x.x .x. x.x .x. x.x",   // 8
    "x.x x.x .x. x.x x.x",   // 9
    "xxx x.x ... x.x xxx"    // 10
};

// Titolo del menu: largo 102 colonne, serve un terminale di almeno 105.
const vector<string> TITOLO = {
    "▀█████████▄   ▄█          ▄████████  ▄████████    ▄█   ▄█▄      ▄█    ▄████████  ▄████████    ▄█   ▄█▄",
    "  ███    ███ ███         ███    ███ ███    ███   ███ ▄███▀     ███   ███    ███ ███    ███   ███ ▄███▀",
    "  ███    ███ ███         ███    ███ ███    █▀    ███▐██▀       ███   ███    ███ ███    █▀    ███▐██▀",
    " ▄███▄▄▄██▀  ███         ███    ███ ███         ▄█████▀        ███   ███    ███ ███         ▄█████▀",
    "▀▀███▀▀▀██▄  ███       ▀███████████ ███        ▀▀█████▄        ███ ▀███████████ ███        ▀▀█████▄",
    "  ███    ██▄ ███         ███    ███ ███    █▄    ███▐██▄       ███   ███    ███ ███    █▄    ███▐██▄",
    "  ███    ███ ███▌    ▄   ███    ███ ███    ███   ███ ▀███▄     ███   ███    ███ ███    ███   ███ ▀███▄",
    "▄█████████▀  █████▄▄██   ███    █▀  ████████▀    ███   ▀█▀ █▄ ▄███   ███    █▀  ████████▀    ███   ▀█▀",
    "             ▀                                   ▀         ▀▀▀▀▀▀                            ▀"
};

// Legge una riga e la converte con convertiInIntero. Ripete finché il
// valore è un numero intero tra minimo e massimo.
int leggiIntero(int minimo, int massimo) {
    string riga;
    while (true) {
        cout << colora("> ", GIALLO);
        if (!getline(cin, riga)) {   // input finito (Ctrl+D / Ctrl+Z)
            cout << "\nInput terminato. Uscita dal programma.\n";
            std::exit(0);
        }

        int numero = 0;
        if (!convertiInIntero(riga, numero)) {
            cout << colora("Input non valido: scrivi solo un numero.", ROSSO) << "\n";
            continue;
        }

        if (numero < minimo || numero > massimo) {
            cout << colora("Scegli un valore tra " + to_string(minimo)
                           + " e " + to_string(massimo) + ".", ROSSO) << "\n";
            continue;
        }
        return numero;
    }
}

// Il valore predefinito del parametro sta nella dichiarazione (grafica.h).
void aspettaInvio(const string& testo) {
    cout << "\n" << colora(testo, GIALLO) << flush;
    string scarto;
    getline(cin, scarto);
}

// Importo con il segno davanti: +20, -10, 0
static string conSegno(int numero) {
    if (numero > 0) {
        return "+" + to_string(numero);
    }
    return to_string(numero);
}

// --- Riquadri ---

// Riga a due colonne: testo a sinistra, valore a destra
static string coppia(const string& sinistra, const string& destra) {
    int spazi = LARGHEZZA_BOX - larghezza(sinistra) - larghezza(destra);
    if (spazi < 0) {
        spazi = 0;
    }
    return sinistra + string(spazi, ' ') + destra;
}

// Riquadro con titolo. Una riga può essere testo o SEPARATORE.
// Con centrato = true il testo sta al centro.
static void stampaBox(const vector<string>& righe, const string& titolo,
               const string& colore, bool centrato = false) {

    int trattini = LARGHEZZA_BOX - 1 - larghezza(titolo);
    cout << colora("╭─ ", colore) << colora(titolo, GRASSETTO + colore)
         << colora(" " + ripeti("─", trattini) + "╮", colore) << "\n";

    for (const string& riga : righe) {
        if (riga == SEPARATORE) {
            cout << colora("├" + ripeti("─", LARGHEZZA_BOX + 2) + "┤", colore) << "\n";
            continue;
        }
        int spazi = LARGHEZZA_BOX - larghezza(riga);
        if (spazi < 0) {
            spazi = 0;
        }
        int davanti = centrato ? spazi / 2 : 0;
        cout << colora("│", colore) << " " << string(davanti, ' ') << riga
             << string(spazi - davanti, ' ') << " " << colora("│", colore) << "\n";
    }

    cout << colora("╰" + ripeti("─", LARGHEZZA_BOX + 2) + "╯", colore) << "\n";
}

static void stampaTitolo() {
    for (const string& riga : TITOLO) {
        cout << colora(riga, CIANO) << "\n";
    }
}

// --- Carte ---

static string nomeValore(int valore) {
    switch (valore) {
        case ASSO:  return "A";
        case FANTE: return "J";
        case DONNA: return "Q";
        case RE:    return "K";
        default:    return to_string(valore + 1);
    }
}

static string simboloSeme(Seme seme) {
    switch (seme) {
        case Seme::CUORI:  return "♥";
        case Seme::QUADRI: return "♦";
        case Seme::FIORI:  return "♣";
        case Seme::PICCHE: return "♠";
    }
    return "";
}

static bool semeRosso(Seme seme) {
    return seme == Seme::CUORI || seme == Seme::QUADRI;
}

// Carta su una riga sola, per i riepiloghi: "A♠", "10♥"
static string testoCarta(const Carta& carta) {
    string testo = nomeValore(carta.valore) + simboloSeme(carta.seme);
    return semeRosso(carta.seme) ? colora(testo, ROSSO) : testo;
}

// Una delle 9 righe del disegno di una carta (11 colonne). Esempio, 4 di Picche:
//   ╭─────────╮   riga 0
//   │4♠       │   riga 1: valore e seme
//   │ ♠     ♠ │   righe 2-6: simboli
//   │         │
//   │         │
//   │         │
//   │ ♠     ♠ │
//   │       ♠4│   riga 7: seme e valore
//   ╰─────────╯   riga 8
// Con compatta = true restituisce solo i primi 4 caratteri della riga, per
// sovrapporre le carte quando la mano è lunga.
static string rigaCarta(const Carta& carta, int riga, bool coperta, bool compatta) {
    if (coperta) {
        if (riga == 0) return colora("╭─────────╮", CIANO);
        if (riga == ALTEZZA_CARTA - 1) return colora("╰─────────╯", CIANO);
        string trama = (riga % 2 == 1) ? "░▒░▒░▒░▒░" : "▒░▒░▒░▒░▒";
        return colora("│" + trama + "│", CIANO);
    }

    string valore = nomeValore(carta.valore);
    string seme = simboloSeme(carta.seme);
    string colore = semeRosso(carta.seme) ? ROSSO : "";
    int lunghezzaValore = static_cast<int>(valore.size());   // "10" è lungo 2

    if (riga == 0) return compatta ? "╭───" : "╭─────────╮";
    if (riga == ALTEZZA_CARTA - 1) return compatta ? "╰───" : "╰─────────╯";

    if (riga == 1) {
        string angolo = colora(valore + seme, colore);
        if (compatta) return "│" + angolo + string(3 - lunghezzaValore - 1, ' ');
        return "│" + angolo + string(9 - lunghezzaValore - 1, ' ') + "│";
    }

    // Il resto di una carta compatta è coperto dalla successiva
    if (compatta) return "│   ";

    if (riga == ALTEZZA_CARTA - 2) {
        return "│" + string(9 - lunghezzaValore - 1, ' ')
             + colora(seme + valore, colore) + "│";
    }

    // Centro: le righe 2-6 diventano p = 0..4
    int p = riga - 2;
    string contenuto;

    if (carta.valore >= FANTE) {
        if (p == 1)      contenuto = "  ╭───╮  ";
        else if (p == 2) contenuto = "  │" + colora(seme + valore + seme, colore) + "│  ";
        else if (p == 3) contenuto = "  ╰───╯  ";
        else             contenuto = "         ";
    } else {
        string simbolo = colora(seme, colore);
        contenuto = "  ";
        for (int colonna = 0; colonna < 3; colonna++) {
            if (PIP[carta.valore][p * 4 + colonna] == 'x') {
                contenuto += simbolo;
            } else {
                contenuto += " ";
            }
            if (colonna < 2) {
                contenuto += " ";
            }
        }
        contenuto += "  ";
    }
    return "│" + contenuto + "│";
}

// "(punteggio: 18)", verde con 21 e rosso se sfora. Il blackjack lo decide
// chi chiama, perché dopo uno split un 21 non è un blackjack.
static string testoPunteggio(const vector<Carta>& carte, bool blackjack) {
    string coloreP = "";
    if (haSforato(carte)) {
        coloreP = ROSSO;
    } else if (haVentuno(carte)) {
        coloreP = VERDE;
    }
    string testo = "(punteggio: " + colora(to_string(calcolaPunteggio(carte)), coloreP) + ")";
    if (blackjack) {
        testo += colora("  BLACKJACK!", GIALLO);
    } else if (haSforato(carte)) {
        testo += colora("  SFORATO", ROSSO);
    }
    return testo;
}

// Mano con le carte affiancate. Con nascondiSeconda la seconda è coperta
// (carta del banco).
static void stampaMano(const string& titolo, const vector<Carta>& mano,
                bool nascondiSeconda, bool blackjack) {
    cout << colora(titolo, GRASSETTO) << "  ";
    if (nascondiSeconda && mano.size() >= 2) {
        cout << "(punteggio: " << valoreCarta(mano[0]) << " + ?)";
    } else {
        cout << testoPunteggio(mano, blackjack);
    }
    cout << "\n";

    int numeroCarte = static_cast<int>(mano.size());
    bool sovrapponi = numeroCarte > MAX_CARTE_INTERE;   // l'ultima resta sempre intera

    // Una riga alla volta, attraverso tutte le carte
    for (int riga = 0; riga < ALTEZZA_CARTA; riga++) {
        cout << "  ";
        for (int i = 0; i < numeroCarte; i++) {
            bool coperta = (i == 1 && nascondiSeconda);
            bool compatta = sovrapponi && i < numeroCarte - 1;
            cout << rigaCarta(mano[i], riga, coperta, compatta);
            if (!compatta) {
                cout << " ";
            }
        }
        cout << "\n";
    }
}

// Una mano su una riga sola: serve quando le mani sono più d'una, così
// il tavolo non diventa più alto del terminale.
static void stampaRiepilogoMano(const string& titolo, const Mano& mano) {
    cout << colora(titolo, GRASSETTO) << "  ";
    for (const Carta& carta : mano.carte) {
        cout << testoCarta(carta) << " ";
    }
    cout << testoPunteggio(mano.carte, blackjackGiocatore(mano))
         << "  puntata " << mano.puntata << "\n";
}

// --- Schermate ---

// Barra in alto durante la mano
static void stampaBarraStato(const Sessione& sessione, const Tavolo& tavolo) {
    cout << " " << colora("FICHES " + to_string(sessione.fiches), GIALLO)
         << "     PUNTATA " << puntataTotale(tavolo)
         << "     MANO " << tavolo.numeroMano
         << "     CARTE " << sessione.mazzo.size() << "\n";
    cout << colora(ripeti("─", LARGHEZZA_BOX + 4), CIANO) << "\n";
}

// Pulisce e ridisegna tutto il tavolo. Chiamata a ogni cambiamento:
// le carte restano ferme e, con una pausa in mezzo, sembra un'animazione.
// Con una mano sola è disegnata per intero. Con più mani solo quella attiva
// è per intero, le altre sono riepiloghi su una riga (a mani finite, tutte).
void disegnaTavolo(const Sessione& sessione, const Tavolo& tavolo,
                   bool nascondiBanco, const string& messaggio) {
    pulisci();
    stampaBarraStato(sessione, tavolo);
    stampaMano("BANCO", tavolo.banco, nascondiBanco, haBlackjack(tavolo.banco));

    int numeroMani = static_cast<int>(tavolo.mani.size());
    for (int i = 0; i < numeroMani; i++) {
        const Mano& mano = tavolo.mani[i];
        if (numeroMani == 1) {
            stampaMano("TU", mano.carte, false, blackjackGiocatore(mano));
        } else if (i == tavolo.manoAttiva) {
            string titolo = "MANO " + to_string(i + 1) + "/" + to_string(numeroMani)
                          + " (puntata " + to_string(mano.puntata) + ")";
            stampaMano(titolo, mano.carte, false, blackjackGiocatore(mano));
        } else {
            stampaRiepilogoMano("Mano " + to_string(i + 1) + "/" + to_string(numeroMani), mano);
        }
    }

    if (!messaggio.empty()) {
        cout << messaggio << "\n";
    }
}

static string testoEsito(Risultato risultato) {
    switch (risultato) {
        case Risultato::DOPPIO_BLACKJACK:    return "Blackjack per entrambi: pareggio";
        case Risultato::BLACKJACK_GIOCATORE: return "BLACKJACK! Paga 3:2";
        case Risultato::BLACKJACK_BANCO:     return "Il banco ha un blackjack";
        case Risultato::GIOCATORE_SFORA:     return "Hai sforato";
        case Risultato::BANCO_SFORA:         return "Il banco sfora! Hai vinto";
        case Risultato::VITTORIA:            return "Hai vinto!";
        case Risultato::SCONFITTA:           return "Hai perso";
        case Risultato::PAREGGIO:            return "Pareggio: puntata restituita";
        case Risultato::RESA:                return "Ti sei arreso: perdi metà puntata";
    }
    return "";
}

// Verde se vinci, rosso se perdi, giallo se pareggi (sul totale del tavolo).
// Una mano sola senza assicurazione: messaggio e fiches. Altrimenti una riga
// per mano, l'assicurazione se presa e il totale.
void stampaEsito(const EsitoTavolo& esiti) {
    string colore = GIALLO;
    if (esiti.totale > 0) {
        colore = VERDE;
    } else if (esiti.totale < 0) {
        colore = ROSSO;
    }

    int numeroMani = static_cast<int>(esiti.mani.size());

    if (numeroMani == 1 && esiti.assicurazione == 0) {
        string messaggio = testoEsito(esiti.mani[0].risultato);
        if (esiti.totale == 0) {
            stampaBox({messaggio}, "ESITO", colore, true);
        } else {
            stampaBox({messaggio, SEPARATORE, coppia("Fiches:", conSegno(esiti.totale))},
                      "ESITO", colore, true);
        }
        return;
    }

    vector<string> righe;
    for (int i = 0; i < numeroMani; i++) {
        string etichetta = testoEsito(esiti.mani[i].risultato);
        if (numeroMani > 1) {
            etichetta = "Mano " + to_string(i + 1) + ": " + etichetta;
        }
        righe.push_back(coppia(etichetta, conSegno(esiti.mani[i].variazione)));
    }
    if (esiti.assicurazione != 0) {
        righe.push_back(coppia("Assicurazione:", conSegno(esiti.assicurazione)));
    }
    righe.push_back(SEPARATORE);
    righe.push_back(coppia("Totale fiches:", conSegno(esiti.totale)));
    stampaBox(righe, "ESITO", colore);
}

static void stampaHUD(int fiches, const Statistiche& stat) {
    stampaBox({
        coppia("Fiches:", colora(to_string(fiches), GIALLO)),
        coppia("Mani:", to_string(stat.maniGiocate)),
        coppia("Serie:", to_string(stat.serieVittorie))
    }, "STATO", GIALLO);
}

static void stampaStatistiche(const Statistiche& stat, int fiches) {
    vector<string> righe;
    righe.push_back(coppia("Mani giocate:", to_string(stat.maniGiocate)));
    righe.push_back(coppia("Mani vinte:", to_string(stat.maniVinte)));
    righe.push_back(coppia("Mani perse:", to_string(stat.maniPerse)));
    righe.push_back(coppia("Mani pareggiate:", to_string(stat.maniPareggiate)));
    righe.push_back(coppia("Blackjack naturali:", to_string(stat.blackjackNaturali)));
    righe.push_back(coppia("Migliore serie:", to_string(stat.migliorSerie)));

    if (stat.maniGiocate > 0) {
        // Percentuale con un decimale usando solo interi: si calcola in
        // decimi di punto (moltiplicando prima di dividere), poi si separano
        // parte intera e decimale.
        int decimi = stat.maniVinte * 1000 / stat.maniGiocate;
        righe.push_back(coppia("Percentuale vittorie:",
                               to_string(decimi / 10) + "." + to_string(decimi % 10) + "%"));
    }

    righe.push_back(coppia("Fiches massime:", to_string(stat.fichesMassime)));
    righe.push_back(coppia("Fiches minime:", to_string(stat.fichesMinime)));
    righe.push_back(coppia("Fiches attuali:", to_string(fiches)));
    righe.push_back(SEPARATORE);

    // Bilancio rispetto alla partenza
    int bilancio = fiches - FICHES_INIZIALI;
    string testoBilancio = "0";
    if (bilancio > 0) {
        testoBilancio = colora("+" + to_string(bilancio), VERDE);
    } else if (bilancio < 0) {
        testoBilancio = colora(to_string(bilancio), ROSSO);
    }
    righe.push_back(coppia("Bilancio:", testoBilancio));

    stampaBox(righe, "STATISTICHE", CIANO);
}

static void stampaRegole() {
    stampaBox({
        "Vince chi si avvicina di più a 21 senza superarlo.",
        "Asso = 1 o 11, figure = 10, le altre il loro numero.",
        "Il banco pesca finché ha meno di " + to_string(SOGLIA_BANCO) + ";",
        "sta su tutti i 17, anche \"soft\" (asso + 6).",
        "Si gioca con " + to_string(NUMERO_MAZZI) + " mazzi.",
        SEPARATORE,
        "Puntate pari: minimo " + to_string(PUNTATA_MINIMA) + ", nessun massimo.",
        "Vittoria: paghi 1:1.",
        "Blackjack naturale (21 con due carte): 3:2.",
        "Pareggio: la puntata ti viene restituita.",
        SEPARATORE,
        "Raddoppia: puntata x2, una sola carta, poi ti fermi.",
        "Dividi: due carte di pari valore (anche K e Q) fanno",
        "due mani con la stessa puntata (al massimo "
            + to_string(MAX_MANI) + " mani).",
        "Assi divisi: una carta per mano, niente nuovo split.",
        "Dopo uno split un 21 non è blackjack: paga 1:1.",
        "Arrenditi: perdi metà puntata e la mano finisce.",
        "Non si può farlo dopo aver diviso.",
        "Raddoppia, dividi e arrenditi: solo con due carte;",
        "per raddoppiare e dividere servono altre fiches.",
        SEPARATORE,
        "Assicurazione: se il banco mostra un asso puoi",
        "puntare metà puntata che abbia blackjack: paga 2:1.",
        SEPARATORE,
        "Se non hai fiches per la puntata minima, è finita."
    }, "REGOLE", CIANO);
}

void schermataMenu(int fiches, const Statistiche& stat) {
    pulisci();
    stampaTitolo();
    cout << "\n";
    stampaHUD(fiches, stat);
    cout << "\n[1] Gioca\n"
         << "[2] Statistiche\n"
         << "[3] Regole\n"
         << "[0] Esci\n";
}

void schermataStatistiche(const Statistiche& stat, int fiches) {
    pulisci();
    stampaStatistiche(stat, fiches);
    aspettaInvio("Premi Invio per tornare al menu...");
}

void schermataRegole() {
    pulisci();
    stampaRegole();
    aspettaInvio("Premi Invio per tornare al menu...");
}

void schermataFinale(const Statistiche& stat, int fiches, bool gameOver) {
    pulisci();
    if (gameOver) {
        stampaBox({"Non hai fiches per la puntata minima."}, "GAME OVER", ROSSO, true);
    } else {
        stampaBox({"Alla prossima!"}, "ARRIVEDERCI", CIANO, true);
    }
    cout << "\n";
    stampaStatistiche(stat, fiches);
    cout << "\nGrazie per aver giocato!\n";
}

// Puntata pari da PUNTATA_MINIMA a tutte le fiches. Restituisce 0 se il
// giocatore preferisce tornare al menu.
int chiediPuntata(int fiches, const Statistiche& stat) {
    int massimo = puntataMassima(fiches);

    pulisci();
    stampaHUD(fiches, stat);
    cout << "\n";
    stampaBox({
        "Quante fiches vuoi puntare?",
        "Solo numeri pari, da " + to_string(PUNTATA_MINIMA) + " a " + to_string(massimo) + ".",
        "[0] Torna al menu"
    }, "PUNTATA", CIANO);

    while (true) {
        int puntata = leggiIntero(0, massimo);
        if (puntata == 0 || puntataValida(puntata, fiches)) {
            return puntata;
        }
        cout << colora("La puntata deve essere un numero pari (minimo "
                       + to_string(PUNTATA_MINIMA) + ").", ROSSO) << "\n";
    }
}

static string nomeAzione(Azione azione) {
    switch (azione) {
        case Azione::CARTA:     return "Carta";
        case Azione::STAI:      return "Stai";
        case Azione::RADDOPPIA: return "Raddoppia";
        case Azione::DIVIDI:    return "Dividi";
        case Azione::RESA:      return "Arrenditi";
    }
    return "";
}

// Mostra le azioni disponibili numerate da 1 e restituisce quella scelta
Azione chiediAzione(const vector<Azione>& azioni) {
    for (size_t i = 0; i < azioni.size(); i++) {
        cout << (i > 0 ? "   " : "") << "[" << i + 1 << "] " << nomeAzione(azioni[i]);
    }
    cout << "\n";
    int scelta = leggiIntero(1, static_cast<int>(azioni.size()));
    return azioni[scelta - 1];
}

// Il tavolo è già disegnato con l'asso del banco: si chiede solo se assicurarsi
bool chiediAssicurazione(int importo) {
    cout << "\nAssicurazione: " << importo
         << " fiches, paga 2:1 se il banco ha blackjack.\n"
         << "[1] Assicurati   [2] No\n";
    return leggiIntero(1, 2) == 1;
}
