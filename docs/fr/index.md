# Bienvenue sur argus

<div align="center">
  <img src="assets/argus-logo.webp" alt="argus" width="180px">
  <br><br>
  <em>Parser d'arguments en ligne de commande moderne pour C</em>
  <br><br>
</div>

## Explorer argus

<div class="grid cards" markdown>

-   :material-rocket-launch:{ .lg .middle } **Commencer avec argus**

    ---

    Démarrez avec argus en quelques minutes :

    [:octicons-arrow-right-24: Installation](guide/installation.md)
    [:octicons-arrow-right-24: Démarrage rapide](guide/quickstart.md)
    [:octicons-arrow-right-24: Exemples](examples/basic.md)

-   :material-book-open-variant:{ .lg .middle } **Concepts fondamentaux**

    ---

    Maîtrisez les concepts de base :

    [:octicons-arrow-right-24: Options de base](guide/basic-options.md)
    [:octicons-arrow-right-24: Formats d'options](guide/option-formats.md)
    [:octicons-arrow-right-24: Validation](guide/validation.md)

-   :material-tools:{ .lg .middle } **Fonctionnalités avancées**

    ---

    Exploitez toute la puissance d'argus :

    [:octicons-arrow-right-24: Sous-commandes](guide/subcommands.md)
    [:octicons-arrow-right-24: Multi-valeurs](guide/multi-values.md)
    [:octicons-arrow-right-24: Variables d'environnement](guide/environment.md)

-   :material-api:{ .lg .middle } **Référence API**

    ---

    Documentation complète pour les développeurs :

    [:octicons-arrow-right-24: Fonctions](api/functions.md)
    [:octicons-arrow-right-24: Macros](api/macros.md)
    [:octicons-arrow-right-24: Types](api/types.md)

</div>

## Caractéristiques en bref

argus offre une combinaison unique de puissance et de simplicité pour l'analyse d'arguments en ligne de commande en C :

- **API élégante** - Macros concises et expressives pour définir les options
- **Sécurité des types** - Support intégré pour les chaînes, entiers, flottants, booléens
- **Collections multi-valeurs** - Tableaux et mappages pour les données complexes
- **Sous-commandes imbriquées** - Hiérarchies de commandes de style Git/Docker
- **Validation des entrées** - Validateurs de plage, regex et personnalisés
- **Gestion des erreurs** - Messages d'erreur clairs et informatifs
- **Variables d'environnement** - Chargement automatique des options depuis l'environnement
- **Génération d'aide** - Affichage d'aide formaté et élégant

## Exemple rapide

```c
#include "argus.h"
#include <stdio.h>

// Définition des options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Activer le mode verbeux")),
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Numéro de port"), RANGE(1, 65535), DEFAULT(8080)),
    POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
)

int main(int argc, char **argv)
{
    // Initialisation d'argus
    argus_t argus = argus_init(options, "mon_programme", "1.0.0");
    argus.description = "Démonstrateur d'argus";

    // Analyse des arguments
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
        return 1;
    }

    // Accès aux valeurs analysées
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    int port = argus_get(argus, "port").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;

    printf("Configuration :\n");
    printf("  Entrée : %s\n", input);
    printf("  Sortie : %s\n", output);
    printf("  Port : %d\n", port);
    printf("  Verbeux : %s\n", verbose ? "oui" : "non");

    // Libération des ressources
    argus_free(&argus);
    return 0;
}
```

## Affichage d'aide automatique

Lorsque les utilisateurs invoquent votre programme avec `--help`, argus génère automatiquement un message d'aide formaté comme celui-ci :

```
mon_programme v1.0.0

Démonstrateur d'argus

Usage: mon_programme [OPTIONS] <input>

Arguments:
  <input>                - Fichier d'entrée

Options:
  -h, --help             - Afficher ce message d'aide (quitter)
  -V, --version          - Afficher les informations de version (quitter)
  -v, --verbose          - Activer le mode verbeux
  -o, --output <STR>     - Fichier de sortie (défaut : "output.txt")
  -p, --port <NUM>       - Numéro de port [1-65535] (défaut : 8080)
```

## Obtenir de l'aide

- **[:material-help-circle: Guide utilisateur](guide/installation.md)** - Apprenez à utiliser argus étape par étape
- **[:material-code-tags: Référence API](api/overview.md)** - Explorez la documentation complète de l'API
- **[:material-github: GitHub](https://github.com/lucocozz/argus)** - Signalez des problèmes ou contribuez au développement

## Communauté

Nous accueillons les contributions et les retours de la communauté :

- **[:octicons-git-pull-request-24: Contribuer](contributing.md)** - Aidez à améliorer argus
- **[:octicons-law-24: Licence](license.md)** - Licence MIT
