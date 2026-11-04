// LOGICA: le regole del blackjack.
// Nessun cout, cin, colore o pausa: solo dati in ingresso e risultati.
// Questo header (e logica.cpp) NON deve mai includere terminale.h o grafica.h:
// se succede, lo strato è rotto.
#pragma once

#include <random>
#include <string>
#include <vector>

#include "base/dati.h"

// --- Input e mazzo ---
bool convertiInIntero(const std::string& testo, int& numero);
void creaMazzo(std::vector<Carta>& mazzo, std::mt19937& generatore);
bool mazzoDaRimescolare(const std::vector<Carta>& mazzo);
Carta pescaCarta(Sessione& sessione);

// --- Punteggi ---
int valoreCarta(const Carta& carta);
int calcolaPunteggio(const std::vector<Carta>& mano);
bool haSforato(const std::vector<Carta>& mano);
bool haVentuno(const std::vector<Carta>& mano);
bool haBlackjack(const std::vector<Carta>& mano);
bool blackjackGiocatore(const Mano& mano);

// --- Puntate ---
bool puntataValida(int puntata, int fiches);
int puntataMassima(int fiches);
bool fichesInsufficienti(int fiches);
int puntataTotale(const Tavolo& tavolo);
int fichesLibere(const Tavolo& tavolo, int fiches);

// --- Azioni ---
std::vector<Azione> azioniDisponibili(const Tavolo& tavolo, int fiches);
void dividiMano(Tavolo& tavolo);

// --- Banco ---
bool bancoDevePescare(const Tavolo& tavolo);
bool bancoDeveGiocare(const Tavolo& tavolo);

// --- Assicurazione ---
int importoAssicurazione(const Tavolo& tavolo);
bool assicurazioneDisponibile(const Tavolo& tavolo, int fiches);
int risolviAssicurazione(const Tavolo& tavolo);

// --- Esiti ---
Esito risolviMano(const Tavolo& tavolo, const Mano& mano);
EsitoTavolo risolviTavolo(const Tavolo& tavolo);
void registraMano(Statistiche& stat, const Esito& esito);
void applicaEsito(Sessione& sessione, const EsitoTavolo& esiti);
