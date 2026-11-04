// GRAFICA: disegna lo schermo e legge l'input.
// Non decide le regole e non cambia lo stato di gioco.
// Qui ci sono solo le funzioni che usano gli altri file: tutto il resto
// (stampaBox, rigaCarta, PIP, TITOLO, ...) resta dentro grafica.cpp.
#pragma once

#include <string>
#include <vector>

#include "base/dati.h"

// --- Input ---
int leggiIntero(int minimo, int massimo);
void aspettaInvio(const std::string& testo = "Premi Invio per continuare...");

// --- Tavolo ---
void disegnaTavolo(const Sessione& sessione, const Tavolo& tavolo,
                   bool nascondiBanco, const std::string& messaggio);
void stampaEsito(const EsitoTavolo& esiti);

// --- Schermate intere ---
void schermataMenu(int fiches, const Statistiche& stat);
void schermataStatistiche(const Statistiche& stat, int fiches);
void schermataRegole();
void schermataFinale(const Statistiche& stat, int fiches, bool gameOver);

// --- Domande al giocatore ---
int chiediPuntata(int fiches, const Statistiche& stat);
Azione chiediAzione(const std::vector<Azione>& azioni);
bool chiediAssicurazione(int importo);
