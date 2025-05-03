# Collections multi-valeurs

argus offre un support puissant pour les collections multi-valeurs à travers les tableaux et les mappages, vous permettant de gérer des scénarios complexes de ligne de commande que les bibliothèques traditionnelles d'analyse d'arguments ont du mal à traiter.

!!! abstract "Aperçu"
    Ce guide couvre des techniques avancées pour travailler avec les collections :
    
    - **Options de tableau et de mapping** - Collecte de valeurs multiples et de paires clé-valeur
    - **API d'itérateur** - Parcours et filtrage efficaces
    - **Drapeaux de collection** - Tri, unicité et organisation
    - **Optimisation des performances** - Techniques pour gérer de grandes collections
    - **Cas d'utilisation avancés** - Exemples et modèles concrets
    
    Pour une utilisation de base, consultez le guide [Collections multi-valeurs](../guide/multi-values.md).

## Options de tableau

Les options de tableau permettent aux utilisateurs de fournir plusieurs valeurs pour une seule option, soit à travers plusieurs occurrences soit par des listes séparées par des virgules.

### Types de tableaux pris en charge

argus prend en charge ces types de tableaux :

- `OPTION_ARRAY_STRING` : Tableau de chaînes
- `OPTION_ARRAY_INT` : Tableau d'entiers (avec prise en charge des plages)
- `OPTION_ARRAY_FLOAT` : Tableau de valeurs à virgule flottante

### Définition des options de tableau

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Tableau de chaînes simple sans drapeaux
    OPTION_ARRAY_STRING('n', "names", HELP("Liste de noms")),
    
    // Tableau d'entiers avec tri et unicité
    OPTION_ARRAY_INT('i', "ids", HELP("Liste d'identifiants"),
                    FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Tableau de flottants avec indice personnalisé
    OPTION_ARRAY_FLOAT('f', "factors", HELP("Facteurs d'échelle"),
                      HINT("FLOAT"))
)
```

### Fournir des valeurs de tableau

Les utilisateurs peuvent fournir des valeurs de tableau de plusieurs façons :

=== "Occurrences multiples d'option"
    ```bash
    # Chaque occurrence d'option ajoute au tableau
    ./program --names=john --names=alice --names=bob
    ```

=== "Listes séparées par des virgules"
    ```bash
    # Valeurs séparées par des virgules
    ./program --names=john,alice,bob
    ```

=== "Approches mixtes"
    ```bash
    # Approches combinées
    ./program --names=john,alice --names=bob
    ```

=== "Syntaxe abrégée"
    ```bash
    # Variantes d'options courtes
    ./program -n john -n alice -n bob
    ./program -njohn,alice,bob
    ```

### Syntaxe de plage d'entiers

Pour les tableaux d'entiers, argus prend en charge une syntaxe de plage spéciale :

```bash
# Les plages s'étendent pour inclure toutes les valeurs dans la plage
./program --ids=1-5,10,15-20
# or
./program --ids=1:5,10,15:20
# Équivalent à : --ids=1,2,3,4,5,10,15,16,17,18,19,20
```

!!! tip "Gestion des plages"
    La syntaxe de plage est particulièrement utile pour spécifier des plages de ports, des IDs de séquence, ou d'autres séquences numériques sans avoir à taper chaque valeur individuellement.

## Options de mapping

Les options de mapping permettent aux utilisateurs de fournir des paires clé-valeur, permettant une configuration structurée via des arguments de ligne de commande.

### Types de mappings pris en charge

argus prend en charge ces types de mappings :

| Type | Macro | Valeurs | Exemple d'utilisation |
|------|-------|--------|---------------|
| Mapping de chaînes | `OPTION_MAP_STRING` | Chaînes de texte | `--env=USER=alice,HOME=/home` |
| Mapping d'entiers | `OPTION_MAP_INT` | Nombres entiers | `--ports=http=80,https=443` |
| Mapping de flottants | `OPTION_MAP_FLOAT` | Nombres à virgule flottante | `--scales=width=0.5,height=1.5` |
| Mapping de booléens | `OPTION_MAP_BOOL` | Valeurs booléennes | `--features=debug=true,cache=false` |

### Gestion des valeurs booléennes

Pour les mappings booléens (`OPTION_MAP_BOOL`), les valeurs sont analysées selon les conventions suivantes :

| Valeurs vraies | Valeurs fausses |
|-------------|--------------|
| "true", "yes", "1", "on", "y" | "false", "no", "0", "off", "n" |

Toutes les valeurs sont insensibles à la casse, donc "True", "TRUE", et "true" sont équivalentes.

## Drapeaux de collection

argus fournit des drapeaux spéciaux pour modifier la façon dont les collections sont traitées :

### Drapeaux de tableau

| Drapeau | Description | Exemple |
|------|-------------|---------|
| `FLAG_SORTED` | Trier les valeurs du tableau | `OPTION_ARRAY_STRING('t', "tags", "Tags", FLAGS(FLAG_SORTED))` |
| `FLAG_UNIQUE` | Supprimer les valeurs en double | `OPTION_ARRAY_INT('p', "ports", "Ports", FLAGS(FLAG_UNIQUE))` |

Lorsque les deux sont utilisés ensemble, les doublons sont supprimés après le tri :

```c
OPTION_ARRAY_INT('i', "ids", "IDs", FLAGS(FLAG_SORTED | FLAG_UNIQUE))
```

### Drapeaux de mapping

| Drapeau | Description | Exemple |
|------|-------------|---------|
| `FLAG_SORTED_KEY` | Trier les entrées de mapping par clé | `FLAGS(FLAG_SORTED_KEY)` |
| `FLAG_SORTED_VALUE` | Trier les entrées de mapping par valeur | `FLAGS(FLAG_SORTED_VALUE)` |
| `FLAG_UNIQUE_VALUE` | Assurer qu'il n'y a pas de valeurs en double | `FLAGS(FLAG_UNIQUE_VALUE)` |

!!! warning "Priorité des drapeaux"
    Lorsque `FLAG_SORTED_KEY` et `FLAG_SORTED_VALUE` sont tous deux spécifiés, `FLAG_SORTED_KEY` a la priorité.

## Accès aux collections

argus fournit plusieurs façons d'accéder aux données de collection, chacune avec ses propres avantages :

### Accès direct

L'approche la plus directe est l'accès direct aux tableaux ou mappages de valeurs :

```c
// Obtenir le tableau entier
argus_value_t *names_array = argus_get(argus, "names").as_array;
size_t names_count = argus_count(argus, "names");

// Accéder directement aux éléments du tableau
for (size_t i = 0; i < names_count; i++) {
    const char* name = names_array[i].as_string;
    printf("Nom %zu : %s\n", i + 1, name);
}

// Obtenir le mapping entier
argus_pair_t *env_map = argus_get(argus, "env").as_map;
size_t env_count = argus_count(argus, "env");

// Accéder directement aux entrées du mapping
for (size_t i = 0; i < env_count; i++) {
    const char* key = env_map[i].key;
    const char* value = env_map[i].value.as_string;
    printf("%s = %s\n", key, value);
}
```

### Fonctions d'aide pour l'accès aux éléments

argus fournit des fonctions d'aide pour un accès plus pratique à des éléments spécifiques :

```c
// Obtenir un élément spécifique du tableau par indice
const char* first_name = argus_array_get(argus, "names", 0).as_string;
int second_id = argus_array_get(argus, "ids", 1).as_int;

// Rechercher une valeur spécifique de mapping par clé
const char* user = argus_map_get(argus, "env", "USER").as_string;
int http_port = argus_map_get(argus, "ports", "http").as_int;
bool debug_enabled = argus_map_get(argus, "features", "debug").as_bool;
```

Ces fonctions d'aide gèrent les indices invalides ou les clés manquantes avec élégance, retournant une valeur vide (`{.raw = 0}`) lorsque l'élément demandé n'existe pas.

### API d'itérateur

Pour une itération plus idiomatique, argus fournit une API d'itérateur propre :

#### Itérateurs de tableau

```c
// Créer un itérateur de tableau
argus_array_it_t names_it = argus_array_it(argus, "names");

// Itérer à travers tous les éléments
while (argus_array_next(&names_it)) {
    printf("Nom : %s\n", names_it.value.as_string);
}
```

#### Itérateurs de mapping

```c
// Créer un itérateur de mapping
argus_map_it_t env_it = argus_map_it(argus, "env");

// Itérer à travers toutes les paires clé-valeur
while (argus_map_next(&env_it)) {
    printf("%s = %s\n", env_it.key, env_it.value.as_string);
}
```

#### Réinitialisation d'itérateur

Les itérateurs peuvent être réinitialisés et réutilisés pour plusieurs passages à travers la collection :

```c
argus_map_it_t features_it = argus_map_it(argus, "features");

// Premier passage : afficher les fonctionnalités activées
printf("Fonctionnalités activées : ");
while (argus_map_next(&features_it)) {
    if (features_it.value.as_bool) {
        printf("%s ", features_it.key);
    }
}
printf("\n");

// Réinitialiser l'itérateur pour un second passage
argus_map_reset(&features_it);

// Second passage : afficher les fonctionnalités désactivées
printf("Fonctionnalités désactivées : ");
while (argus_map_next(&features_it)) {
    if (!features_it.value.as_bool) {
        printf("%s ", features_it.key);
    }
}
printf("\n");
```

!!! tip "Filtrage avec itérateurs"
    Les itérateurs sont particulièrement utiles lorsque vous devez filtrer ou catégoriser des éléments en fonction de leurs valeurs, comme montré dans l'exemple ci-dessus.

## Considérations de performance

Lorsque vous travaillez avec de grandes collections, tenez compte de ces conseils de performance :

### 1. Choisir la bonne méthode d'accès

Différentes méthodes d'accès ont différentes caractéristiques de performance :

| Méthode d'accès | Meilleur pour | Caractéristiques de performance |
|---------------|----------|----------------------------|
| Accès direct | Accès aléatoire aux éléments | Accès O(1) mais nécessite de gérer les indices |
| Fonctions d'aide | Recherche d'éléments spécifiques | O(1) pour les tableaux, O(n) pour les mappings |
| Itérateurs | Traitement séquentiel de tous les éléments | Plus efficace pour un parcours complet |

### 2. Drapeaux de traitement de collection

Les drapeaux de collection ajoutent une surcharge de traitement :

- `FLAG_SORTED` : Nécessite un temps de tri O(n log n)
- `FLAG_UNIQUE` : Nécessite un temps de comparaison O(n²) pour une implémentation naïve, O(n log n) avec tri
- `FLAG_SORTED_KEY` / `FLAG_SORTED_VALUE` : Nécessite un temps de tri O(n log n)

Utilisez ces drapeaux uniquement lorsque les avantages l'emportent sur le coût de traitement.

### 3. Gestion de la mémoire

Les collections sont allouées dynamiquement avec une capacité initiale de 8 éléments, doublant de taille lorsque nécessaire :

```c
#define MULTI_VALUE_INITIAL_CAPACITY 8
```

Pour les collections extrêmement grandes, envisagez d'utiliser un gestionnaire personnalisé qui préalloue la capacité appropriée.

## Cas d'utilisation avancés

### Tags ou étiquettes en ligne de commande

```c
OPTION_ARRAY_STRING('t', "tag", "Tags pour la ressource",
                   FLAGS(FLAG_SORTED | FLAG_UNIQUE))
```

Utilisation :
```bash
./program --tag=important,urgent --tag=priority
```

### Substitutions de variables d'environnement

```c
OPTION_MAP_STRING('D', "define", "Définir des variables de configuration")
```

Utilisation :
```bash
./program -Ddebug=true -Dlog.level=info -Dserver.port=8080
```

### Gestion de bascules de fonctionnalités

Un exemple complet de gestion de fonctionnalités avec bascules :

```c
#include "argus.h"
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Drapeaux de fonctionnalités avec clés triées pour un affichage cohérent
    OPTION_MAP_BOOL('f', "feature", HELP("Drapeaux de fonctionnalités"),
                   FLAGS(FLAG_SORTED_KEY))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "feature_manager", "1.0.0");
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    if (argus_is_set(argus, "feature")) {
        // Créer des catégories pour activé/désactivé
        printf("Configuration des fonctionnalités :\n");
        
        printf("  Fonctionnalités activées :\n");
        argus_map_it_t it = argus_map_it(argus, "feature");
        while (argus_map_next(&it)) {
            if (it.value.as_bool) {
                printf("    - %s\n", it.key);
            }
        }
        
        printf("  Fonctionnalités désactivées :\n");
        argus_map_reset(&it);
        while (argus_map_next(&it)) {
            if (!it.value.as_bool) {
                printf("    - %s\n", it.key);
            }
        }
    } else {
        printf("Aucune fonctionnalité configurée.\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

## Détails d'implémentation technique

En coulisses, argus implémente les collections en utilisant des structures de données efficaces :

### Tableaux

Les tableaux sont implémentés comme des tableaux dynamiques d'éléments `argus_value_t` :

```c
// Stockage de tableau dans l'option
option->value.as_array = malloc(option->value_capacity * sizeof(argus_value_t));
```

Quand un tableau a besoin de croître :

```c
void adjust_array_size(argus_option_t *option)
{
    if (option->value.as_array == NULL) {
        option->value_capacity = MULTI_VALUE_INITIAL_CAPACITY;
        option->value.as_array = malloc(option->value_capacity * sizeof(argus_value_t));
    } else if (option->value_count >= option->value_capacity) {
        option->value_capacity *= 2;
        void *new = realloc(option->value.as_array, option->value_capacity * sizeof(argus_value_t));
        if (new == NULL) {
            option->value_capacity /= 2;
            return;
        }
        option->value.as_array = new;
    }
}
```

### Mappings

Les mappings sont implémentés comme des tableaux dynamiques d'éléments `argus_pair_t` :

```c
typedef struct argus_pair_s
{
    const char *key;
    argus_value_t     value;
} argus_pair_t;
```

La recherche de clé est effectuée par recherche linéaire :

```c
int map_find_key(argus_option_t *option, const char *key)
{
    for (size_t i = 0; i < option->value_count; ++i) {
        if (option->value.as_map[i].key && strcmp(option->value.as_map[i].key, key) == 0)
            return ((int)i);
    }
    return (-1);
}
```

### Implémentation des itérateurs

Les itérateurs sont des structures simples qui maintiennent une référence à la collection et une position courante :

```c
typedef struct argus_array_iterator_s
{
    argus_value_t *_array;    /* Pointeur vers le tableau */
    size_t   _count;    /* Nombre d'éléments */
    size_t   _position; /* Position courante */
    argus_value_t  value;     /* Valeur courante */
} argus_array_it_t;

typedef struct argus_map_iterator_s
{
    argus_pair_t *_map;      /* Pointeur vers le mapping */
    size_t        _count;    /* Nombre d'éléments */
    size_t        _position; /* Position courante */
    const char   *key;       /* Clé courante */
    argus_value_t       value;     /* Valeur courante */
} argus_map_it_t;
```

## Exemple complet

Voici un exemple complet démontrant des techniques avancées de gestion de collection :

```c
#include "argus.h"
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Options de tableau avec drapeaux
    OPTION_ARRAY_STRING('n', "name", HELP("Noms d'utilisateurs"),
                       FLAGS(FLAG_SORTED)),
    OPTION_ARRAY_INT('i', "id", HELP("IDs d'utilisateurs"),
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    
    // Options de mapping avec drapeaux
    OPTION_MAP_STRING('e', "env", HELP("Variables d'environnement"),
                     FLAGS(FLAG_SORTED_KEY)),
    OPTION_MAP_INT('p', "port", HELP("Mappages de ports"),
                  FLAGS(FLAG_SORTED_KEY)),
    OPTION_MAP_BOOL('f', "feature", HELP("Drapeaux de fonctionnalités"))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "multi_values", "1.0.0");
    argus.description = "Exemple avancé de collections multi-valeurs";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Traiter les tableaux en utilisant des itérateurs
    if (argus_is_set(argus, "name")) {
        printf("Utilisateurs :\n");
        argus_array_it_t it = argus_array_it(argus, "name");
        while (argus_array_next(&it)) {
            printf("  - %s\n", it.value.as_string);
        }
    }
    
    if (argus_is_set(argus, "id")) {
        printf("IDs d'utilisateurs :\n");
        argus_array_it_t it = argus_array_it(argus, "id");
        while (argus_array_next(&it)) {
            printf("  - %d\n", it.value.as_int);
        }
    }
    
    // Traiter les mappings en utilisant des itérateurs
    if (argus_is_set(argus, "env")) {
        printf("Variables d'environnement :\n");
        argus_map_it_t it = argus_map_it(argus, "env");
        while (argus_map_next(&it)) {
            printf("  %s = %s\n", it.key, it.value.as_string);
        }
    }
    
    if (argus_is_set(argus, "port")) {
        printf("Mappages de ports :\n");
        argus_map_it_t it = argus_map_it(argus, "port");
        while (argus_map_next(&it)) {
            printf("  %s: %d\n", it.key, it.value.as_int);
        }
    }
    
    // Traiter le mapping booléen avec catégories et filtrage
    if (argus_is_set(argus, "feature")) {
        printf("Fonctionnalités :\n");
        
        printf("  Activées :");
        argus_map_it_t it = argus_map_it(argus, "feature");
        while (argus_map_next(&it)) {
            if (it.value.as_bool) {
                printf(" %s", it.key);
            }
        }
        printf("\n");
        
        printf("  Désactivées :");
        argus_map_reset(&it);
        while (argus_map_next(&it)) {
            if (!it.value.as_bool) {
                printf(" %s", it.key);
            }
        }
        printf("\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

Cet exemple démontre :

1. Options de tableau et de mapping avec drapeaux de tri et d'unicité
2. Traitement basé sur des itérateurs pour toutes les collections
3. Filtrage et catégorisation en utilisant des itérateurs
4. Gestion appropriée des ressources

## Résumé

Les collections multi-valeurs de argus fournissent un moyen puissant de gérer des interfaces en ligne de commande complexes avec :

- **Valeurs multiples** pour une seule option (tableaux)
- **Configuration clé-valeur** à travers une seule option (mappings)
- **Formats d'entrée flexibles** pour la commodité de l'utilisateur
- **Tri et unicité** pour des données organisées
- **Itération efficace** à travers l'API d'itérateur

Ces fonctionnalités avancées permettent à argus de gérer des scénarios qui seraient difficiles ou impossibles avec les bibliothèques traditionnelles d'analyse d'arguments.
