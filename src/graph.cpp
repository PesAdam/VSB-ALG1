/**
 * @file graph.cpp
 * @brief Implementácia globálnych premenných a algoritmov pre DAG.
 *
 * Obsahuje definície globálnych premenných a implementácie funkcií
 * @c dfs(), @c jeDAG() a @c doplnDAG().
 */

#include "graph.hpp"

// ============================================================
//  Definície globálnych premenných
// ============================================================

/// @copydoc n
int n;
/// @copydoc adj
vector<vector<int>> adj;
/// @copydoc stav
vector<int> stav;
/// @copydoc topo
vector<int> topo;
/// @copydoc cyklus
bool cyklus;

// ============================================================
//  Implementácie funkcií
// ============================================================

void dfs(int u) {
    stav[u] = 1; // sivý — práve navštevujeme u (je na zásobníku rekurzie)

    for (int v : adj[u]) {
        if (stav[v] == 1) {
            // Sivý sused = spätná hrana = cyklus
            cyklus = true;
            return;
        }
        if (stav[v] == 0)
            dfs(v); // rekurzívne zanorenie do nenavštíveného suseda

        if (cyklus) return; // prerušíme rekurziu hneď po nájdení cyklu
    }

    stav[u] = 2;        // čierny — všetci susedia spracovaní
    topo.push_back(u);  // pridáme do topologického poradia (odzadu)
}

bool jeDAG() {
    stav.assign(n, 0); // všetky vrcholy sú biele
    cyklus = false;
    topo.clear();

    // Spustíme DFS z každého nenavštíveného vrcholu — pokryjeme aj nesúvislý graf
    for (int i = 0; i < n; i++)
        if (stav[i] == 0)
            dfs(i);

    // DFS pridával odzadu → otočíme na správne topologické poradie
    reverse(topo.begin(), topo.end());

    return !cyklus;
}

void doplnDAG(const string& vystupSubor) {
    // Množina existujúcich hrán — O(log n) overenie existencie
    set<pair<int,int>> hrany;

    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            hrany.insert({u, v});

    // Pridáme chýbajúce hrany podľa topologického poradia (i < j → bez cyklu)
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int u = topo[i]; // predchodca
            int v = topo[j]; // nasledovník
            if (!hrany.count({u, v}))
                hrany.insert({u, v});
        }
    }

    // Zapíšeme kompletný DAG do výstupného súboru (formát: "u v" na každý riadok)
    ofstream out(vystupSubor);
    if (!out) {
        cerr << "Chyba: nepodarilo sa vytvorit vystupny subor '" << vystupSubor << "'!\n";
        return;
    }
    for (auto& [u, v] : hrany)
        out << u << " " << v << "\n";
    out.close();

    // Bod 3: očakávaný počet hrán podľa vzorca n*(n-1)/2
    long long ocakavane = (long long)n * (n - 1) / 2;
    cout << "Ocakavany pocet hran kompletneho DAG: " << ocakavane << "\n";

    // Bod 4: skutočný počet vrcholov a hrán výsledku
    cout << "Kompletny DAG — pocet vrcholov: " << n
         << ", pocet hran: " << (long long)hrany.size() << "\n";
    cout << "Kompletny DAG zapísany do suboru: " << vystupSubor << "\n";
}
