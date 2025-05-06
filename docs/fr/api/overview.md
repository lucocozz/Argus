# Aperçu de l'API

!!! abstract "Aperçu"
    Cette page fournit une vue d'ensemble complète de l'architecture de l'API argus et de ses composants clés. argus est conçu pour être à la fois simple pour une utilisation basique et suffisamment puissant pour des interfaces en ligne de commande complexes.

## Architecture

argus est organisé autour de plusieurs composants clés :

```mermaid
graph TD
    A[Définition d'options] --> B[Analyse d'arguments]
    B --> C[Accès aux valeurs]
    B --> D[Validation]
    B --> E[Gestion des erreurs]
    F[Variables d'environnement] --> B
    G[Sous-commandes] --> B
```

1. **Définition d'options** - Macros pour définir les options et leurs propriétés
2. **Analyse d'arguments** - Fonctions pour traiter les arguments de ligne de commande
3. **Accès aux valeurs** - Fonctions pour récupérer les valeurs analysées
4. **Validation** - Mécanismes pour valider les entrées
5. **Gestion des erreurs** - Système pour gérer et signaler les erreurs
6. **Variables d'environnement** - Support pour la configuration via l'environnement
7. **Sous-commandes** - Support pour les hiérarchies de commandes

## Composants clés

### Structure principale

La structure `argus_t` est le cœur de argus, contenant toutes les données nécessaires :

```c
typedef struct argus_s {
    /* Champs publics */
    const char *program_name;    // Nom du programme
    const char *version;         // Version du programme
    const char *description;     // Description du programme
    const char *env_prefix;      // Préfixe pour les variables d'environnement
    
    /* Champs internes - ne pas accéder directement */
    argus_option_t     *options;      // Options définies
    argus_error_stack_t error_stack;  // Pile d'erreurs
    // Autres champs internes...
} argus_t;
```

!!! warning "Champs internes"
    Accédez uniquement aux champs publics directement. Les champs internes doivent être accédés via les fonctions API fournies.

### Définition d'options

Les options sont définies en utilisant la macro `ARGUS_OPTIONS` avec des macros spécifiques à chaque option :

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Numéro de port"), RANGE(1, 65535), DEFAULT(8080)),
    OPTION_FLAG('v', "verbose", HELP("Activer le mode verbeux")),
    POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
)
```

La structure `argus_option_t` représente une option individuelle avec toutes ses propriétés et son comportement.

### Types de valeurs

argus prend en charge divers types de valeurs pour les options :

| Catégorie | Types | Description |
|----------|-------|-------------|
| **Primitifs** | `VALUE_TYPE_STRING`<br>`VALUE_TYPE_INT`<br>`VALUE_TYPE_FLOAT`<br>`VALUE_TYPE_BOOL` | Types de valeurs de base |
| **Tableaux** | `VALUE_TYPE_ARRAY_STRING`<br>`VALUE_TYPE_ARRAY_INT`<br>`VALUE_TYPE_ARRAY_FLOAT` | Tableaux de valeurs multiples |
| **Maps** | `VALUE_TYPE_MAP_STRING`<br>`VALUE_TYPE_MAP_INT`<br>`VALUE_TYPE_MAP_FLOAT`<br>`VALUE_TYPE_MAP_BOOL` | Paires clé-valeur |
| **Personnalisés** | `VALUE_TYPE_CUSTOM` | Types définis par l'utilisateur |

Les valeurs sont stockées dans une union `argus_value_t` qui peut contenir différents types :

```c
union argus_value_u {
    uintptr_t raw;          // Valeur brute sous forme d'entier
    void     *as_ptr;       // Pointeur générique
    
    // Types de base
    char     *as_string;    // Chaîne de caractères
    int       as_int;       // Entier
    double    as_float;     // Flottant
    bool      as_bool;      // Booléen
    
    // Types de collection
    argus_value_t      *as_array;  // Tableau de valeurs
    argus_pair_t *as_map;    // Map de paires clé-valeur
};
```

### Types d'options

Les options sont catégorisées en plusieurs types :

| Type | Macro | Description | Exemple |
|------|-------|-------------|---------|
| **Options régulières** | `OPTION_*` | Options standard avec préfixe - ou -- | `OPTION_INT('p', "port", "Numéro de port")` |
| **Options drapeaux** | `OPTION_FLAG` | Options booléennes | `OPTION_FLAG('v', "verbose", "Mode verbeux")` |
| **Arguments positionnels** | `POSITIONAL_*` | Arguments sans préfixe tiret | `POSITIONAL_STRING("input", "Fichier d'entrée")` |
| **Sous-commandes** | `SUBCOMMAND` | Hiérarchies de commandes | `SUBCOMMAND("add", add_options, ...)` |
| **Groupes d'options** | `GROUP_START` ... `GROUP_END` | Regroupement visuel | `GROUP_START("Network", ...)` |
| **Options de collection** | `OPTION_ARRAY_*`<br>`OPTION_MAP_*` | Valeurs multiples | `OPTION_ARRAY_STRING('t', "tags", "Tags")` |

## Cycle de vie de l'API

L'utilisation typique de argus suit ce cycle de vie :

```mermaid
graph TD
    A[Définir les options] --> B[Initialiser argus]
    B --> C[Analyser les arguments]
    C --> D{Succès?}
    D -- Non --> E[Gérer l'erreur]
    D -- Oui --> F[Accéder aux valeurs]
    F --> G[Logique de l'application]
    G --> H[Libérer les ressources]
```

1. **Définir les options** avec `ARGUS_OPTIONS`
2. **Initialiser** avec `argus_init()`
3. **Analyser les arguments** avec `argus_parse()`
4. **Accéder aux valeurs** avec `argus_get()`, `argus_is_set()`, etc.
5. **Libérer les ressources** avec `argus_free()`

```c
// 1. Définir les options
ARGUS_OPTIONS(options, /* ... */)

int main(int argc, char **argv)
{
    // 2. Initialiser
    argus_t argus = argus_init(options, "my_program", "1.0.0");
    argus.description = "Description de mon programme";
    
    // 3. Analyser les arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // 4. Accéder aux valeurs
    const char *output = argus_get(argus, "output").as_string;
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Logique de l'application...
    
    // 5. Libérer les ressources
    argus_free(&argus);
    return 0;
}
```

## Familles de fonctions

L'API argus est organisée en plusieurs familles de fonctions :

### Fonctions d'initialisation et d'analyse

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `argus_init()` | Initialise le contexte argus | `argus_t argus = argus_init(options, "my_program", "1.0.0");` |
| `argus_parse()` | Analyse les arguments de ligne de commande | `int status = argus_parse(&argus, argc, argv);` |
| `argus_free()` | Libère les ressources | `argus_free(&argus);` |

### Fonctions d'accès aux valeurs

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `argus_get()` | Récupère la valeur d'une option | `int port = argus_get(argus, "port").as_int;` |
| `argus_is_set()` | Vérifie si une option a été définie | `if (argus_is_set(argus, "verbose")) { ... }` |
| `argus_count()` | Obtient le nombre de valeurs pour une option | `size_t count = argus_count(argus, "names");` |
| `argus_array_get()` | Récupère un élément d'un tableau | `const char* name = argus_array_get(argus, "names", 0).as_string;` |
| `argus_map_get()` | Récupère une valeur d'une map | `int port = argus_map_get(argus, "ports", "http").as_int;` |

### Fonctions d'itération

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `argus_array_it()` | Crée un itérateur de tableau | `argus_array_it_t it = argus_array_it(argus, "names");` |
| `argus_array_next()` | Avance au prochain élément du tableau | `while (argus_array_next(&it)) { ... }` |
| `argus_array_reset()` | Réinitialise un itérateur de tableau | `argus_array_reset(&it);` |
| `argus_map_it()` | Crée un itérateur de map | `argus_map_it_t it = argus_map_it(argus, "env");` |
| `argus_map_next()` | Avance à la prochaine entrée de map | `while (argus_map_next(&it)) { ... }` |
| `argus_map_reset()` | Réinitialise un itérateur de map | `argus_map_reset(&it);` |

### Fonctions de sous-commandes

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `argus_has_command()` | Vérifie si une sous-commande a été spécifiée | `if (argus_has_command(argus)) { ... }` |
| `argus_exec()` | Exécute l'action associée à une sous-commande | `status = argus_exec(&argus, data);` |

### Fonctions d'affichage

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `argus_print_help()` | Affiche un message d'aide formaté | `argus_print_help(argus);` |
| `argus_print_usage()` | Affiche un résumé d'utilisation | `argus_print_usage(argus);` |
| `argus_print_version()` | Affiche les informations de version | `argus_print_version(argus);` |

## Gestion des erreurs

argus utilise un système de codes d'erreur et une pile d'erreurs pour une gestion complète des erreurs :

```c
// Analyser les arguments avec gestion des erreurs
int status = argus_parse(&argus, argc, argv);
if (status != ARGUS_SUCCESS) {
    // Une erreur s'est produite pendant l'analyse
    argus_print_error_stack(&argus);  // Afficher les erreurs détaillées
    return status;
}
```

Codes d'erreur courants :

| Code d'erreur | Description |
|------------|-------------|
| `ARGUS_SUCCESS` | Opération réussie |
| `ARGUS_ERROR_INVALID_ARGUMENT` | Argument invalide |
| `ARGUS_ERROR_MISSING_VALUE` | Valeur requise mais non fournie |
| `ARGUS_ERROR_MISSING_REQUIRED` | Option requise non fournie |
| `ARGUS_ERROR_INVALID_FORMAT` | Format de valeur incorrect |
| `ARGUS_ERROR_INVALID_RANGE` | Valeur hors de la plage autorisée |
| `ARGUS_ERROR_INVALID_CHOICE` | Valeur n'est pas dans les choix autorisés |
| `ARGUS_ERROR_CONFLICTING_OPTIONS` | Options mutuellement exclusives spécifiées |

## Composants avancés

### Validateurs

Les validateurs vérifient que les valeurs d'entrée répondent à certains critères :

=== "Validateur de plage"
    ```c
    // S'assurer que le port est entre 1 et 65535
    OPTION_INT('p', "port", "Numéro de port", RANGE(1, 65535))
    ```

=== "Validateur d'expression régulière"
    ```c
    // S'assurer que l'email est valide
    OPTION_STRING('e', "email", "Adresse email", REGEX(ARGUS_RE_EMAIL))
    ```

=== "Validateur personnalisé"
    ```c
    // Logique de validation personnalisée
    OPTION_INT('n', "number", "Nombre pair", 
               VALIDATOR(even_validator, NULL))
    ```

### Gestionnaires

Les gestionnaires traitent les valeurs d'entrée et les convertissent en représentations internes :

```c
// Gestionnaire personnalisé pour les adresses IPv4
int ipv4_handler(argus_t *argus, argus_option_t *option, char *arg);

// Utilisation
OPTION_BASE('i', "ip", "Adresse IP", VALUE_TYPE_CUSTOM,
            HANDLER(ipv4_handler),
            FREE_HANDLER(ipv4_free_handler))
```

### Variables d'environnement

Les options peuvent être configurées pour lire depuis des variables d'environnement :

```c
// Définir des options avec support de variables d'environnement
ARGUS_OPTIONS(
    options,
    OPTION_STRING('H', "host", "Nom d'hôte", 
                  ENV_VAR("HOST")),  // Utilisera APP_HOST
    OPTION_INT('p', "port", "Numéro de port", 
               FLAGS(FLAG_AUTO_ENV))  // Utilisera APP_PORT
)

// Définir le préfixe d'environnement
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.env_prefix = "APP";
```

## Organisation des en-têtes

L'API publique de argus est répartie sur plusieurs fichiers d'en-tête :

| En-tête | Contenu | Inclure quand vous avez besoin |
|--------|---------|----------------------|
| `argus.h` | Point d'entrée principal, inclut tous les autres en-têtes | Toujours |
| `argus/types.h` | Définitions de types et structures de données | Rarement nécessaire directement |
| `argus/options.h` | Macros pour définir des options | Rarement nécessaire directement |
| `argus/api.h` | Fonctions de l'API publique | Rarement nécessaire directement |
| `argus/errors.h` | Codes d'erreur et fonctions de gestion des erreurs | Lors de la gestion d'erreurs spécifiques |
| `argus/regex.h` | Motifs d'expressions régulières prédéfinis | Lors de l'utilisation de la validation regex |

Dans la plupart des cas, vous n'aurez besoin d'inclure que l'en-tête principal :

```c
#include "argus.h"
```

## Considérations de performance

### Mode développement vs mode release

argus propose un mode release pour des performances optimales en production. Ce mode ignore la validation complète de la structure des options pendant l'initialisation.

```c
// En mode développement (par défaut) : validation complète
argus_t argus = argus_init(options, "mon_programme", "1.0.0");

// En mode release : initialisation plus rapide
// (activé en compilant avec -DARGUS_RELEASE)
argus_t argus = argus_init(options, "mon_programme", "1.0.0");
```

Pour activer le mode release, compilez avec le flag `-DARGUS_RELEASE`.

## Exemple de code

Voici un exemple complet démontrant les fonctionnalités clés de argus :

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Définir les options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Options régulières
    OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse")),
    OPTION_STRING('o', "output", HELP("Fichier journal"), 
                 DEFAULT("output.txt"), 
                 HINT("FICHIER")),
    OPTION_INT('p', "port", HELP("Numéro de port"), 
               DEFAULT(8080), 
               RANGE(1, 65535)),
    
    // Groupe d'options
    GROUP_START("Advanced", GROUP_DESC("Options avancées")),
        OPTION_FLAG('f', "force", HELP("Forcer l'opération")),
        OPTION_FLAG('r', "recursive", HELP("Mode récursif")),
    GROUP_END(),
    
    // Options de tableau
    OPTION_ARRAY_STRING('t', "tag", HELP("Tags"), FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Arguments positionnels
    POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
)

int main(int argc, char **argv)
{
    // Initialiser argus
    argus_t argus = argus_init(options, "example", "1.0.0");
    argus.description = "Programme exemple démontrant argus";
    
    // Analyser les arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Accéder aux valeurs
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    int port = argus_get(argus, "port").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;
    bool force = argus_get(argus, "force").as_bool;
    bool recursive = argus_get(argus, "recursive").as_bool;
    
    // Traiter les valeurs de tableau si définies
    if (argus_is_set(argus, "tag")) {
        size_t count = argus_count(argus, "tag");
        printf("Tags (%zu) :\n", count);
        
        argus_array_it_t it = argus_array_it(argus, "tag");
        while (argus_array_next(&it)) {
            printf("  - %s\n", it.value.as_string);
        }
    }
    
    // Logique de l'application...
    
    // Libérer les ressources
    argus_free(&argus);
    return 0;
}
```

## Catégories de fonctions

Pour faciliter la référence, voici un résumé des catégories de fonctions :

| Catégorie | Fonctions |
|----------|-----------|
| **Initialisation** | `argus_init`, `argus_parse`, `argus_free` |
| **Accès aux valeurs** | `argus_get`, `argus_is_set`, `argus_count` |
| **Fonctions de tableau** | `argus_array_get`, `argus_array_it`, `argus_array_next`, `argus_array_reset` |
| **Fonctions de map** | `argus_map_get`, `argus_map_it`, `argus_map_next`, `argus_map_reset` |
| **Fonctions de sous-commandes** | `argus_has_command`, `argus_exec` |
| **Fonctions d'affichage** | `argus_print_help`, `argus_print_usage`, `argus_print_version` |
| **Fonctions d'erreur** | `argus_print_error_stack`, `argus_strerror` |

## Documentation associée

- [Référence des types](types.md) - Informations détaillées sur les types de données
- [Référence des macros](macros.md) - Liste complète des macros de définition d'options
- [Référence des fonctions](functions.md) - Référence complète des fonctions de l'API
