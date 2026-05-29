# ifcc — mini-compilateur C

Compilateur d'un sous-ensemble de C, écrit en C++ avec ANTLR4. Le pipeline :

```
source .c  →  lexer/parser ANTLR  →  arbre  →  SymbolTableVisitor  →  AsmGeneratorVisitor  →  assembleur (stdout)
```

Deux façons de travailler : **Docker** (zéro install, recommandé pour éviter les
galères de config entre machines) ou **natif** (installation locale d'ANTLR).

---

## Mode Docker (recommandé)

Prérequis : seulement [Docker](https://docs.docker.com/get-docker/).

Le code est édité normalement sur ta machine ; seule la compilation tourne dans
le conteneur (le repo est monté via bind mount). L'image embarque g++, Java,
Python 3 et ANTLR 4.13.2 aux chemins attendus par le Makefile.

```bash
make docker-build   # construit l'image ifcc-dev (une fois ; ~quelques min)
make docker         # équivalent de `make`        (build du compilateur)
make docker-test    # équivalent de `make test`   (build + tous les tests)
make docker-clean   # équivalent de `make clean`  (supprime build/ et generated/)
```

Les cibles `docker*` reconstruisent l'image au préalable (instantané grâce au cache).

> **Mac Apple Silicon** : Docker tourne en arm64, donc la branche assembleur
> `__aarch64__` est testée. Pour forcer le chemin x86-64, ajoute
> `--platform linux/amd64` au `docker build`/`docker run`.

> **Ne mélange pas** `make` natif et `make docker-*` sans `make clean` entre les
> deux : `build/` (objets Linux vs macOS) et `generated/` seraient incompatibles.
> Les deux dossiers sont gitignorés.

---

## Mode natif

Prérequis : `g++` (C++17), `make`, `python3`, Java (pour ANTLR), et le runtime
C++ ANTLR 4.13.2 + le jar. Les chemins sont déclarés par plateforme dans
`config/` (le Makefile détecte automatiquement la bonne config via `uname`) :

- `config/config-macos.mk` — macOS Apple Silicon (install via Homebrew)
- `config/config-wsl-2025.mk` — Linux / WSL x86-64
- `config/config-IF501.mk` — salles IF501

Adapte le fichier correspondant si tes chemins ANTLR diffèrent. Puis :

```bash
make          # build → build/ifcc
make test     # build + lance tous les tests de tests/cases/
make clean    # supprime build/, generated/, ifcc-test-output/
```

---

## Lancer un test à la main

```bash
./build/ifcc tests/cases/1_return42.c              # affiche l'assembleur sur stdout
python3 tests/ifcc-test.py tests/cases/1_return42.c # compare le résultat avec GCC
```

Un test passe quand `ifcc` et GCC sont d'accord : soit les deux acceptent le
programme et produisent le même code de retour, soit les deux le rejettent.

## Visualiser l'arbre syntaxique

```bash
make gui FILE=tests/cases/1_return42.c   # fenêtre graphique (nécessite Java)
```
