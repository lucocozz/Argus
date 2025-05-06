# Sous-commandes

argus prend en charge les sous-commandes de style Git/Docker, vous permettant de créer des interfaces en ligne de commande avec des commandes distinctes.

!!! abstract "Aperçu"
    Les sous-commandes vous permettent d'organiser les fonctionnalités de votre programme en commandes séparées, chacune avec ses propres options et comportement. Par exemple :
    
    - `git commit`, `git push`, `git pull`
    - `docker run`, `docker build`, `docker pull`
    
    Ce guide couvre les bases de l'implémentation des sous-commandes dans argus. Pour une utilisation avancée, y compris les sous-commandes imbriquées, consultez le guide [Commandes imbriquées](../advanced/nested-commands.md).

## Concept de base

Les sous-commandes transforment votre programme d'un simple utilitaire en un outil polyvalent avec des commandes spécialisées :

```
my_program [options globales] commande [options de commande] [arguments]
```

Chaque commande peut avoir son propre ensemble d'options et de comportement tout en partageant des options globales communes.

## Implémentation des sous-commandes

Pour implémenter des sous-commandes dans argus, vous devez :

1. Définir les options pour chaque sous-commande
2. Définir des actions pour chaque sous-commande
3. Référencer les sous-commandes dans vos options principales

### 1. Définir les options des sous-commandes

D'abord, définissez les options pour chacune de vos sous-commandes :

```c
// Options pour la sous-commande "add"
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Forcer l'opération d'ajout")),
    POSITIONAL_STRING("file", HELP("Fichier à ajouter"))
)

// Options pour la sous-commande "remove"
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(),
    OPTION_FLAG('r', "recursive", HELP("Supprimer récursivement les répertoires")),
    POSITIONAL_STRING("file", HELP("Fichier à supprimer"))
)
```

### 2. Définir les fonctions d'action

Ensuite, définissez des fonctions d'action qui seront appelées lorsque chaque sous-commande sera exécutée :

```c
// Action pour la sous-commande "add"
int add_command(argus_t *argus, void *data)
{
    // Accéder aux options de la sous-commande
    const char* file = argus_get(*argus, "add.file").as_string;
    bool force = argus_get(*argus, "add.force").as_bool;
    
    printf("Ajout du fichier : %s\n", file);
    if (force) printf("  avec l'option force\n");
    
    return 0;
}

// Action pour la sous-commande "remove"
int remove_command(argus_t *argus, void *data)
{
    // Accéder aux options de la sous-commande
    const char* file = argus_get(*argus, "remove.file").as_string;
    bool recursive = argus_get(*argus, "remove.recursive").as_bool;
    
    printf("Suppression du fichier : %s\n", file);
    if (recursive) printf("  récursivement\n");
    
    return 0;
}
```

### 3. Définir les options principales avec les sous-commandes

Enfin, définissez vos options principales et incluez les sous-commandes :

```c
// Options principales avec sous-commandes
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Option globale applicable à toutes les sous-commandes
    OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse")),
    
    // Définir les sous-commandes
    SUBCOMMAND("add", add_options, 
               HELP("Ajouter des fichiers à l'index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Supprimer des fichiers de l'index"), 
               ACTION(remove_command))
)
```

## Traitement des sous-commandes

Dans votre fonction principale, vous devez vérifier si une sous-commande a été spécifiée et exécuter son action :

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "subcommands_example", "1.0.0");
    argus.description = "Exemple de sous-commandes";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }

    // Vérifier si une sous-commande a été spécifiée
    if (argus_has_command(argus)) {
        // Exécuter l'action de la sous-commande
        status = argus_exec(&argus, NULL);
    } else {
        printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
    }

    argus_free(&argus);
    return status;
}
```

## Accès aux options

### Accès aux options des sous-commandes

Dans une fonction d'action de sous-commande, vous pouvez accéder aux options de deux façons :

=== "Utilisation du chemin absolu"
    ```c
    // Accès avec chemin complet 
    const char* file = argus_get(*argus, "add.file").as_string;
    bool force = argus_get(*argus, "add.force").as_bool;
    ```

=== "Utilisation du chemin relatif"
    ```c
    // Dans la fonction add_command, vous pouvez utiliser des chemins relatifs
    const char* file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;
    ```

### Accès aux options globales

Les options globales sont accessibles depuis les actions des sous-commandes :

```c
// Accès aux options globales
bool verbose = argus_get(*argus, ".verbose").as_bool;
```

Le point initial (`.`) indique que l'option est définie au niveau racine.

## Intégration de l'aide

argus intègre automatiquement les sous-commandes dans l'affichage de l'aide :

```
subcommands_example v1.0.0

Exemple de sous-commandes

Usage: subcommands_example [OPTIONS] COMMAND

Options:
  -h, --help             - Afficher ce message d'aide (exit)
  -V, --version          - Afficher les informations de version (exit)
  -v, --verbose          - Activer la sortie verbeuse

Commandes:
  add                    - Ajouter des fichiers à l'index
  rm                     - Supprimer des fichiers de l'index

Exécutez 'subcommands_example COMMAND --help' pour plus d'informations sur une commande.
```

Chaque sous-commande a également sa propre aide :

```
subcommands_example v1.0.0

Usage: subcommands_example add [OPTIONS] <file>

Ajouter des fichiers à l'index

Arguments:
  <file>                 - Fichier à ajouter

Options:
  -h, --help             - Afficher ce message d'aide (exit)
  -f, --force            - Forcer l'opération d'ajout
```

## Exemple complet

Voici un exemple complet d'implémentation de sous-commandes de base :

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Gestionnaires d'action des sous-commandes
int add_command(argus_t *argus, void *data);
int remove_command(argus_t *argus, void *data);

// Définir les options pour la sous-commande "add"
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Forcer l'opération d'ajout")),
    POSITIONAL_STRING("file", HELP("Fichier à ajouter"))
)

// Définir les options pour la sous-commande "remove"
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(),
    OPTION_FLAG('r', "recursive", HELP("Supprimer récursivement les répertoires")),
    POSITIONAL_STRING("file", HELP("Fichier à supprimer"))
)

// Définir les options principales avec sous-commandes
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Option globale applicable à toutes les sous-commandes
    OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse")),
    
    // Définir les sous-commandes
    SUBCOMMAND("add", add_options, 
               HELP("Ajouter des fichiers à l'index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Supprimer des fichiers de l'index"), 
               ACTION(remove_command))
)

// Implémentation de la commande "add"
int add_command(argus_t *argus, void *data)
{
    (void)data; // Paramètre non utilisé
    
    // Obtenir l'option globale
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    
    // Obtenir les options spécifiques à la commande
    const char* file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;

    printf("Ajout du fichier : %s\n", file);
    if (verbose) printf("  mode verbeux activé\n");
    if (force) printf("  avec l'option force\n");

    return 0;
}

// Implémentation de la commande "remove"
int remove_command(argus_t *argus, void *data)
{
    (void)data; // Paramètre non utilisé
    
    // Obtenir l'option globale
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    
    // Obtenir les options spécifiques à la commande
    const char* file = argus_get(*argus, "file").as_string;
    bool recursive = argus_get(*argus, "recursive").as_bool;

    printf("Suppression du fichier : %s\n", file);
    if (verbose) printf("  mode verbeux activé\n");
    if (recursive) printf("  récursivement\n");

    return 0;
}

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "subcommands_example", "1.0.0");
    argus.description = "Exemple de sous-commandes";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }

    if (argus_has_command(argus)) {
        // Exécuter le gestionnaire de sous-commande
        status = argus_exec(&argus, NULL);
    } else {
        printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
    }

    argus_free(&argus);
    return 0;
}
```

## Prochaines étapes

Pour des fonctionnalités de sous-commandes plus avancées, telles que :

- Sous-commandes imbriquées (commandes au sein de commandes)
- Abréviations de noms de commandes
- Placements alternatifs d'arguments positionnels
- Gestion de commande personnalisée

Consultez le guide [Commandes imbriquées](../advanced/nested-commands.md).
