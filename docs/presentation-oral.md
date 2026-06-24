---
marp: true
theme: default
paginate: true
title: "ifcc — Oral"
style: |
  section {
    font-size: 24px;
    padding: 40px 55px;
    justify-content: flex-start;
  }
  h1 { font-size: 42px; }
  h2 { font-size: 32px; margin-bottom: 0.4em; }
  pre, code { font-size: 0.8em; }
  pre { line-height: 1.25; margin: 0.4em 0; }
  table { font-size: 0.9em; }
  blockquote { font-size: 0.85em; }
  li { margin: 0.15em 0; }
---

# ifcc — Mini-compilateur C

**Sous-ensemble de C → assembleur x86-64, écrit en C++ / ANTLR4**

Projet PLD-COMP — INSA Lyon

Luka Maret · Valentin Dury · Sarah Ripoche · Adina Valkova · Louis Zyzelewicz · Clément Dubois

---

## Ce qu'on a construit

Un vrai pipeline de compilation, en **3 passes** séparées :

```
source .c → Lexer/Parser ANTLR → arbre
          → SymbolTableVisitor   (passe 1 : sémantique)
          → IRGeneratorVisitor   (passe 2 : IR = graphe de flot)
          → X86Backend           (passe 3 : assembleur)
          → x86-64 AT&T sur stdout
```

Critère de réussite **différentiel** : un test passe si `ifcc` et **GCC sont d'accord**
(même code de sortie, ou tous deux rejettent le programme).

> **122 cas de test** automatisés (`tests/ifcc-test.py`).

---

## Le langage supporté

- **Types** : `int`, `char`, `void`, pointeurs (`int*`, `int**`).
- **Arithmétique** : `+ - * / %`, unaire `-`, précédence et parenthèses.
- **Bit-à-bit** : `& | ^` · **Comparaisons** : `< > <= >= == !=`.
- **Variables** : déclarations multiples, init, affectations chaînées.
- **Contrôle de flux** : `if / else / else if`, `while` (imbriqués).
- **Fonctions** : définition, paramètres (>6 ⇒ pile), récursion, `putchar`/`getchar`.
- **Pointeurs** : `&x`, `*p`, `**pp`, écriture `*p = …`.
- **Tableaux** : déclaration, init `{…}` (partielle), lecture/écriture indexée.

---

## Passe 1 — Analyse sémantique

`SymbolTableVisitor` construit les tables **avant** toute génération de code :

- une table `map<string, Variable>` **par fonction**, + une table des fonctions
  (type de retour + types des paramètres) ;
- erreurs détectées : redéfinition, variable non déclarée, `void`, mauvais nombre
  d'arguments, fonction non définie, absence de `main`.

```c
int main() { int a; int a; }   // Error: 'a' is already declared
int main() { return f(1, 2); } // Error: mauvais nombre d'arguments
```

---

## Passe 2 — L'IR : un graphe de flot de contrôle

```
ControlFlowGraph ─owns→ [Block, Block, …]
Block            ─owns→ [Instruction, …] + arêtes true/false
```

- Chaque `visitXxx` **renvoie le nom de la variable** contenant son résultat
  → composition naturelle des expressions.
- Temporaires `$temp0, $temp1…` (`CFG::addTempVariable`), `$return` pour le retour.
- **21 classes d'instructions** : `Add, Subtract, Copy, LoadConstant, Negate,
  Bitwise*, comparaisons, CallFunction, Reference, DereferenceRead/Write…`

---

## Passe 2 — Optimisations à la volée

**Pliage de constantes** — évaluées à la compilation :
```c
int x = 2 + 3 * 4;   //  →  x = 14   (une seule constante)
```

**Simplifications algébriques** — neutres / absorbants :
```c
x + 0, x * 1  → x        x * 0, x & 0  → 0
```

**Code mort** — après `return`, bloc inatteignable détecté et signalé :
```
warning: 1 unreachable code block(s) detected
```

---

## Passe 3 — Backend & double-dispatch

Le backend ignore tout de l'IR, l'IR ignore tout de l'assembleur :

```cpp
instruction->generate(backend, output);          // Block::generateASM
void Add::generate(Backend &b, ostream &o) { b.emit(this, o); } // dispatch
// → résolution sur le type concret : X86Backend::emit(Add*)
```

→ Ajouter une **cible** = une nouvelle classe `Backend` (un `ARMBackend` existe).

Conventions **System V** : 6 premiers args en registres (`%edi, %esi…`), pile au-delà,
cadre aligné sur 16 octets, épilogue `leave ; ret`.

---

## Exemple : une instruction émise

Toutes les opérandes vivent sur la pile → on **charge, calcule, range** :

```cpp
void X86Backend::emit(Add *instr, ostream &o) {
    o << "    movl " << varToLocation(instr->getLeft(),  cfg) << ", %eax\n";
    o << "    addl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    o << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}
```

Comparaison `a < b` → `cmpl ; setl %al ; movzbl %al, %eax` (résultat 0 ou 1).

---

## Tableaux (1/2) — l'idée

Pas de nouvelle instruction IR : un tableau est une **zone contiguë** réutilisant
`DereferenceRead` / `DereferenceWrite`, avec un **index** transmis au backend.

```c
int arr[5] = {64, 34, 25, 12, 22};   // init partielle légale (reste à 0)
int x = arr[i];                      // lecture indexée
arr[j + 1] = arr[j];                 // écriture indexée
```

```cpp
// visitTable_expression_read_value : a[i]
std::string indexVar = asString(visit(ctx->expression()));
block->addInstruction(
    new DereferenceRead(block, elementType, tmp, tableName, 0, indexVar));
```

---

## Tableaux (2/2) — l'assembleur

Un **tableau fournit son adresse** (`leaq`), un **pointeur fournit
son contenu** (`movq`). Puis adressage indexé `(base, index, scale)` :

```cpp
if (cfg->getVar(src).pointerDepth > 0)
    o << "    movq " << loc(src) << ", %rax\n";   // pointeur
else
    o << "    leaq " << loc(src) << ", %rax\n";   // tableau (zone contiguë)

o << "    movslq " << loc(index) << ", %rcx\n";   // index 32→64 bits
o << "    movl (%rax,%rcx," << size << "), %eax\n"; // base + index*scale
```

---

## Exemple bout-en-bout

```c
int main() { int x = 2 + 3 * 4; return x; }
```

IR (pliage) :
```
LoadConstant $temp0 = 14      Copy x = $temp0      Copy $return = x
```

Assembleur :
```asm
main:
    pushq %rbp ; movq %rsp, %rbp ; subq $16, %rsp
    movl $14, -8(%rbp)       # x
    movl -8(%rbp), %eax      # $return = x
    leave ; ret              # → code de sortie 14
```

---

## Bilan & points de conception

- **Séparation stricte** front-end / IR (CFG) / back-end.
- **Double-dispatch** pour découpler instructions et cibles.
- Optimisations réelles : **pliage**, simplifications, **code mort**.
- Tableaux intégrés **sans nouvelle instruction** (réutilisation + adressage indexé).
- **122 tests** différentiels vs GCC · build multi-plateforme (Makefile + Docker).
