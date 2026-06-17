# Tests Complets pour les Tableaux et Vérification d'Erreurs

## Vue d'ensemble
Cette suite de tests contient 65 tests au total pour valider:
1. **27 tests valides** (59-85): Fonctionnalités des tableaux qui doivent compiler
2. **38 tests d'erreur** (86-123): Syntaxe invalide que le compilateur doit rejeter

---

## Tests Valides - Fonctionnalités des Tableaux (59-85)

### Initialisation et Accès Basiques
- **59_array_simple_init**: Initialiser un tableau et lire le premier élément
- **60_array_element_access**: Accéder à différents éléments d'un tableau
- **67_array_implicit_size**: Créer un tableau sans spécifier la taille (inférence)
- **68_array_single_element**: Tableau avec un seul élément

### Modification d'Éléments
- **61_array_element_modification**: Modifier un élément et le retourner
- **62_array_element_modification_complex**: Opérations complexes avec plusieurs modifications
- **83_array_zero_initialization**: Initialiser avec des zéros et modifier après

### Accès Dynamique
- **63_array_dynamic_index**: Utiliser une variable comme indice
- **64_array_dynamic_index_computed**: Index calculé (variable + expression)
- **72_array_zero_index**: Accès à l'index 0
- **71_array_last_element**: Accès au dernier élément
- **77_array_index_expr**: Expressions complexes comme indices

### Types de Données
- **65_array_char**: Tableau de caractères
- **66_array_double**: Tableau de doubles
- **75_array_mixed_types**: Plusieurs types de tableaux dans le même programme

### Opérations et Boucles
- **69_array_loop_sum**: Somme des éléments avec while
- **70_array_loop_product**: Produit des éléments avec while
- **79_array_modify_in_loop**: Modifier des éléments dans une boucle
- **80_array_accumulate**: Accumulation complexe avec division

### Expressions et Conditions
- **76_array_nested_expr**: Opérations arithmétiques sur éléments de tableau
- **78_array_if_condition**: Utiliser un élément dans une condition if
- **82_array_complex_init**: Initialiser avec expressions (1+2, 3*4, etc.)
- **84_array_negative_values**: Tableau avec nombres négatifs
- **85_array_bitwise_ops**: Opérations binaires sur éléments

### Fonctions
- **73_array_function_param**: Passer un tableau en paramètre
- **74_array_function_param_access**: Accéder aux éléments d'un tableau passé en paramètre
- **81_array_function_modify**: Fonction qui modifie un tableau

---

## Tests d'Erreur - Syntaxe Invalide (86-123)

### Erreurs de Syntaxe de Base
- **86_error_missing_semicolon**: Point-virgule manquant après `int x = 5`
- **93_error_missing_semicolon_multi**: Plusieurs déclarations sans point-virgule
- **102_error_no_semicolon_before_return**: Manque de `;` avant `return`
- **94_error_missing_paren**: Parenthèse fermante manquante dans `if (x > 3`
- **100_error_while_missing_paren**: Parenthèse manquante dans `while (x > 0 {`
- **110_error_unclosed_paren**: Parenthèse non fermée: `int x = (5 + 3;`

### Déclarations et Types
- **101_error_missing_type**: `int x y;` (pas de type)
- **103_error_missing_return_type**: `main() { }` (pas de type retour)
- **107_error_missing_return_value**: `return;` (sans valeur)

### Variables
- **87_error_undeclared_variable**: Utiliser une variable non déclarée `y`
- **88_error_redeclaration**: Redéclaration de la même variable `x`
- **106_error_decl_in_if**: Déclaration dans une condition `if` sans bloc
- **113_error_missing_function_call_parens**: `int x = helper;` (sans `()`)
- **122_error_missing_operator**: `int c = a b;` (opérateur manquant)
- **123_error_param_redeclaration**: Redéclarer un paramètre de fonction

### Fonctions et Appels
- **89_error_undefined_function**: Appeler une fonction non définie
- **90_error_missing_argument**: Pas assez d'arguments: `add(5)` pour `add(a, b)`
- **91_error_too_many_arguments**: Trop d'arguments: `add(5, 10)` pour `add(a)`
- **104_error_trailing_comma**: Virgule traînante: `func(a, b,)`
- **115_error_pointer_type_mismatch**: Passer `int` quand `int*` attendu
- **120_error_void_function_call**: Appeler une fonction `void` et l'utiliser

### Tableaux
- **95_error_return_array**: Retourner un tableau directement
- **96_error_missing_array_index**: Index manquant: `arr[]`
- **97_error_array_partial_init**: Initialisation incomplète (moins d'éléments que la taille)
- **98_error_zero_size_array**: `int arr[0]` (taille zéro)
- **105_error_array_out_of_bounds**: Accès hors limites
- **109_error_index_non_array**: Indexer un non-tableau: `double x[5]` → `x[5]`
- **111_error_array_literal_non_array**: Littéral tableau pour variable simple: `int x = {1, 2}`
- **116_error_reassign_array**: Réassigner un tableau: `arr = {1, 2}`
- **108_error_non_constant_array_size**: Taille non-constante: `int arr[x]`

### Expressions
- **92_error_invalid_expression**: Expression invalide: `return 5 +;`
- **99_error_empty_assignment**: Assignation vide: `x = ;`

### Contrôle de Flux
- **112_error_missing_else_path**: Condition `if` sans `else` (peut être rejeté si pas de retour assuré)

### Constructs Non Implémentés
- **117_error_switch_statement**: `switch` (non implémenté)
- **118_error_do_while**: `do-while` (non implémenté)
- **119_error_for_loop**: `for` (non implémenté)

### Pointeurs
- **121_error_uninitialized_pointer**: Pointeur non initialisé déréférencé

---

## Comment Exécuter les Tests

### Option 1: Utiliser le script Python fourni
```bash
python3 test_arrays.py
```

Ce script exécute tous les tests et affiche:
- Les tests valides qui compilent avec succès
- Les tests d'erreur qui sont correctement rejetés
- Un résumé final avec le nombre de tests passés/échoués

### Option 2: Tester manuellement
```bash
# Tester un test valide (devrait produire du code assembleur)
./build/ifcc tests/cases/59_array_simple_init.c

# Tester un test d'erreur (devrait afficher une erreur)
./build/ifcc tests/cases/86_error_missing_semicolon.c
```

### Option 3: Utiliser le test suite existant
```bash
make test  # Cela exécutera aussi les nouveaux tests
```

---

## Critères de Succès

✓ **Tests Valides**: Doivent compiler sans erreur
✓ **Tests d'Erreur**: Doivent être rejetés par le compilateur avec un message d'erreur

---

## Notes Importantes

1. **Inférence de taille**: Le compilateur doit pouvoir inférer la taille d'un tableau à partir du nombre d'initializers: `int arr[] = {1, 2, 3}` → taille 3

2. **Tableaux en paramètres**: Les tableaux peuvent être passés en paramètres

3. **Tableaux de chars**: Support pour `char[n]` avec initialisation par caractères

4. **Types multiples**: Support pour `int[]`, `char[]`, `double[]`

5. **Vérifications sémantiques**: Le compilateur doit vérifier:
   - Déclaration avant utilisation
   - Pas de redéclaration
   - Fonction définie avant appel
   - Nombre d'arguments correct
   - Accès à variables existantes

