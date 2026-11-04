// Costanti del programma: impostazioni, regole, pagamenti, mazzo.
// Sono solo numeri e flag: questo header non include niente.
// (Le costanti del disegno stanno in grafica.cpp: solo la grafica le usa.)
//
// Una "const" a livello globale ha collegamento interno: ogni .cpp che
// include questo file ne riceve una copia sua, quindi non ci sono conflitti
// al momento del link.
#pragma once

// ---------------------------------------------------------------------
//  IMPOSTAZIONI
// ---------------------------------------------------------------------

const bool USA_COLORI = true;         // false se il terminale mostra simboli strani
const int PAUSA_ANIMAZIONI = 700;     // millisecondi tra un passaggio e l'altro (0 = nessuna pausa)
const int FICHES_INIZIALI = 100;
const int NUMERO_MAZZI = 6;           // mazzi da 52 carte mescolati insieme

// ---------------------------------------------------------------------
//  COSTANTI
// ---------------------------------------------------------------------

// Regole
const int OBIETTIVO = 21;
const int SOGLIA_BANCO = 17;              // il banco pesca finché ha meno di così
const int VALORE_ASSO = 11;
const int VALORE_FIGURA = 10;
const int CARTE_INIZIALI = 2;
const int PUNTATA_MINIMA = 2;             // le puntate sono sempre pari
const int MAX_MANI = 4;                   // mani contemporanee dopo gli split

// Pagamenti. Con le puntate pari il 3:2 dà sempre un numero intero.
const int BLACKJACK_NUMERATORE = 3;
const int BLACKJACK_DENOMINATORE = 2;
const int PAGAMENTO_ASSICURAZIONE = 2;    // l'assicurazione paga 2:1

// Mazzo. Il valore di una carta va da 0 a 12.
const int CARTE_PER_MAZZO = 52;
const int VALORI_PER_SEME = 13;
const int NUMERO_SEMI = 4;
const int ASSO = 0;
const int FANTE = 10;
const int DONNA = 11;
const int RE = 12;

// Si rimescola quando resta circa un quarto delle carte, ma mai sotto
// MINIMO_CARTE, così una mano non finisce le carte a metà.
const int QUARTO_DEL_MAZZO = CARTE_PER_MAZZO * NUMERO_MAZZI / 4;
const int MINIMO_CARTE = 20;
const int SOGLIA_RIMESCOLAMENTO =
    (QUARTO_DEL_MAZZO > MINIMO_CARTE) ? QUARTO_DEL_MAZZO : MINIMO_CARTE;
