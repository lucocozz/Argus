# Référence des fonctions

Cette page fournit une référence complète pour toutes les fonctions de l'API argus. Ces fonctions vous permettent d'initialiser la bibliothèque, d'analyser les arguments, d'accéder aux valeurs et de gérer les ressources.

!!! abstract "Aperçu"
    L'API argus est organisée en plusieurs groupes de fonctions :
    
    - **Initialisation et analyse** - Fonctions de base pour la configuration et l'analyse
    - **Accès aux valeurs** - Fonctions pour récupérer les valeurs des options
    - **Accès aux collections** - Fonctions pour travailler avec les tableaux et les mappages
    - **Gestion des sous-commandes** - Fonctions pour gérer les sous-commandes
    - **Fonctions d'affichage** - Fonctions pour générer le texte d'aide et d'utilisation
    - **Gestion des erreurs** - Fonctions pour gérer et signaler les erreurs

## Initialisation et analyse

### argus_init

Initialise un contexte argus avec les informations du programme et les options.

```c
argus_t argus_init(argus_option_t *options, const char *program_name, const char *version);
```

**Paramètres :**
- `options` : Tableau d'options de ligne de commande définies avec `ARGUS_OPTIONS`
- `program_name` : Nom du programme (utilisé dans les messages d'aide/d'erreur)
- `version` : Chaîne de version

**Retourne :**
- Une structure `argus_t` initialisée

**Exemple :**
```c
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.description = "Description de mon programme génial";
```

!!! tip
    Après l'initialisation, vous pouvez définir des champs supplémentaires comme `description` et `env_prefix` avant l'analyse.

### argus_parse

Analyse les arguments de la ligne de commande selon les options définies.

```c
int argus_parse(argus_t *argus, int argc, char **argv);
```

**Paramètres :**
- `argus` : Pointeur vers un contexte argus initialisé
- `argc` : Nombre d'arguments (de `main`)
- `argv` : Valeurs des arguments (de `main`)

**Retourne :**
- `ARGUS_SUCCESS` (0) en cas de succès
- Un code d'erreur non nul en cas d'échec

**Exemple :**
```c
int status = argus_parse(&argus, argc, argv);
if (status != ARGUS_SUCCESS) {
    return status;
}
```

### argus_free

Libère les ressources allouées pendant l'analyse.

```c
void argus_free(argus_t *argus);
```

**Paramètres :**
- `argus` : Pointeur vers le contexte argus à libérer

**Exemple :**
```c
argus_free(&argus);
```

!!! warning
    Appelez toujours `argus_free()` lorsque vous avez terminé avec un contexte argus pour éviter les fuites de mémoire.

## Accès aux valeurs

### argus_get

Récupère la valeur d'une option.

```c
argus_value_t argus_get(argus_t argus, const char *option_path);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option (format nom ou `sous-commande.nom`)

**Retourne :**
- La valeur de l'option sous forme d'union `argus_value_t`, ou `{.raw = 0}` si non trouvée

**Exemple :**
```c
const char *output = argus_get(argus, "output").as_string;
int port = argus_get(argus, "port").as_int;
bool verbose = argus_get(argus, "verbose").as_bool;
```

### argus_is_set

Vérifie si une option a été explicitement définie sur la ligne de commande.

```c
bool argus_is_set(argus_t argus, const char *option_path);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option (format nom ou `sous-commande.nom`)

**Retourne :**
- `true` si l'option a été définie, `false` sinon

**Exemple :**
```c
if (argus_is_set(argus, "verbose")) {
    // Mode verbose activé
}
```

### argus_count

Obtient le nombre de valeurs pour une option (pour les collections).

```c
size_t argus_count(argus_t argus, const char *option_path);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option (format nom ou `sous-commande.nom`)

**Retourne :**
- Nombre de valeurs pour l'option, ou 0 si non trouvée ou pas une collection

**Exemple :**
```c
size_t tags_count = argus_count(argus, "tags");
printf("Tags : %zu\n", tags_count);
```

## Accès aux collections

### argus_array_get

Récupère un élément d'une option de tableau à l'indice spécifié.

```c
argus_value_t argus_array_get(argus_t argus, const char *option_path, size_t index);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option de tableau
- `index` : Indice de l'élément à récupérer

**Retourne :**
- La valeur à l'indice spécifié, ou `{.raw = 0}` si non trouvée ou indice hors limites

**Exemple :**
```c
const char *first_tag = argus_array_get(argus, "tags", 0).as_string;
```

### argus_map_get

Récupère une valeur d'une option de mapping avec la clé spécifiée.

```c
argus_value_t argus_map_get(argus_t argus, const char *option_path, const char *key);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option de mapping
- `key` : Clé à rechercher dans le mapping

**Retourne :**
- La valeur associée à la clé, ou `{.raw = 0}` si non trouvée

**Exemple :**
```c
const char *user = argus_map_get(argus, "env", "USER").as_string;
int http_port = argus_map_get(argus, "ports", "http").as_int;
```

### argus_array_it

Crée un itérateur pour parcourir efficacement une option de tableau.

```c
argus_array_it_t argus_array_it(argus_t argus, const char *option_path);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option de tableau

**Retourne :**
- Structure d'itérateur pour le tableau, avec `count=0` si l'option n'est pas trouvée

**Exemple :**
```c
argus_array_it_t it = argus_array_it(argus, "tags");
```

### argus_array_next

Obtient l'élément suivant d'un itérateur de tableau.

```c
bool argus_array_next(argus_array_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de tableau

**Retourne :**
- `true` si une valeur a été récupérée, `false` si fin du tableau

**Exemple :**
```c
argus_array_it_t it = argus_array_it(argus, "tags");
while (argus_array_next(&it)) {
    printf("Tag : %s\n", it.value.as_string);
}
```

### argus_array_reset

Réinitialise un itérateur de tableau au début.

```c
void argus_array_reset(argus_array_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de tableau à réinitialiser

**Exemple :**
```c
argus_array_reset(&it);  // Réinitialiser pour commencer une nouvelle itération
```

### argus_map_it

Crée un itérateur pour parcourir efficacement une option de mapping.

```c
argus_map_it_t argus_map_it(argus_t argus, const char *option_path);
```

**Paramètres :**
- `argus` : Le contexte argus
- `option_path` : Chemin vers l'option de mapping

**Retourne :**
- Structure d'itérateur pour le mapping, avec `count=0` si l'option n'est pas trouvée

**Exemple :**
```c
argus_map_it_t it = argus_map_it(argus, "env");
```

### argus_map_next

Obtient la paire clé-valeur suivante d'un itérateur de mapping.

```c
bool argus_map_next(argus_map_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de mapping

**Retourne :**
- `true` si une paire a été récupérée, `false` si fin du mapping

**Exemple :**
```c
argus_map_it_t it = argus_map_it(argus, "env");
while (argus_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

### argus_map_reset

Réinitialise un itérateur de mapping au début.

```c
void argus_map_reset(argus_map_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de mapping à réinitialiser

**Exemple :**
```c
argus_map_reset(&it);  // Réinitialiser pour commencer une nouvelle itération
```

## Gestion des sous-commandes

### argus_has_command

Vérifie si une sous-commande a été spécifiée sur la ligne de commande.

```c
bool argus_has_command(argus_t argus);
```

**Paramètres :**
- `argus` : Le contexte argus

**Retourne :**
- `true` si une sous-commande a été spécifiée, `false` sinon

**Exemple :**
```c
if (argus_has_command(argus)) {
    // Une sous-commande a été spécifiée
} else {
    printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
}
```

### argus_exec

Exécute l'action associée à la sous-commande spécifiée.

```c
int argus_exec(argus_t *argus, void *data);
```

**Paramètres :**
- `argus` : Pointeur vers le contexte argus
- `data` : Données optionnelles à passer à l'action de la sous-commande

**Retourne :**
- Code de statut retourné par l'action de la sous-commande
- `ARGUS_ERROR_NO_COMMAND` si aucune commande n'a été spécifiée
- `ARGUS_ERROR_INVALID_HANDLER` si la commande n'a pas d'action

**Exemple :**
```c
if (argus_has_command(argus)) {
    status = argus_exec(&argus, NULL);
}
```

## Fonctions d'affichage

### argus_print_help

Affiche un message d'aide formaté basé sur les options définies.

```c
void argus_print_help(argus_t argus);
```

**Paramètres :**
- `argus` : Le contexte argus

**Exemple :**
```c
argus_print_help(argus);
```

### argus_print_usage

Affiche un résumé court de l'utilisation.

```c
void argus_print_usage(argus_t argus);
```

**Paramètres :**
- `argus` : Le contexte argus

**Exemple :**
```c
argus_print_usage(argus);
```

### argus_print_version

Affiche les informations de version.

```c
void argus_print_version(argus_t argus);
```

**Paramètres :**
- `argus` : Le contexte argus

**Exemple :**
```c
argus_print_version(argus);
```

## Gestion des erreurs

### argus_print_error_stack

Affiche toutes les erreurs dans la pile d'erreurs.

```c
void argus_print_error_stack(const argus_t *argus);
```

**Paramètres :**
- `argus` : Le contexte argus

**Exemple :**
```c
if (status != ARGUS_SUCCESS) {
    argus_print_error_stack(&argus);
    return status;
}
```

### argus_strerror

Obtient une description textuelle d'un code d'erreur.

```c
const char *argus_strerror(argus_error_type_t error);
```

**Paramètres :**
- `error` : Code d'erreur

**Retourne :**
- Description textuelle de l'erreur

**Exemple :**
```c
printf("Erreur : %s\n", argus_strerror(status));
```

## Exemple complet

Voici un exemple complet montrant le schéma d'utilisation de la fonction principale :

```c
#include "argus.h"
#include <stdio.h>

// Définir les options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse")),
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), DEFAULT("output.txt")),
    POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
)

int main(int argc, char **argv)
{
    // Initialiser argus
    argus_t argus = argus_init(options, "example", "1.0.0");
    argus.description = "Programme exemple utilisant argus";
    
    // Analyser les arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        argus_print_error_stack(&argus);
        return status;
    }
    
    // Accéder aux valeurs
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Logique de l'application
    if (verbose) {
        printf("Entrée : %s\n", input);
        printf("Sortie : %s\n", output);
    }
    
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
| **Fonctions de mapping** | `argus_map_get`, `argus_map_it`, `argus_map_next`, `argus_map_reset` |
| **Fonctions de sous-commande** | `argus_has_command`, `argus_exec` |
| **Fonctions d'affichage** | `argus_print_help`, `argus_print_usage`, `argus_print_version` |
| **Fonctions d'erreur** | `argus_print_error_stack`, `argus_strerror` |

## Documentation associée

- [Référence des types](types.md) - Informations détaillées sur les types de données
- [Référence des macros](macros.md) - Liste complète des macros de définition d'options
- [Aperçu de l'API](overview.md) - Aperçu de haut niveau de l'API argus
