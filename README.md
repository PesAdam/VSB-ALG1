# Kompletný DAG pomocou DFS

## Obsah

1. [Čo je to graf?](#1-čo-je-to-graf)
2. [Čo je orientovaný graf?](#2-čo-je-orientovaný-graf)
3. [Čo je DAG?](#3-čo-je-dag)
4. [Čo je kompletný DAG?](#4-čo-je-kompletný-dag)
5. [Ako je graf reprezentovaný v pamäti?](#5-ako-je-graf-reprezentovaný-v-pamäti)
6. [Čo je DFS?](#6-čo-je-dfs)
7. [Ako DFS detekuje cyklus?](#7-ako-dfs-detekuje-cyklus)
8. [Čo je topologické poradie?](#8-čo-je-topologické-poradie)
9. [Ako program doplní DAG na kompletný?](#9-ako-program-doplní-dag-na-kompletný)
10. [Jednoduchý ľudský príklad od začiatku do konca](#10-jednoduchý-ľudský-príklad-od-začiatku-do-konca)
11. [Podrobné fungovanie kódu — funkcia po funkcii](#11-podrobné-fungovanie-kódu--funkcia-po-funkcii)
12. [Formát vstupného a výstupného súboru](#12-formát-vstupného-a-výstupného-súboru)
13. [Ako program spustiť](#13-ako-program-spustiť)

---

## 1. Čo je to graf?

Graf je matematická štruktúra, ktorá popisuje **vzťahy medzi vecami**. Skladá sa z:

- **Vrcholov (nodes/vertices)** — sú to „veci", môžu to byť mestá, ľudia, úlohy, čokoľvek.
- **Hrán (edges)** — sú to „vzťahy" medzi vrcholmi, teda spojenia medzi vecami.

Príklad z bežného života: mapa miest. Mestá sú vrcholy, cesty medzi nimi sú hrany.

---

## 2. Čo je orientovaný graf?

V **orientovanom grafe** má každá hrana **smer** — teda hrana vedie **od** jedného vrcholu **do** druhého.

Myslite na to ako na **jednosmerné ulice**. Ak existuje jednosmerka z Bratislavy do Trnavy, neznamená to, že sa dá ísť aj z Trnavy do Bratislavy.

V zápise: hrana `u → v` znamená, že z vrcholu `u` sa viem dostať do vrcholu `v`, ale **nie naopak** (pokiaľ tam nie je ešte jedna hrana `v → u`).

---

## 3. Čo je DAG?

**DAG** = **D**irected **A**cyclic **G**raph = Orientovaný Acyklický Graf.

Je to orientovaný graf, ktorý **neobsahuje žiaden cyklus**.

**Cyklus** je situácia, keď sa z nejakého vrcholu viem dostať späť do samého seba. Napríklad:
```
0 → 1 → 2 → 0   ← toto JE cyklus, graf NIE JE DAG
```

```
0 → 1 → 3
0 → 2 → 3        ← toto NIE JE cyklus, graf JE DAG
```

**Príklady DAGov z reálneho života:**

- **Rozvrh úloh** — niektoré úlohy musia byť dokončené pred inými. Napríklad: najprv kúp múku, potom urob cesto, potom upeč chlieb. Nemôžeš upiecť chlieb skôr, ako urobíš cesto. Nikdy sa nedostaneme do situácie, že úloha A závisí od B, B závisí od C a C závisí od A — to by bol cyklus a rozvrh by bol nesplniteľný.
- **Zostavovací systém** — kód sa nedá skompilovať, kým nie sú skompilovné jeho závislosti.
- **Akademické predmety** — Matematiku 2 nemôžeš absolvovať skôr ako Matematiku 1.

---

## 4. Čo je kompletný DAG?

**Kompletný DAG** je DAG, do ktorého sa nedá pridať žiadna ďalšia orientovaná hrana bez toho, aby sme buď:
- vytvorili cyklus, alebo
- pridali duplicitnú hranu (tú istú hranu, čo už existuje).

Inými slovami — medzi každou dvojicou vrcholov existuje práve jedna hrana (v jednom smere). Celkový počet hrán v kompletnom DAG s `n` vrcholmi je vždy:

$$|E| = \frac{n \cdot (n-1)}{2}$$

Pre `n = 4` vrcholy je to `4·3/2 = 6` hrán.

Prečo? Pretože existuje `n·(n-1)/2` dvojíc vrcholov a medzi každou dvojicou musí viesť práve jedna hrana (v niektorom smere), aby sme nepridali cyklus ani duplicitu.

---

## 5. Ako je graf reprezentovaný v pamäti?

Program používa **zoznam susedov** (anglicky *adjacency list*).

V kóde je to: `vector<vector<int>> adj`

- `adj[u]` je zoznam všetkých vrcholov, do ktorých vedie hrana **z** vrcholu `u`.
- Napríklad ak existujú hrany `0→1` a `0→3`, potom `adj[0] = {1, 3}`.

**Prečo zoznam susedov a nie matica susednosti?**

Matica susednosti je tabuľka `n×n` kde na pozícii `[u][v]` je 1 ak hrana existuje, inak 0. Je prehľadná, ale zaberá veľa pamäte `O(n²)` aj keď je hrán málo.

Zoznam susedov zaberá len toľko miesta, koľko hrán skutočne existuje — `O(n + |E|)`. Pre riedke grafy (málo hrán) je oveľa efektívnejší. Zároveň je prirodzenejší pre DFS, pretože pri prechode z vrcholu `u` stačí prejsť cez `adj[u]`.

---

## 6. Čo je DFS?

**DFS** = **D**epth-**F**irst **S**earch = Prehľadávanie do hĺbky.

Je to spôsob, ako systematicky navštíviť každý vrchol grafu. Princíp je jednoduchý:

> Keď prídeme do vrcholu, ihneď ideme čo najhlbšie — teda nasledujeme hrany ďalej, kým sa nedostaneme do vrcholu, z ktorého nikam ďalej ísť nemôžeme. Potom sa vrátime a skúsime iného suseda.

Toto správanie je rekurzívne a prirodzene využíva **zásobník volania funkcií** (call stack).

**Analógia z reálneho života:** Predstavte si, že blúdite v bludisku. Vždy keď prídete na rozcestie, vyberiete si prvý možný smer a idete ním čo najďalej. Keď narazíte na slepú uličku, vrátite sa na posledné rozcestie a vyskúšate ďalší smer, ktorý ste ešte neskúšali.

### Stavy vrcholov počas DFS

Každý vrchol má v každom momente jeden z troch stavov — uložené v poli `stav[]`:

| Číslo | Farba | Meaning |
|-------|-------|---------|
| `0` | Biely | Vrchol sme ešte vôbec nenavštívili. |
| `1` | Sivý | Vrchol sme práve navštevujeme — je „otvorený" na zásobníku rekurzie. |
| `2` | Čierny | Vrchol sme úplne dokončili — spracovali sme aj všetkých jeho susedov. |

---

## 7. Ako DFS detekuje cyklus?

Toto je kľúčová myšlienka celého algoritmu.

Keď sa počas DFS pohybujeme z vrcholu `u` do jeho suseda `v`, pozrieme sa na stav `v`:

- **`stav[v] == 0` (biely):** `v` sme ešte nenavštívili. Normálne pokračujeme DFS rekurzívne do `v`.
- **`stav[v] == 1` (sivý):** `v` je práve na zásobníku — teda sme sa dostali **späť do vrcholu, z ktorého sme prišli** (prípadne vyššie v rekurzii). Toto je **spätná hrana** a znamená, že sme našli **cyklus**!
- **`stav[v] == 2` (čierny):** `v` je už dokončený. Táto hrana nie je problém — ide dopredu alebo do strany, nie späť.

**Prečo sivý = cyklus?** Sivý vrchol znamená, že sme ním práve prechádzali a ešte sme z neho nevyšli. Ak z neho vedie cesta (cez iné vrcholy) späť k sebe samému, znamená to presne to, čo je cyklus — kruhová závislosť.

**Príklad krokov DFS pre graf `0→1→2→0`:**
```
dfs(0): stav[0] = 1 (sivý)
  dfs(1): stav[1] = 1 (sivý)
    dfs(2): stav[2] = 1 (sivý)
      sused 0: stav[0] == 1  →  CYKLUS! cyklus = true, return
```

---

## 8. Čo je topologické poradie?

Topologické poradie (anglicky *topological order*) je také usporiadanie vrcholov, kde pre každú hranu `u → v` platí, že `u` sa nachádza **pred** `v`.

Inými slovami — usporiadame vrcholy tak, aby všetky šípky smerovali „dopredu" (zľava doprava).

**Príklad:**

Graf: `0→2`, `1→2`, `2→3`

Možné topologické poradie: `0, 1, 2, 3` alebo `1, 0, 2, 3`.

V DAG vždy topologické poradie existuje. V grafe s cyklom neexistuje (preto ho musíme najprv otestovať).

**Ako DFS buduje topologické poradie?**

Vrchol pridáme do zoznamu `topo[]` až vtedy, keď sú **všetci jeho nástupcovia** (celý podstrom za ním) dokončení — teda keď ho nastavujeme na čierny (`stav[u] = 2`). DFS ich pridáva od „najhlbšieho" po „najplytší", čiže v **opačnom** topologickom poradí. Preto na konci zavoláme `reverse()`.

---

## 9. Ako program doplní DAG na kompletný?

Ak sme overili, že graf je DAG a máme topologické poradie, doplnenie je priamočiare:

1. Uložíme všetky **existujúce hrany** do množiny `set<pair<int,int>>`, aby sme v čase O(log n) vedeli, ktoré hrany už sú.
2. Prejdeme **všetky dvojice indexov** `(i, j)` kde `i < j` v poli `topo[]`. Na pozícii `i` je vrchol `u`, na pozícii `j` je vrchol `v` — a keďže `i < j`, vieme že `u` je pred `v` v topologickom poradí.
3. Pre každú takúto dvojicu skontrolujeme, či hrana `u → v` existuje.
4. Ak nie, vložíme ju do množiny hrán.
5. Na konci zapíšeme celú množinu hrán do výstupného súboru.

Pretože pridávame iba hrany v smere topologického poradia, **nikdy nemôžeme vytvoriť cyklus** — všetky nové hrany idú „dopredu".

---

## 10. Jednoduchý ľudský príklad od začiatku do konca

Predstavte si, že plánujete **prípravu raňajok** a máte tieto úlohy:

| Číslo | Úloha |
|-------|-------|
| 0 | Kúp vajíčka |
| 1 | Kúp chlieb |
| 2 | Uvar vajíčka |
| 3 | Naporcuj chlieb |
| 4 | Zjedz raňajky |

Závislosti (hrany):
- `0 → 2` (Najprv kúp vajíčka, potom ich uvar)
- `1 → 3` (Najprv kúp chlieb, potom ho naporcuj)
- `2 → 4` (Najprv uvar vajíčka, potom jedz)
- `3 → 4` (Najprv naporcuj chlieb, potom jedz)

Vstupný súbor:
```
0 2
1 3
2 4
3 4
```

### Krok 1 — Načítanie

Program prečíta súbor riadok po riadku. Zistí, že najväčší index vrcholu je 4, teda `n = 5`. Vytvorí zoznam susedov:
```
adj[0] = {2}
adj[1] = {3}
adj[2] = {4}
adj[3] = {4}
adj[4] = {}
```

### Krok 2 — Je to DAG? (DFS)

Program inicializuje všetky stavy na 0 (biele) a spustí DFS:

```
dfs(0): stav[0]=1
  dfs(2): stav[2]=1
    dfs(4): stav[4]=1
      žiadni susedia → stav[4]=2, topo=[4]
    stav[2]=2, topo=[4,2]
  stav[0]=2, topo=[4,2,0]
dfs(1): stav[1]=1
  dfs(3): stav[3]=1
    sused 4: stav[4]==2 (čierny) → OK, nie je cyklus
    stav[3]=2, topo=[4,2,0,3]
  stav[1]=2, topo=[4,2,0,3,1]
```

Po `reverse()`: `topo = [1, 3, 0, 2, 4]`

Žiadna spätná hrana nebola nájdená → **graf JE DAG**.

### Krok 3 — Stdout výpis (body 1 a 2)

```
Graf JE DAG.
Pocet vrcholov: 5, pocet hran: 4
```

### Krok 4 — Doplnenie na kompletný DAG

Existujúce hrany: `{0→2, 1→3, 2→4, 3→4}` — 4 hrany.

Pre `n=5` je cieľ `5·4/2 = 10` hrán.

Program prechádza dvojice indexov `(i,j)` v `topo = [1,3,0,2,4]`:

| i | j | u=topo[i] | v=topo[j] | Existuje? | Akcia |
|---|---|-----------|-----------|-----------|-------|
| 0 | 1 | 1 | 3 | ÁNO | nič |
| 0 | 2 | 1 | 0 | NIE | pridáme `1→0` |
| 0 | 3 | 1 | 2 | NIE | pridáme `1→2` |
| 0 | 4 | 1 | 4 | NIE | pridáme `1→4` |
| 1 | 2 | 3 | 0 | NIE | pridáme `3→0` |
| 1 | 3 | 3 | 2 | NIE | pridáme `3→2` |
| 1 | 4 | 3 | 4 | ÁNO | nič |
| 2 | 3 | 0 | 2 | ÁNO | nič |
| 2 | 4 | 0 | 4 | NIE | pridáme `0→4` |
| 3 | 4 | 2 | 4 | ÁNO | nič |

Pridaných 6 hrán. Celkom: 4 + 6 = **10** = `5·4/2` ✓

### Krok 5 — Stdout výpis (body 3 a 4) a zápis do súboru

```
Ocakavany pocet hran kompletneho DAG: 10
Kompletny DAG — pocet vrcholov: 5, pocet hran: 10
Kompletny DAG zapísany do suboru: vstup_complete.txt
```

Súbor `vstup_complete.txt` obsahuje všetkých 10 hrán, každú na samostatnom riadku vo formáte `u v`.

---

## 11. Podrobné fungovanie kódu — funkcia po funkcii

### Globálne premenné

```cpp
int n;                    // počet vrcholov (zistí sa pri načítaní)
vector<vector<int>> adj;  // zoznam susedov: adj[u] = zoznam vrcholov kam vedie hrana z u
vector<int> stav;         // stav každého vrcholu: 0=biely, 1=sivý, 2=čierny
vector<int> topo;         // topologické poradie — výsledok DFS
bool cyklus;              // príznak: true ak bol nájdený cyklus
```

Všetky premenné sú globálne, čo znamená, že sú dostupné vo všetkých funkciách bez nutnosti ich odovzdávať ako parametre. To zjednodušuje kód a uľahčuje pochopenie toku dát.

---

### Funkcia `dfs(int u)`

Toto je jadro celého algoritmu. Funkcia sa volá rekurzívne — volá samu seba pre každého nenavštíveného suseda.

```
vstup:  vrchol u, ktorý chceme navštíviť
výstup: (žiadny) — vedľajšie efekty: zmena stav[], pridanie do topo[], nastavenie cyklus
```

**Postup krok po kroku:**

1. Nastavíme `stav[u] = 1` (sivý) — začíname spracovávať `u`.
2. Pre každého suseda `v` vrcholu `u` (teda pre každú hranu `u → v`):
   - Ak `stav[v] == 1` (sivý) → nájdená spätná hrana = cyklus. Nastavíme `cyklus = true` a `return`.
   - Ak `stav[v] == 0` (biely) → rekurzívne zavoláme `dfs(v)`.
   - Ak po rekurzii `cyklus == true` → okamžite `return` (nepokračujeme zbytočne).
   - Ak `stav[v] == 2` (čierny) → vrchol je hotový, pokračujeme na ďalšieho suseda.
3. Nastavíme `stav[u] = 2` (čierny) — všetci susedia spracovaní.
4. Pridáme `u` do `topo[]` — vrchol je kompletne hotový.

**Prečo pridávame do `topo[]` až na konci?** Pretože topologické poradie vyžaduje, aby bol vrchol v zozname až **za** všetkými svojimi nástupcami (z pohľadu DFS). Keď pridávame na konci, všetci nástupcovia už boli pridaní pred nami, takže po `reverse()` budeme my pred nimi — čo je správne topologické poradie.

---

### Funkcia `jeDAG()`

```
vstup:  (žiadny) — používa globálne n, adj
výstup: true ak je graf DAG, false ak obsahuje cyklus
        vedľajší efekt: vyplní topo[] topologickým poradím (ak je DAG)
```

**Postup:**

1. Inicializuje `stav[]` — všetky vrcholy na 0 (biele).
2. Vynuluje `cyklus = false` a vyprázdni `topo[]`.
3. Prechádza všetky vrcholy od `0` do `n-1`. Pre každý biely vrchol spustí `dfs(i)`. Toto je dôležité pre **nesúvislé grafy** — také, kde nie všetky vrcholy sú navzájom dosažiteľné. Ak by sme spustili DFS len z vrcholu 0, niektoré vrcholy by sme nikdy nenavštívili.
4. Po skončení všetkých DFS zavolá `reverse(topo)` — otočí poradie, pretože DFS pridáva vrcholy od najhlbšieho po najplytší.
5. Vráti `!cyklus`.

---

### Funkcia `doplnDAG(const string& vystupSubor)`

```
vstup:  meno výstupného súboru
výstup: (žiadny) — vedľajšie efekty: vypíše body 3 a 4 na stdout, zapíše kompletný DAG do súboru
```

**Postup:**

1. Vytvorí `set<pair<int,int>> hrany` — množinu existujúcich hrán. `set` v C++ je zoradená množina, čo znamená, že hrany budú v súbore zapísané v usporiadanom poradí (najprv podľa `u`, potom podľa `v`).
2. Naplní množinu zo `adj[]` — prejde každý vrchol a každého jeho suseda.
3. Prechádza všetky dvojice `(i, j)` kde `i < j` v poli `topo[]`. Vyberieme `u = topo[i]` a `v = topo[j]`. Pretože `i < j`, vieme že `u` je topologicky pred `v`. Hrana `u → v` teda nikdy nevytvorí cyklus.
4. Ak hrana `{u, v}` nie je v množine, vloží ju tam. (Pôvodné hrany tam už sú z kroku 2.)
5. Otvorí výstupný súbor a zapíše každú hranu z množiny ako `u v\n`.
6. Vypíše na stdout: očakávaný počet hrán (bod 3) a skutočný počet vrcholov a hrán (bod 4).

**Prečo `set` a nie `vector`?** Pretože `set` automaticky zaručuje, že každá hrana je uložená **práve raz** — nevzniknú duplicity. Zároveň `set::count({u,v})` v čase O(log n) overí existenciu hrany. Ak by sme použili `vector`, museli by sme prehľadávať celý zoznam v čase O(|E|) pre každú novú hranu.

---

### Funkcia `main()`

Hlavná funkcia riadi celý beh programu v tomto poradí:

1. **Načítanie mena súboru** — `cin >> subor`. Používateľ zadá meno vstupného súboru.
2. **Otvorenie súboru** — `ifstream f(subor)`. Ak sa súbor nedá otvoriť, program skončí s chybou.
3. **Načítanie hrán** — `while (f >> u >> v)`. Čítame dvojice čísel zo súboru. Zároveň sledujeme `maxIdx` — najväčší index vrcholu, ktorý sme videli.
4. **Výpočet `n`** — `n = maxIdx + 1`. Vrcholy sú číslované od 0, takže ich je `maxIdx + 1`.
5. **Zostavenie `adj[]`** — pre každú načítanú hranu `(from, to)` pridáme `to` do `adj[from]`.
6. **Overenie DAG** — `jeDAG()`. Ak vráti false, vypíšeme hlásenie (body 1 a 2) a skončíme.
7. **Výpis** — body 1 a 2 na stdout.
8. **Odvdenie mena výstupného súboru** — nájdeme poslednú bodku v mene vstupného súboru pomocou `rfind('.')` a vložíme `_complete` pred príponu. Napríklad `graph1.txt → graph1_complete.txt`.
9. **Doplnenie a zápis** — `doplnDAG(vystupSubor)`. Táto funkcia doplní hrany, zapíše súbor a vypíše body 3 a 4.

---

## 12. Formát vstupného a výstupného súboru

### Vstupný súbor

Každý riadok obsahuje dve celé čísla oddelené medzerou — `u v` — čo znamená, že existuje hrana `u → v`. Vrcholy sú číslované od `0`. Program automaticky zistí počet vrcholov z najväčšieho čísla v súbore.

**Príklad (`vstup.txt`):**
```
0 1
2 3
0 2
```

### Výstupný súbor

Automaticky sa vytvorí vedľa vstupného súboru s príponou `_complete` pred príponou. Napríklad `vstup.txt → vstup_complete.txt`. Má rovnaký formát ako vstup — každý riadok je `u v`. Obsahuje **všetky** hrany kompletného DAG (pôvodné aj nové).

**Príklad (`vstup_complete.txt`):**
```
0 1
0 2
0 3
1 3
2 3
...
```

---

## 13. Ako program spustiť

### Kompilácia

```bash
g++ -std=c++17 -Wall -o dag main.cpp
```

### Spustenie

```bash
./dag
```

Program sa opýta na názov súboru:
```
Zadaj meno suboru: tests/graph2.txt
```

### Ukážkový výstup pre DAG

```
Graf JE DAG.
Pocet vrcholov: 20, pocet hran: 34
Ocakavany pocet hran kompletneho DAG: 190
Kompletny DAG — pocet vrcholov: 20, pocet hran: 190
Kompletny DAG zapísany do suboru: tests/graph2_complete.txt
```

### Ukážkový výstup pre graf s cyklom

```
Graf NIE JE DAG — obsahuje orientovany cyklus.
Pocet vrcholov: 3, pocet hran: 3
```

V tomto prípade sa výstupný súbor **nevytvorí** — nemá zmysel doplňovať hrany do grafu, ktorý nie je DAG.
