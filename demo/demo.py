#!/usr/bin/env python3
"""
Demo CLI — présentation compilateur PLD
Usage : python3 demo/demo.py  (depuis la racine du projet)
"""

import subprocess
import sys
import os
import uuid

RESET   = "\033[0m"
BOLD    = "\033[1m"
DIM     = "\033[2m"
GREEN   = "\033[32m"
CYAN    = "\033[36m"
YELLOW  = "\033[33m"
BLUE    = "\033[34m"
MAGENTA = "\033[35m"
RED     = "\033[31m"

DEMO_DIR    = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(DEMO_DIR)
DOCKER_IMAGE = "ifcc-dev"
DOCKER_RUN = ["docker", "run", "--rm",
              "-v", f"{PROJECT_DIR}:/work",
              "-w", "/work",
              DOCKER_IMAGE]
TMP_DIR        = os.path.join(PROJECT_DIR, ".demo_tmp")
TMP_DIR_DOCKER = ".demo_tmp"


def _read(filename):
    with open(os.path.join(DEMO_DIR, filename)) as f:
        return f.read()


DEMOS = [
    {
        "title": "Retour de constante + addition",
        "concepts": ["Compilation basique", "ASM minimal", "Opération arithmétique"],
        "file": "01_addition.c",
    },
    {
        "title": "Précédence des opérateurs",
        "concepts": ["Précédence * avant +", "Génération de temporaires dans l'IR"],
        "file": "02_precedence.c",
    },
    {
        "title": "Unaire et parenthèses",
        "concepts": ["Unaire -", "Override de précédence par parenthèses"],
        "file": "03_unary.c",
    },
    {
        "title": "Type char et valeur ASCII",
        "concepts": ["Type char", "Littéral caractère 'A' → 65 en ASM"],
        "file": "04_char.c",
    },
    {
        "title": "Opérateurs bit à bit",
        "concepts": ["AND / OR / XOR bit à bit", "Instructions x86 and / or / xor"],
        "file": "05_bitwise.c",
    },
    {
        "title": "Comparaisons & if / else if / else",
        "concepts": ["Opérateurs de comparaison", "Blocs conditionnels dans le CFG", "Labels de branchement en ASM"],
        "file": "06_if_else_if.c",
    },
    {
        "title": "Boucle while — somme 0..4",
        "concepts": ["Boucle while", "Back-edge dans le CFG", "Compteur et accumulateur"],
        "file": "07_while.c",
    },
    {
        "title": "Contrôle de flux complexe",
        "concepts": ["while + if/else imbriqués", "CFG avec de nombreux blocs", "Multiples chemins d'exécution"],
        "file": "08_complex_flow.c",
    },
    {
        "title": "Fonctions avec paramètres ≤ 6 (registres)",
        "concepts": ["ABI x86-64 : passage par %edi, %esi...", "Portée des variables par fonction", "Appels imbriqués"],
        "file": "09_functions_regs.c",
    },
    {
        "title": "Fonctions avec 7+ paramètres (pile)",
        "concepts": ["ABI x86-64 : 7e arg+ passés sur la pile", "Gestion du cadre de pile étendu"],
        "file": "10_functions_stack.c",
    },
    {
        "title": "Code mort après return",
        "concepts": ["Détection de code mort (dead code)", "Warning compilateur", "Bloc inaccessible ignoré"],
        "file": "11_dead_code.c",
    },
    {
        "title": "Pointeurs — référencement & déréférencement",
        "concepts": ["Opérateur & (adresse)", "Opérateur * (déréférencement)", "Pointeur de pointeur **"],
        "file": "12_pointers.c",
    },
    {
        "title": "Pointeur en paramètre de fonction",
        "concepts": ["Passage de pointeur à une fonction", "Écriture via *ptr", "Passage d'adresse &x en argument"],
        "file": "13_pointer_param.c",
    },
    {
        "title": "putchar / getchar — E/S caractère",
        "concepts": ["Appel de fonctions externes (libc)", "putchar écrit sur stdout", "getchar lit depuis stdin"],
        "file": "14_putchar_getchar.c",
        "stdin": "B",
    },
]


def _filter_docker_warnings(text):
    return "\n".join(
        line for line in text.splitlines()
        if "WARNING: The requested image" not in line
    )


def clear():
    os.system("clear")


def hr(char="─", width=60, color=DIM):
    print(color + char * width + RESET)


def print_header():
    hr("═", 60, CYAN)
    print(CYAN + BOLD + "  Démo compilateur ifcc — PLD".center(60) + RESET)
    hr("═", 60, CYAN)


def print_menu():
    print_header()
    print()
    for i, demo in enumerate(DEMOS):
        num = f"{i+1:2}."
        concepts = ", ".join(demo["concepts"][:2])
        print(f"  {BOLD}{YELLOW}{num}{RESET} {demo['title']}")
        print(f"      {DIM}{concepts}{RESET}")
        print()
    hr()
    print(f"  {BOLD}{GREEN} q.{RESET} Quitter")
    print()


def show_code(code):
    print(BOLD + CYAN + "┌─ Code source " + "─" * 44 + RESET)
    for line in code.splitlines():
        print(CYAN + "│ " + RESET + line)
    print(CYAN + "└" + "─" * 59 + RESET)


def compile_and_run(demo, code):
    os.makedirs(TMP_DIR, exist_ok=True)
    uid = uuid.uuid4().hex[:8]
    c_path   = os.path.join(TMP_DIR, f"demo_{uid}.c")
    asm_path = os.path.join(TMP_DIR, f"demo_{uid}.s")
    bin_path = os.path.join(TMP_DIR, f"demo_{uid}")
    c_docker   = f"{TMP_DIR_DOCKER}/demo_{uid}.c"
    asm_docker = f"{TMP_DIR_DOCKER}/demo_{uid}.s"
    bin_docker = f"{TMP_DIR_DOCKER}/demo_{uid}"

    try:
        with open(c_path, "w") as f:
            f.write(code)

        # Étape 1 : compilation → CFG (stderr) + ASM (stdout)
        result = subprocess.run(
            DOCKER_RUN + ["./build/ifcc", "--debug-ir", c_docker],
            capture_output=True, text=True
        )
        cfg_output = _filter_docker_warnings(result.stderr)
        asm_output = result.stdout

        exit_code = None
        run_stdout = ""

        if asm_output.strip():
            with open(asm_path, "w") as f:
                f.write(asm_output)

            # Étape 2 : assemblage + linkage
            link = subprocess.run(
                DOCKER_RUN + ["gcc", "-o", bin_docker, asm_docker],
                capture_output=True, text=True
            )

            if link.returncode == 0:
                # Étape 3 : exécution
                run = subprocess.run(
                    DOCKER_RUN + [bin_docker],
                    input=demo.get("stdin", ""), capture_output=True, text=True
                )
                exit_code = run.returncode
                run_stdout = run.stdout

    finally:
        for p in [c_path, asm_path, bin_path]:
            if os.path.exists(p):
                os.unlink(p)

    return cfg_output, asm_output, exit_code, run_stdout


def show_demo(demo):
    code = _read(demo["file"])
    clear()
    print_header()
    print()
    print(BOLD + MAGENTA + f"  {demo['title']}" + RESET)
    print()

    print(BOLD + "  Notions illustrées :" + RESET)
    for concept in demo["concepts"]:
        print(f"    {GREEN}•{RESET} {concept}")
    print()

    show_code(code)
    print()

    input(DIM + "  [ Entrée pour compiler... ]" + RESET)
    print()

    cfg_output, asm_output, exit_code, run_stdout = compile_and_run(demo, code)

    # CFG
    print(BOLD + YELLOW + "┌─ CFG (Représentation Intermédiaire) " + "─" * 22 + RESET)
    if cfg_output.strip():
        for line in cfg_output.splitlines():
            print(YELLOW + "│ " + RESET + line)
    else:
        print(YELLOW + "│ " + RESET + DIM + "(aucune sortie)" + RESET)
    print(YELLOW + "└" + "─" * 59 + RESET)
    print()

    input(DIM + "  [ Entrée pour voir l'assembleur... ]" + RESET)
    print()

    # ASM
    print(BOLD + BLUE + "┌─ Assembleur x86-64 (AT&T) " + "─" * 31 + RESET)
    if asm_output.strip():
        for line in asm_output.splitlines():
            print(BLUE + "│ " + RESET + line)
    else:
        print(BLUE + "│ " + RESET + DIM + "(aucune sortie ASM — erreur de compilation)" + RESET)
    print(BLUE + "└" + "─" * 59 + RESET)
    print()

    # Résultat exécution
    if exit_code is not None:
        print(BOLD + GREEN + "┌─ Exécution " + "─" * 46 + RESET)
        if run_stdout:
            print(GREEN + "│ " + RESET + f"stdout : {repr(run_stdout)}")
        signed = exit_code if exit_code <= 127 else exit_code - 256
        note = f"  {DIM}(= {signed} signé — exit code tronqué à 8 bits){RESET}" if signed != exit_code else ""
        print(GREEN + "│ " + RESET + f"Code de retour : {BOLD}{exit_code}{RESET}{note}")
        print(GREEN + "└" + "─" * 59 + RESET)
    else:
        print(RED + "  Erreur de compilation / linkage." + RESET)

    print()
    input(DIM + "  [ Entrée pour revenir au menu ]" + RESET)


def main():
    # Vérification que le binaire est disponible dans le conteneur
    check = subprocess.run(
        DOCKER_RUN + ["./build/ifcc"],
        capture_output=True, text=True
    )
    filtered_stderr = _filter_docker_warnings(check.stderr).lower()
    if "usage:" not in filtered_stderr and check.returncode not in (0, 1):
        print(YELLOW + "  build/ifcc introuvable, lancement de `make docker`..." + RESET)
        build = subprocess.run(["make", "docker"], cwd=PROJECT_DIR)
        if build.returncode != 0:
            print(RED + "Erreur : échec du build. Lancez `make docker` manuellement." + RESET)
            sys.exit(1)

    while True:
        clear()
        print_menu()
        choice = input(f"  Choix (1-{len(DEMOS)}, q) : ").strip().lower()

        if choice == "q":
            clear()
            print("Au revoir !")
            break

        if not choice.isdigit():
            continue
        idx = int(choice) - 1
        if 0 <= idx < len(DEMOS):
            show_demo(DEMOS[idx])


if __name__ == "__main__":
    main()
