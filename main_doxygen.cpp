/**
 * @file main_doxygen.cpp
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

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

// ============================================================
//  Globálne premenné
// ============================================================

/**
 * @brief Celkový počet vrcholov grafu.
 *
 * Hodnota sa určí pri načítaní súboru ako @c maxIdx + 1,
 * kde @c maxIdx je najväčší číselný index vrcholu v súbore.
 */
int n;

/**
 * @brief Zoznam susedov orientovaného grafu.
 *
 * @c adj[u] obsahuje zoznam všetkých vrcholov @c v, do ktorých
 * vedie orientovaná hrana @c u → @c v.
 * Napríklad ak existujú hrany @c 0→1 a @c 0→3, potom @c adj[0] = {1, 3}.
 */
vector<vector<int>> adj;

/**
 * @brief Stavové pole vrcholov počas DFS.
 *
 * Každý prvok @c stav[u] nadobúda jednu z troch hodnôt:
 * - @c 0 — biely: vrchol ešte nebol navštívený.
 * - @c 1 — sivý: vrchol je práve spracovávaný (je na zásobníku rekurzie).
 * - @c 2 — čierny: vrchol je dokončený, všetci jeho susedia boli spracovaní.
 */
vector<int> stav;

/**
 * @brief Výsledné topologické poradie vrcholov.
 *
 * Pole sa vypĺňa počas DFS. Vrchol sa pridá až vtedy, keď sú
 * všetci jeho nástupcovia dokončení (čierni). Keďže DFS pridáva
 * vrcholy od najhlbšieho po najplytší, na konci je potrebné
 * zavolať @c reverse().
 */
vector<int> topo;

/**
 * @brief Príznak detekcie cyklu.
 *
 * Nastaví sa na @c true hneď ako DFS nájde spätnú hranu —
 * teda hranu vedúcu do sivého vrcholu. Po nastavení sa
 * rekurzia čo najrýchlejšie preruší.
 */
bool cyklus;

// ============================================================
//  Funkcie
// ============================================================

/**
 * @brief Rekurzívne prehľadávanie do hĺbky (DFS) z vrcholu @p u.
 *
 * Funkcia plní dve úlohy naraz:
 *  -# **Detekcia cyklu** — ak narazí na sivý vrchol (hrana späť na zásobník),
 *     nastaví globálnu premennú @c cyklus na @c true a okamžite sa vráti.
 *  -# **Zostavenie topologického poradia** — vrchol @p u sa pridá do @c topo[]
 *     až po dokončení všetkých jeho nástupcov (keď sa nastavuje na čierny).
 *
 * Farebná konvencia stavov:
 * - biely (0) → nenavštívený
 * - sivý  (1) → práve na zásobníku rekurzie
 * - čierny (2) → dokončený
 *
 * @param u Index vrcholu, z ktorého sa spúšťa DFS.
 *
 * @note Funkcia mení globálne premenné @c stav, @c topo a @c cyklus.
 * @note Ak @c cyklus == true po návrate, graf obsahuje orientovaný cyklus.
 */
void dfs(int u) {
    stav[u] = 1; // vrchol u je teraz sivý — práve ho navštevujeme

    for (int v : adj[u]) {
        if (stav[v] == 1) {
            // spätná hrana → cyklus
            cyklus = true;
            return;
        }
        if (stav[v] == 0)
            dfs(v); // rekurzívne zanorenie do nenavštíveného suseda

        if (cyklus) return; // prerušíme rekurziu hneď po nájdení cyklu
    }

    stav[u] = 2;        // vrchol u je čierny — všetci susedia spracovaní
    topo.push_back(u);  // pridáme ho do topologického poradia (odzadu)
}

/**
 * @brief Overí, či je graf orientovaný acyklický graf (DAG).
 *
 * Spustí DFS zo všetkých doteraz nenavštívených vrcholov —
 * tým pokryje aj nesúvislé grafy, kde nie všetky vrcholy sú
 * dosažiteľné z jedného štartovacieho bodu.
 *
 * Po skončení DFS otočí pole @c topo[], pretože DFS pridáva
 * vrcholy v opačnom topologickom poradí.
 *
 * @return @c true ak graf je DAG (neobsahuje orientovaný cyklus),
 *         @c false ak graf obsahuje aspoň jeden orientovaný cyklus.
 *
 * @post Ak návratová hodnota je @c true, globálne pole @c topo[]
 *       obsahuje platné topologické poradie vrcholov.
 *
 * @note Využíva globálne premenné @c n, @c adj, @c stav, @c topo, @c cyklus.
 */
bool jeDAG() {
    stav.assign(n, 0); // všetky vrcholy sú biele
    cyklus = false;
    topo.clear();

    for (int i = 0; i < n; i++)
        if (stav[i] == 0)
            dfs(i); // spustíme DFS z každého nenavštíveného vrcholu

    reverse(topo.begin(), topo.end()); // otočíme — DFS pridával odzadu

    return !cyklus;
}

/**
 * @brief Doplní graf na kompletný DAG a zapíše ho do súboru.
 *
 * Predpokladá, že graf je DAG a že pole @c topo[] obsahuje platné
 * topologické poradie (teda že pred volaním bola úspešne zavolaná
 * funkcia jeDAG()).
 *
 * **Algoritmus:**
 *  -# Naplní množinu @c hrany všetkými existujúcimi hranami z @c adj[].
 *  -# Prechádza všetky dvojice @c (i, j) kde @c i < @c j v poli @c topo[].
 *     Pre každú dvojicu @c u = topo[i], @c v = topo[j] pridá hranu
 *     @c u→v ak ešte neexistuje. Keďže @c u je topologicky pred @c v,
 *     hrana nemôže vytvoriť cyklus.
 *  -# Zapíše všetky hrany do výstupného súboru.
 *  -# Vypíše na stdout body 3 a 4 podľa zadania.
 *
 * Cieľový počet hrán po doplnení:
 * @f[
 *   |E_C| = \frac{n(n-1)}{2}
 * @f]
 *
 * @param vystupSubor Meno výstupného textového súboru, do ktorého sa
 *                    zapíše kompletný DAG vo formáte @c "u v" na každý riadok.
 *
 * @note Používa globálne premenné @c n, @c adj, @c topo.
 * @warning Ak sa výstupný súbor nedá vytvoriť, funkcia vypíše chybu na
 *          @c stderr a skončí bez zápisu (počty na stdout sa stále vypíšu).
 */
void doplnDAG(const string& vystupSubor) {
    // Množina existujúcich hrán — umožňuje O(log n) overenie existencie
    set<pair<int,int>> hrany;

    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            hrany.insert({u, v});

    // Pridáme chýbajúce hrany v smere topologického poradia
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int u = topo[i]; // predchodca v topologickom poradí
            int v = topo[j]; // nasledovník v topologickom poradí
            if (!hrany.count({u, v}))
                hrany.insert({u, v});
        }
    }

    // Zápis kompletného DAG do súboru (rovnaký formát ako vstup)
    ofstream out(vystupSubor);
    if (!out) {
        cerr << "Chyba: nepodarilo sa vytvorit vystupny subor '" << vystupSubor << "'!\n";
        return;
    }
    for (auto& [u, v] : hrany)
        out << u << " " << v << "\n";
    out.close();

    // Bod 3: očakávaný počet hrán podľa vzorca
    long long ocakavane = (long long)n * (n - 1) / 2;
    cout << "Ocakavany pocet hran kompletneho DAG: " << ocakavane << "\n";

    // Bod 4: skutočný počet vrcholov a hrán výsledku
    cout << "Kompletny DAG — pocet vrcholov: " << n
         << ", pocet hran: " << (long long)hrany.size() << "\n";

    cout << "Kompletny DAG zapísany do suboru: " << vystupSubor << "\n";
}

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
    string subor;
    cout << "Zadaj meno suboru: ";
    cin >> subor;

    ifstream f(subor);
    if (!f) {
        cerr << "Chyba: subor '" << subor << "' sa neda otvorit!\n";
        return 1;
    }

    vector<pair<int,int>> hranyZoSuboru;
    int maxIdx = -1, u, v;
    while (f >> u >> v) {
        hranyZoSuboru.push_back({u, v});
        maxIdx = max(maxIdx, max(u, v));
    }
    f.close();

    if (maxIdx < 0) {
        cout << "Subor je prazdny alebo nema spravny format.\n";
        return 1;
    }

    n = maxIdx + 1;
    adj.assign(n, {});
    for (auto& [from, to] : hranyZoSuboru)
        adj[from].push_back(to);

    if (!jeDAG()) {
        // Bod 1: graf nie je DAG
        cout << "Graf NIE JE DAG — obsahuje orientovany cyklus.\n";
        // Bod 2: počty vstupného grafu
        cout << "Pocet vrcholov: " << n
             << ", pocet hran: " << hranyZoSuboru.size() << "\n";
        return 0;
    }

    // Bod 1: graf je DAG
    cout << "Graf JE DAG.\n";
    // Bod 2: počty vstupného grafu
    cout << "Pocet vrcholov: " << n
         << ", pocet hran: " << hranyZoSuboru.size() << "\n";

    // Odvodíme meno výstupného súboru: "graph1.txt" → "graph1_complete.txt"
    string vystupSubor;
    size_t bodka = subor.rfind('.');
    if (bodka != string::npos)
        vystupSubor = subor.substr(0, bodka) + "_complete" + subor.substr(bodka);
    else
        vystupSubor = subor + "_complete";

    // Doplníme hrany, zapíšeme súbor a vypíšeme body 3 a 4
    doplnDAG(vystupSubor);

    return 0;
}
