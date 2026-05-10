#pragma once

/**
 * @file graph.hpp
 * @brief Deklarácie globálnych premenných a funkcií pre prácu s DAG.
 *
 * Obsahuje externé deklarácie globálnych premenných zdieľaných medzi
 * @c graph.cpp a @c main.cpp, a prototypy funkcií @c dfs(), @c jeDAG()
 * a @c doplnDAG(). Definície sú v @c graph.cpp.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

/**
 * @brief Celkový počet vrcholov grafu.
 *
 * Hodnota sa určí pri načítaní súboru ako @c maxIdx + 1,
 * kde @c maxIdx je najväčší číselný index vrcholu v súbore.
 */
extern int n;

/**
 * @brief Zoznam susedov orientovaného grafu.
 *
 * @c adj[u] obsahuje zoznam všetkých vrcholov @c v, do ktorých
 * vedie orientovaná hrana @c u → @c v.
 */
extern vector<vector<int>> adj;

/**
 * @brief Stavové pole vrcholov počas DFS.
 *
 * Každý prvok @c stav[u] nadobúda jednu z troch hodnôt:
 * - @c 0 — biely: vrchol ešte nebol navštívený.
 * - @c 1 — sivý: vrchol je práve spracovávaný (je na zásobníku rekurzie).
 * - @c 2 — čierny: vrchol je dokončený, všetci susedia boli spracovaní.
 */
extern vector<int> stav;

/**
 * @brief Výsledné topologické poradie vrcholov.
 *
 * Pole sa vypĺňa počas DFS — vrchol sa pridá keď sú všetci jeho
 * nástupcovia dokončení. Keďže DFS pridáva odzadu, na konci je
 * potrebné zavolať @c reverse().
 */
extern vector<int> topo;

/**
 * @brief Príznak detekcie cyklu.
 *
 * Nastaví sa na @c true hneď ako DFS nájde spätnú hranu
 * (hranu vedúcu do sivého vrcholu). Rekurzia sa potom čo
 * najrýchlejšie preruší.
 */
extern bool cyklus;

// ============================================================
//  Deklarácie funkcií (implementácie sú v graph.cpp)
// ============================================================

/**
 * @brief Rekurzívne DFS z vrcholu @p u — detekcia cyklu a topologické poradie.
 * @param u Index vrcholu, z ktorého sa spúšťa DFS.
 * @note Mení globálne premenné @c stav, @c topo a @c cyklus.
 */
void dfs(int u);

/**
 * @brief Overí, či je graf DAG; ak áno, vyplní @c topo[] topologickým poradím.
 * @return @c true ak graf je DAG, @c false ak obsahuje orientovaný cyklus.
 * @post Ak vráti @c true, @c topo[] obsahuje platné topologické poradie.
 */
bool jeDAG();

/**
 * @brief Doplní graf na kompletný DAG a zapíše ho do súboru.
 *
 * Pre každú dvojicu @c (u, v) v topologickom poradí kde chýba hrana
 * @c u→v, pridá ju. Cieľový počet hrán: @f$ \frac{n(n-1)}{2} @f$.
 *
 * @param vystupSubor Meno výstupného súboru (@c "u v" na každý riadok).
 * @warning Ak sa súbor nedá vytvoriť, vypíše chybu na @c stderr.
 */
void doplnDAG(const string& vystupSubor);
