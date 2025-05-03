# argus

![CI/CD Pipeline](https://github.com/lucocozz/argus/actions/workflows/ci.yml/badge.svg)
![CodeQL Analysis](https://github.com/lucocozz/argus/actions/workflows/codeql.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

> Bibliothèque C moderne pour l'analyse des arguments de ligne de commande avec une API élégante basée sur des macros.

**argus** est une bibliothèque C puissante pour gérer les arguments de ligne de commande, conçue pour être à la fois simple à utiliser et suffisamment flexible pour les scénarios d'utilisation avancés.

## ✨ Fonctionnalités

- 📋 **Génération d'aide** : affichage automatique et formaté de l'aide et de l'utilisation
- 🔄 **Options typées** : booléens, entiers, chaînes de caractères, flottants, tableaux, mappages
- 🎨 **Analyse de format flexible** : prend en charge plusieurs formats d'options (--option=valeur, --option valeur, -ovaleur, etc.)
- 🌳 **Sous-commandes** : prise en charge des commandes imbriquées de style Git/Docker
- ⚠️ **Rapports d'erreurs clairs** : messages d'erreur détaillés et conviviaux pour les options ou valeurs invalides
- ✨ **Design élégant** : définition des options avec des macros concises et expressives
- 🔍 **Validateurs intégrés** : validateurs de plage, de choix et d'expressions régulières avec des ensembles prédéfinis complets
- 📦 **Regroupement organisé des options** : regroupement visuel des options associées dans l'affichage d'aide
- 🔗 **Relations entre options** : définition de dépendances et de conflits entre options
- 🚦 **Validation intelligente** : vérification complète de la structure des options pendant le développement pour prévenir les erreurs d'exécution, avec un mode release pour des performances optimales en production
- 🌐 **Variables d'environnement** : configuration ENV automatique
- 🚀 **Efficacité mémoire** : minimise les allocations sur le tas pour de meilleures performances et fiabilité
- 🧰 **Personnalisation facile des options** : créez vos propres types d'options et gestionnaires
- 🛡️ **Pipeline de validation personnalisé** : concevez vos propres validateurs avec un pré/post-traitement flexible

## Exemple rapide

```c
#include "argus.h"
#include <stdio.h>

// Définition des options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Activer la sortie verbeuse"),
    OPTION_STRING('o', "output", "Fichier de sortie", DEFAULT("output.txt")),
    OPTION_INT('p', "port", "Numéro de port", RANGE(1, 65535), DEFAULT(8080)),
    POSITIONAL_STRING("input", "Fichier d'entrée")
)

int main(int argc, char **argv)
{
    // Initialiser argus
    argus_t argus = argus_init(options, "my_program", "1.0.0");
    argus.description = "Démonstrateur de argus";

    // Analyser les arguments
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
        return 1;
    }

    // Accéder aux valeurs analysées
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    int port = argus_get(argus, "port").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;

    printf("Configuration :\n");
    printf("  Entrée : %s\n", input);
    printf("  Sortie : %s\n", output);
    printf("  Port : %d\n", port);
    printf("  Verbose : %s\n", verbose ? "oui" : "non");

    // Libérer les ressources
    argus_free(&argus);
    return 0;
}
```

## 🚀 Démarrage rapide

- [Installation](guide/installation.md) - Comment installer la bibliothèque argus
- [Démarrage rapide](guide/quickstart.md) - Créez votre première application avec argus
- [Exemples](examples/basic.md) - Explorez des exemples d'utilisation

## 📚 Documentation

La documentation est organisée comme suit :

- **[Guide de l'utilisateur](guide/installation.md)** - Instructions détaillées pour utiliser argus
- **[Référence API](api/overview.md)** - Référence complète de l'API argus
- **[Exemples](examples/basic.md)** - Exemples de code pratiques
- **[Fonctionnalités avancées](advanced/custom-handlers.md)** - Utilisation des fonctionnalités avancées
