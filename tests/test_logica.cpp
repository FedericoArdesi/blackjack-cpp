// Test della logica del gioco (src/logica). Nessuna libreria esterna:
// un piccolo CHECK che stampa i fallimenti e fa uscire il programma con codice 1.
//
//   make test

#include "logica/logica.h"

#include <iostream>
#include <vector>

using namespace std;

static int fallimenti = 0;
static int controlli = 0;

#define CHECK(condizione)                                                      \
    do {                                                                       \
        controlli++;                                                           \
        if (!(condizione)) {                                                   \
            fallimenti++;                                                      \
            cerr << "FALLITO " << __FILE__ << ":" << __LINE__ << "  "          \
                 << #condizione << "\n";                                       \
        }                                                                      \
    } while (0)

// Crea una carta come sulle carte vere: 1 = Asso, 2-10, 11 = J, 12 = Q, 13 = K
static Carta carta(int numero) {
    Carta c;
    c.valore = numero - 1;
    c.seme = Seme::CUORI;
    return c;
}

static vector<Carta> mano(initializer_list<int> numeri) {
    vector<Carta> carte;
    for (int n : numeri) carte.push_back(carta(n));
    return carte;
}

// Tavolo con una mano del giocatore e il banco
static Tavolo tavolo(initializer_list<int> giocatore, initializer_list<int> banco, int puntata = 10) {
    Tavolo t;
    Mano m;
    m.carte = mano(giocatore);
    m.puntata = puntata;
    t.mani.push_back(m);
    t.banco = mano(banco);
    return t;
}

static void testConversione() {
    int n = 0;
    CHECK(convertiInIntero("42", n) && n == 42);
    CHECK(convertiInIntero("  7 ", n) && n == 7);
    CHECK(!convertiInIntero("", n));
    CHECK(!convertiInIntero("12abc", n));
    CHECK(!convertiInIntero("3.5", n));
    CHECK(!convertiInIntero("-4", n));
    CHECK(!convertiInIntero("1234567890", n));   // troppo lungo per un int
}

static void testPunteggi() {
    CHECK(calcolaPunteggio(mano({10, 7})) == 17);
    CHECK(calcolaPunteggio(mano({13, 12, 11})) == 30);   // K+Q+J
    CHECK(calcolaPunteggio(mano({1, 9})) == 20);         // asso come 11
    CHECK(calcolaPunteggio(mano({1, 9, 5})) == 15);      // asso come 1
    CHECK(calcolaPunteggio(mano({1, 1, 9})) == 21);      // un asso 11, uno 1
    CHECK(calcolaPunteggio(mano({1, 1, 1, 1})) == 14);

    CHECK(haBlackjack(mano({1, 13})));
    CHECK(!haBlackjack(mano({7, 7, 7})));   // 21 con tre carte
    CHECK(haVentuno(mano({7, 7, 7})));
    CHECK(haSforato(mano({10, 6, 8})));
    CHECK(!haSforato(mano({10, 11})));

    Mano divisa;
    divisa.carte = mano({1, 13});
    divisa.daSplit = true;
    CHECK(!blackjackGiocatore(divisa));   // dopo uno split non e' naturale
    divisa.daSplit = false;
    CHECK(blackjackGiocatore(divisa));
}

static void testPuntate() {
    CHECK(puntataValida(2, 100));
    CHECK(puntataValida(100, 100));
    CHECK(!puntataValida(0, 100));
    CHECK(!puntataValida(3, 100));      // dispari
    CHECK(!puntataValida(102, 100));    // piu' delle fiches
    CHECK(puntataMassima(101) == 100);
    CHECK(puntataMassima(100) == 100);
    CHECK(fichesInsufficienti(1));
    CHECK(!fichesInsufficienti(2));
}

static void testAzioni() {
    // Con due carte uguali e fiches sufficienti c'e' tutto
    Tavolo t = tavolo({8, 8}, {6, 10});
    vector<Azione> a = azioniDisponibili(t, 100);
    auto presente = [&](Azione x) {
        for (Azione y : a) if (y == x) return true;
        return false;
    };
    CHECK(presente(Azione::CARTA) && presente(Azione::STAI));
    CHECK(presente(Azione::RADDOPPIA));
    CHECK(presente(Azione::DIVIDI));
    CHECK(presente(Azione::RESA));

    // Senza fiches libere niente raddoppio ne' split
    a = azioniDisponibili(t, 10);
    CHECK(!presente(Azione::RADDOPPIA));
    CHECK(!presente(Azione::DIVIDI));

    // Con tre carte restano solo carta e stai
    Tavolo t3 = tavolo({2, 3, 4}, {6, 10});
    a = azioniDisponibili(t3, 100);
    CHECK(a.size() == 2);

    // K+Q valgono uguale: lo split e' permesso
    Tavolo kq = tavolo({13, 12}, {6, 10});
    a = azioniDisponibili(kq, 100);
    CHECK(presente(Azione::DIVIDI));
}

static void testSplit() {
    Tavolo t = tavolo({8, 8}, {6, 10}, 10);
    dividiMano(t);
    CHECK(t.mani.size() == 2);
    CHECK(t.mani[0].carte.size() == 1 && t.mani[1].carte.size() == 1);
    CHECK(t.mani[0].puntata == 10 && t.mani[1].puntata == 10);
    CHECK(t.mani[0].daSplit && t.mani[1].daSplit);
    CHECK(!t.mani[0].conclusa);

    // Assi divisi: una carta sola e mano conclusa
    Tavolo assi = tavolo({1, 1}, {6, 10}, 10);
    dividiMano(assi);
    CHECK(assi.mani[0].conclusa && assi.mani[1].conclusa);

    // Resa non disponibile dopo lo split
    t.manoAttiva = 0;
    t.mani[0].carte.push_back(carta(3));
    t.mani[1].carte.push_back(carta(3));
    for (Azione x : azioniDisponibili(t, 100)) CHECK(x != Azione::RESA);
}

static void testBanco() {
    CHECK(bancoDevePescare(tavolo({10, 8}, {10, 6})));
    CHECK(!bancoDevePescare(tavolo({10, 8}, {10, 7})));          // sta su 17
    CHECK(!bancoDevePescare(tavolo({10, 8}, {1, 6})));           // 17 soft: sta
    CHECK(bancoDeveGiocare(tavolo({10, 8}, {10, 6})));

    Tavolo sforato = tavolo({10, 6, 8}, {10, 6});
    CHECK(!bancoDeveGiocare(sforato));       // il giocatore ha sforato

    Tavolo resa = tavolo({10, 6}, {10, 6});
    resa.mani[0].resa = true;
    CHECK(!bancoDeveGiocare(resa));
}

static void testAssicurazione() {
    Tavolo t = tavolo({10, 8}, {1, 13}, 10);
    CHECK(importoAssicurazione(t) == 5);
    CHECK(assicurazioneDisponibile(t, 100));
    CHECK(!assicurazioneDisponibile(t, 10));   // niente fiches libere

    CHECK(risolviAssicurazione(t) == 0);       // non presa
    t.assicurazione = 5;
    CHECK(risolviAssicurazione(t) == 10);      // banco ha blackjack: 2:1

    Tavolo senza = tavolo({10, 8}, {1, 6}, 10);
    senza.assicurazione = 5;
    CHECK(risolviAssicurazione(senza) == -5);

    CHECK(!assicurazioneDisponibile(tavolo({10, 8}, {10, 6}, 10), 100));   // banco senza asso
}

static void testEsiti() {
    auto esito = [](Tavolo t) { return risolviMano(t, t.mani[0]); };

    Esito e = esito(tavolo({10, 9}, {10, 8}, 10));
    CHECK(e.risultato == Risultato::VITTORIA && e.variazione == 10);

    e = esito(tavolo({10, 7}, {10, 8}, 10));
    CHECK(e.risultato == Risultato::SCONFITTA && e.variazione == -10);

    e = esito(tavolo({10, 8}, {10, 8}, 10));
    CHECK(e.risultato == Risultato::PAREGGIO && e.variazione == 0);

    e = esito(tavolo({1, 13}, {10, 8}, 10));
    CHECK(e.risultato == Risultato::BLACKJACK_GIOCATORE && e.variazione == 15);   // 3:2

    e = esito(tavolo({10, 8}, {1, 13}, 10));
    CHECK(e.risultato == Risultato::BLACKJACK_BANCO && e.variazione == -10);

    e = esito(tavolo({1, 13}, {1, 12}, 10));
    CHECK(e.risultato == Risultato::DOPPIO_BLACKJACK && e.variazione == 0);

    e = esito(tavolo({10, 6, 8}, {10, 8}, 10));
    CHECK(e.risultato == Risultato::GIOCATORE_SFORA && e.variazione == -10);

    e = esito(tavolo({10, 8}, {10, 6, 8}, 10));
    CHECK(e.risultato == Risultato::BANCO_SFORA && e.variazione == 10);

    Tavolo resa = tavolo({10, 6}, {10, 10}, 10);
    resa.mani[0].resa = true;
    e = esito(resa);
    CHECK(e.risultato == Risultato::RESA && e.variazione == -5);

    // 21 dopo uno split: vince alla pari, non paga 3:2
    Tavolo split = tavolo({1, 13}, {10, 8}, 10);
    split.mani[0].daSplit = true;
    e = esito(split);
    CHECK(e.risultato == Risultato::VITTORIA && e.variazione == 10);

    // Totale del tavolo: due mani + assicurazione
    Tavolo due = tavolo({10, 9}, {1, 13}, 10);   // banco ha blackjack
    Mano seconda;
    seconda.carte = mano({10, 10});
    seconda.puntata = 10;
    due.mani.push_back(seconda);
    due.assicurazione = 5;
    EsitoTavolo tot = risolviTavolo(due);
    CHECK(tot.mani.size() == 2);
    CHECK(tot.assicurazione == 10);
    CHECK(tot.totale == -10 - 10 + 10);
}

static void testStatisticheEFiches() {
    Sessione s;   // 100 fiches iniziali
    EsitoTavolo vinta;
    Esito e;
    e.risultato = Risultato::VITTORIA;
    e.variazione = 10;
    vinta.mani.push_back(e);
    vinta.totale = 10;

    applicaEsito(s, vinta);
    applicaEsito(s, vinta);
    CHECK(s.fiches == 120);
    CHECK(s.stat.maniGiocate == 2 && s.stat.maniVinte == 2);
    CHECK(s.stat.serieVittorie == 2 && s.stat.migliorSerie == 2);
    CHECK(s.stat.fichesMassime == 120);

    EsitoTavolo persa;
    e.risultato = Risultato::SCONFITTA;
    e.variazione = -30;
    persa.mani.push_back(e);
    persa.totale = -30;
    applicaEsito(s, persa);
    CHECK(s.fiches == 90);
    CHECK(s.stat.maniPerse == 1);
    CHECK(s.stat.serieVittorie == 0 && s.stat.migliorSerie == 2);
    CHECK(s.stat.fichesMinime == 90);
}

static void testMazzo() {
    Sessione s;
    s.generatore.seed(12345);
    creaMazzo(s.mazzo, s.generatore);
    CHECK(static_cast<int>(s.mazzo.size()) == CARTE_PER_MAZZO * NUMERO_MAZZI);
    CHECK(!mazzoDaRimescolare(s.mazzo));

    // Ogni valore compare lo stesso numero di volte
    vector<int> conteggio(VALORI_PER_SEME, 0);
    for (const Carta& c : s.mazzo) conteggio[c.valore]++;
    for (int v = 0; v < VALORI_PER_SEME; v++) {
        CHECK(conteggio[v] == NUMERO_SEMI * NUMERO_MAZZI);
    }

    size_t prima = s.mazzo.size();
    pescaCarta(s);
    CHECK(s.mazzo.size() == prima - 1);

    s.mazzo.resize(SOGLIA_RIMESCOLAMENTO - 1);
    CHECK(mazzoDaRimescolare(s.mazzo));

    // Mazzo vuoto: pescaCarta lo ricrea invece di rompersi
    s.mazzo.clear();
    pescaCarta(s);
    CHECK(!s.mazzo.empty());
}

int main() {
    testConversione();
    testPunteggi();
    testPuntate();
    testAzioni();
    testSplit();
    testBanco();
    testAssicurazione();
    testEsiti();
    testStatisticheEFiches();
    testMazzo();

    if (fallimenti == 0) {
        cout << "Tutti i test passati (" << controlli << " controlli).\n";
        return 0;
    }
    cerr << fallimenti << " controlli falliti su " << controlli << ".\n";
    return 1;
}
