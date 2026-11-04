// LOGICA: le regole del blackjack.
// Nessun cout, cin, colore o pausa: solo dati in ingresso e risultati.
// Non include terminale.h né grafica.h.

#include "logica/logica.h"
#include "base/costanti.h"

#include <algorithm>   // shuffle
#include <random>      // mt19937
#include <string>      // stoi
#include <vector>

using namespace std;

// Converte un testo in numero. Accetta solo cifre (spazi ai lati ignorati):
// "12abc", "3.5" o "" vengono rifiutati.
bool convertiInIntero(const string& testo, int& numero) {
    size_t inizio = testo.find_first_not_of(" \t");
    if (inizio == string::npos) {
        return false;
    }
    size_t fine = testo.find_last_not_of(" \t\r");
    string pulito = testo.substr(inizio, fine - inizio + 1);

    if (pulito.size() > 9) {   // oltre sforerebbe un int
        return false;
    }
    for (char c : pulito) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    numero = stoi(pulito);
    return true;
}

// Riempie il mazzo con NUMERO_MAZZI mazzi e lo mescola.
// Il generatore arriva da fuori, così il seme lo decide il chiamante.
void creaMazzo(vector<Carta>& mazzo, mt19937& generatore) {
    mazzo.clear();
    for (int i = 0; i < CARTE_PER_MAZZO * NUMERO_MAZZI; i++) {
        Carta c;
        c.valore = i % VALORI_PER_SEME;
        c.seme = static_cast<Seme>((i / VALORI_PER_SEME) % NUMERO_SEMI);
        mazzo.push_back(c);
    }
    shuffle(mazzo.begin(), mazzo.end(), generatore);
}

bool mazzoDaRimescolare(const vector<Carta>& mazzo) {
    return static_cast<int>(mazzo.size()) < SOGLIA_RIMESCOLAMENTO;
}

// Toglie e restituisce l'ultima carta del mazzo
Carta pescaCarta(Sessione& sessione) {
    if (sessione.mazzo.empty()) {
        creaMazzo(sessione.mazzo, sessione.generatore);   // sicurezza
    }
    Carta estratta = sessione.mazzo.back();
    sessione.mazzo.pop_back();
    return estratta;
}

// Asso = 11, figure = 10, le altre valore + 1
int valoreCarta(const Carta& carta) {
    if (carta.valore == ASSO) return VALORE_ASSO;
    if (carta.valore >= FANTE) return VALORE_FIGURA;
    return carta.valore + 1;
}

// Somma la mano; gli assi passano da 11 a 1 uno alla volta finché si sfora.
int calcolaPunteggio(const vector<Carta>& mano) {
    int punteggio = 0;
    int assi = 0;   // assi ancora contati come 11

    for (const Carta& carta : mano) {
        punteggio += valoreCarta(carta);
        if (carta.valore == ASSO) {
            assi++;
        }
    }
    while (punteggio > OBIETTIVO && assi > 0) {
        punteggio -= VALORE_ASSO - 1;
        assi--;
    }
    return punteggio;
}

bool haSforato(const vector<Carta>& mano) {
    return calcolaPunteggio(mano) > OBIETTIVO;
}

bool haVentuno(const vector<Carta>& mano) {
    return calcolaPunteggio(mano) == OBIETTIVO;
}

// Blackjack = 21 con le prime due carte
bool haBlackjack(const vector<Carta>& mano) {
    return static_cast<int>(mano.size()) == CARTE_INIZIALI && haVentuno(mano);
}

// Per il giocatore il 21 di una mano divisa non è un blackjack naturale
bool blackjackGiocatore(const Mano& mano) {
    return !mano.daSplit && haBlackjack(mano.carte);
}

// --- Puntate ---

// Puntata valida: pari, almeno il minimo, entro le fiches che hai
bool puntataValida(int puntata, int fiches) {
    return puntata >= PUNTATA_MINIMA && puntata % 2 == 0 && puntata <= fiches;
}

// La puntata più alta possibile: il numero pari più grande che non supera le fiches
int puntataMassima(int fiches) {
    return fiches - fiches % 2;
}

bool fichesInsufficienti(int fiches) {
    return fiches < PUNTATA_MINIMA;
}

// Somma delle puntate di tutte le mani
int puntataTotale(const Tavolo& tavolo) {
    int totale = 0;
    for (const Mano& mano : tavolo.mani) {
        totale += mano.puntata;
    }
    return totale;
}

// Fiches che non sono già in gioco (puntate + assicurazione)
int fichesLibere(const Tavolo& tavolo, int fiches) {
    return fiches - puntataTotale(tavolo) - tavolo.assicurazione;
}

// --- Azioni ---

// Raddoppio, split e resa si possono fare solo con due carte. Raddoppio e
// split richiedono anche fiches libere per una puntata in più; lo split vuole
// due carte dello stesso valore (anche K+Q) e meno di MAX_MANI mani; la resa
// non si può fare dopo aver diviso.
vector<Azione> azioniDisponibili(const Tavolo& tavolo, int fiches) {
    const Mano& mano = tavolo.mani[tavolo.manoAttiva];
    vector<Azione> azioni = {Azione::CARTA, Azione::STAI};

    if (static_cast<int>(mano.carte.size()) == CARTE_INIZIALI) {
        bool fichesBastano = fichesLibere(tavolo, fiches) >= mano.puntata;

        if (fichesBastano) {
            azioni.push_back(Azione::RADDOPPIA);
        }
        bool stessoValore = valoreCarta(mano.carte[0]) == valoreCarta(mano.carte[1]);
        bool postiLiberi = static_cast<int>(tavolo.mani.size()) < MAX_MANI;
        if (fichesBastano && stessoValore && postiLiberi) {
            azioni.push_back(Azione::DIVIDI);
        }
        if (tavolo.mani.size() == 1) {
            azioni.push_back(Azione::RESA);
        }
    }
    return azioni;
}

// Divide la mano attiva: la seconda carta va in una nuova mano, inserita subito
// dopo, con la stessa puntata. Le carte per le due mani le pesca chi chiama.
// Gli assi divisi ricevono una sola carta e la mano finisce lì.
void dividiMano(Tavolo& tavolo) {
    Mano& originale = tavolo.mani[tavolo.manoAttiva];

    Mano nuova;
    nuova.carte.push_back(originale.carte.back());
    nuova.puntata = originale.puntata;
    originale.carte.pop_back();

    originale.daSplit = true;
    nuova.daSplit = true;
    if (originale.carte[0].valore == ASSO) {
        originale.conclusa = true;
        nuova.conclusa = true;
    }

    // Dopo l'inserimento "originale" non è più valido: non si usa oltre
    tavolo.mani.insert(tavolo.mani.begin() + tavolo.manoAttiva + 1, nuova);
}

// --- Banco ---

bool bancoDevePescare(const Tavolo& tavolo) {
    return calcolaPunteggio(tavolo.banco) < SOGLIA_BANCO;
}

// Il banco gioca solo se almeno una mano è ancora in corsa
// (non sforata e non resa).
bool bancoDeveGiocare(const Tavolo& tavolo) {
    for (const Mano& mano : tavolo.mani) {
        if (!mano.resa && !haSforato(mano.carte)) {
            return true;
        }
    }
    return false;
}

// --- Assicurazione ---

// Metà della prima puntata (le puntate sono pari, quindi è sempre intera)
int importoAssicurazione(const Tavolo& tavolo) {
    return tavolo.mani[0].puntata / 2;
}

// Si offre quando il banco mostra un asso e hai le fiches per pagarla
bool assicurazioneDisponibile(const Tavolo& tavolo, int fiches) {
    return tavolo.banco[0].valore == ASSO
        && fichesLibere(tavolo, fiches) >= importoAssicurazione(tavolo);
}

// Paga 2:1 se il banco ha blackjack, altrimenti si perde
int risolviAssicurazione(const Tavolo& tavolo) {
    if (tavolo.assicurazione == 0) {
        return 0;
    }
    if (haBlackjack(tavolo.banco)) {
        return tavolo.assicurazione * PAGAMENTO_ASSICURAZIONE;
    }
    return -tavolo.assicurazione;
}

// --- Esiti ---

// Dice com'è finita una mano. Non modifica niente: fiches e statistiche
// le aggiorna applicaEsito.
Esito risolviMano(const Tavolo& tavolo, const Mano& mano) {
    bool naturaleGiocatore = blackjackGiocatore(mano);
    bool naturaleBanco = haBlackjack(tavolo.banco);
    int punteggioGiocatore = calcolaPunteggio(mano.carte);
    int punteggioBanco = calcolaPunteggio(tavolo.banco);
    int puntata = mano.puntata;

    Esito esito;   // parte da PAREGGIO, variazione 0

    if (mano.resa) {
        esito.risultato = Risultato::RESA;
        esito.variazione = -(puntata / 2);   // puntata pari: metà esatta
    } else if (naturaleGiocatore && naturaleBanco) {
        esito.risultato = Risultato::DOPPIO_BLACKJACK;
    } else if (naturaleGiocatore) {
        esito.risultato = Risultato::BLACKJACK_GIOCATORE;
        esito.variazione = puntata * BLACKJACK_NUMERATORE / BLACKJACK_DENOMINATORE;
    } else if (naturaleBanco) {
        esito.risultato = Risultato::BLACKJACK_BANCO;
        esito.variazione = -puntata;
    } else if (haSforato(mano.carte)) {
        esito.risultato = Risultato::GIOCATORE_SFORA;
        esito.variazione = -puntata;
    } else if (haSforato(tavolo.banco)) {
        esito.risultato = Risultato::BANCO_SFORA;
        esito.variazione = puntata;
    } else if (punteggioGiocatore > punteggioBanco) {
        esito.risultato = Risultato::VITTORIA;
        esito.variazione = puntata;
    } else if (punteggioGiocatore < punteggioBanco) {
        esito.risultato = Risultato::SCONFITTA;
        esito.variazione = -puntata;
    }
    return esito;
}

// Esiti di tutte le mani e dell'assicurazione
EsitoTavolo risolviTavolo(const Tavolo& tavolo) {
    EsitoTavolo esiti;
    for (const Mano& mano : tavolo.mani) {
        Esito esito = risolviMano(tavolo, mano);
        esiti.mani.push_back(esito);
        esiti.totale += esito.variazione;
    }
    esiti.assicurazione = risolviAssicurazione(tavolo);
    esiti.totale += esiti.assicurazione;
    return esiti;
}

// Aggiorna le statistiche con l'esito di una mano (le fiches no)
void registraMano(Statistiche& stat, const Esito& esito) {
    stat.maniGiocate++;

    if (esito.risultato == Risultato::DOPPIO_BLACKJACK
        || esito.risultato == Risultato::BLACKJACK_GIOCATORE) {
        stat.blackjackNaturali++;
    }

    if (esito.variazione > 0) {
        stat.maniVinte++;
        stat.serieVittorie++;
        if (stat.serieVittorie > stat.migliorSerie) {
            stat.migliorSerie = stat.serieVittorie;
        }
    } else if (esito.variazione < 0) {
        stat.maniPerse++;
        stat.serieVittorie = 0;
    } else {
        stat.maniPareggiate++;
    }
}

// Aggiorna statistiche e fiches con gli esiti del tavolo. Le fiches si aggiornano
// una volta sola, col totale, così massimo e minimo non contano passaggi intermedi.
void applicaEsito(Sessione& sessione, const EsitoTavolo& esiti) {
    for (const Esito& esito : esiti.mani) {
        registraMano(sessione.stat, esito);
    }

    sessione.fiches += esiti.totale;
    if (sessione.fiches > sessione.stat.fichesMassime) {
        sessione.stat.fichesMassime = sessione.fiches;
    }
    if (sessione.fiches < sessione.stat.fichesMinime) {
        sessione.stat.fichesMinime = sessione.fiches;
    }
}
