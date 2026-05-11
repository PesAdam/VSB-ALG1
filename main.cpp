/**
 * @file main.cpp
 * @brief Overenie DAG a doplnenie na kompletný DAG pomocou DFS.
 *
 * Program načíta orientovaný graf zo súboru, overí pomocou prehľadávania
 * do hĺbky (DFS), či je graf orientovaný acyklický graf (DAG). Ak áno,
 * doplní ho ďalšími hranami na kompletný DAG — teda na graf s maximálnym
 * počtom hrán @f$ |E| = \frac{n(n-1)}{2} @f$. Výsledok zapíše do súboru.
 *
 * @section Vstup
 * Textový súbor kde každý riadok obsahuje dvojicu celých čísel @c i @c j
 * oddelených bielym znakom — reprezentujú orientovanú hranu z vrcholu @c i
 * do vrcholu @c j. Vrcholy sú číslované od 0.
 *
 * @section Vystup
 * Na štandardný výstup sa vypíše:
 *  -# Či je graf DAG.
 *  -# Počet vrcholov a hrán vstupného grafu.
 *  -# Očakávaný počet hrán kompletného DAG podľa vzorca.
 *  -# Počet vrcholov a hrán výsledného kompletného DAG.
 *
 * Kompletný DAG sa zapíše do súboru s príponou @c _complete (napr.
 * @c graph1.txt → @c graph1_complete.txt).
 *
 * @author Projekt — Algoritmy 1
 * @date 2026
 */

#include "src/graph.hpp"

 /**    
  * @brief Hlavná funkcia — vstupný bod programu.
  *
  * Postup:
  *  -# Načíta meno vstupného súboru od používateľa.
  *  -# Načíta hrany grafu zo súboru a zostaví zoznam susedov @c adj[].
  *  -# Zavolá jeDAG() — ak graf nie je DAG, vypíše hlásenie a skončí.
  *  -# Vypíše body 1 a 2 na stdout (či je DAG, počty vrcholov a hrán).
  *  -# Odvodí meno výstupného súboru (@c vstup_complete.txt).
  *  -# Zavolá doplnDAG() — doplní hrany, zapíše súbor, vypíše body 3 a 4.
  *
  * @return @c 0 pri úspešnom behu,
  *         @c 1 ak sa súbor nedá otvoriť alebo je prázdny.
  */
int main() {
    // nacitanie suboru

    string subor;   // premena pre meno suboru
    cout << "Zadaj meno suboru: ";
    cin >> subor;  // nacitame meno suboru

    // otvorime subor
    ifstream f(subor);

    // ak neni subor tak koncime
    if (!f) {
        cerr << "Chyba: subor '" << subor << "' sa neda otvorit!\n";
        return 1;
    }

    // kazda hrana je dvojica (from, to) — z akeho vrcholu do akeho
    set<pair<int, int>> hranyZoSuboru;
    set<int> unique_nodes;

    // maxIdx sleduje najvacsi ciselny index vrcholu
    // zaciname na -1 ak subor bude prazdny ostane -1
    int maxIdx = -1, u, v;
    int maxNode = -1;

    // citanie suboru 
    while (f >> u >> v) {
        hranyZoSuboru.insert({ u, v });
        unique_nodes.insert(u);  
        unique_nodes.insert(v);        
        maxNode = max(maxNode, max(u, v));            // precitame si dvojice a vyberieme vzdy najvacsi index zo vsetkych dvojich
    }

    f.close();
    n = maxNode + 1;     // pocet vrcholov

    adj.assign(n, {});  // inicializujeme zoznam susedov adjency (n prazdnych vektorov)

    // naplnime zoznam susedov z nacitancyh hran
    // pre kazdu hranu ktoru sme nacitali zo suboru zozstavime zoznam susedov (naplnime zasobnik)
    for (auto& hrana : hranyZoSuboru) {
        adj[hrana.first].push_back(hrana.second);
    }

    if (!jeDAG()) {
        // graf nieje dag
        cout << "Graf NIE JE DAG — obsahuje orientovany cyklus.\n";
        // vypiseme co sme naratali aj napriek tomu ze nieje dag
        cout << "Pocet vrcholov: " << unique_nodes.size()
            << ", pocet hran: " << hranyZoSuboru.size() << "\n";
        return 0;
    }

    cout << "Graf JE DAG.\n";
    // pocet vrcholov vstupneho grafu
    cout << "Pocet vrcholov: " << unique_nodes.size()
        << ", pocet hran: " << hranyZoSuboru.size() << "\n";

    string vystupSubor;
    size_t bodka = subor.rfind('.');   // najdeme poslednu bodku v subore
    if (bodka != string::npos) {         // ak bodka existuje tak mame priponu
        vystupSubor = subor.substr(0, bodka) + "_complete" + subor.substr(bodka);
    }
    else {                               // ak bodka nieje nemame priponu
        vystupSubor = subor + "_complete";
    }
    // zavolame funkciu na doplnenie dagu
    doplnDAG(vystupSubor);

    return 0;
}
