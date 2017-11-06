# Truth-Table
Générateur de table de vérite avec la notation polonaise inverse.

## Utilisation
L'utilisation se fait via la ligne de commande: `./ttable "expression"`

L'expression doit être une expression ecrite via la notation polonaise inverse comme suit:
  - `"<variable_1> [variable_2] <opérateur logique>"`
La répétition de ce processus permet de former une expression logique.
Le programme va ensuite sortir la table de veritée corespondante, avec les noms des variables transmises.

## Exemples d'utilisation:

`./ttable "a b &&"`

|           |b      |a      |Resultat |
|-----------|-------|-------|-------- |
|   1:      |0      |0      |0        |
|   2:      |0      |1      |0        |
|   3:      |1      |0      |0        |
|   4:      |1      |1      |1        |


`./ttable "INTRUSION MISENMARCHE DETECTEUR && ||"`

 |         |DETECTEUR   |MISENMARCHE      |INTRUSION             |Resultat|
 |---------|------------|-----------------|----------------------|--------|
 |   1:    |         0  |           0     |        0             |0       |
 |   2:    |         0  |           0     |        1             |1       |
 |   3:    |         0  |           1     |        0             |0       |
 |   4:    |         0  |           1     |        1             |1       |
 |   5:    |         1  |           0     |        0             |0       |
 |   6:    |         1  |           0     |        1             |1       |
 |   7:    |         1  |           1     |        0             |1       |
 |   8:    |         1  |           1     |        1             |1       |

## Mot clés (nom / abréviation):
### Opérateurs
  * and (&&)
  * or (||)
  * xor (^)
  * not (!)

### Constantes
  * true
  * false
  
### Variables
  * une chaine de caractère
  * un caractère
