# Référence des Macros

Cette référence fournit un guide complet de toutes les macros disponibles dans argus, organisées par fonction et objectif.

## Macros de Définition d'Options

Ces macros sont les éléments de base pour définir les options de ligne de commande et leur comportement.

### Définitions Fondamentales

| Macro | Objectif | Exemple |
|-------|----------|---------|
| `ARGUS_OPTIONS(name, ...)` | Définir un ensemble d'options de ligne de commande | `ARGUS_OPTIONS(options, HELP_OPTION(), ...)` |
| `OPTION_END()` | Terminer un tableau d'options | Généralement ajouté automatiquement |

### Options Standard

Ces macros définissent des options qui acceptent différents types de valeurs :

| Type d'Option | Macro | Description | Exemple |
|---------------|-------|-------------|---------|
| **Chaîne** | `OPTION_STRING(short, long, help, ...)` | Option avec valeur chaîne | `OPTION_STRING('o', "output", HELP("Fichier de sortie"))` |
| **Entier** | `OPTION_INT(short, long, help, ...)` | Option avec valeur entière | `OPTION_INT('p', "port", HELP("Port"))` |
| **Flottant** | `OPTION_FLOAT(short, long, help, ...)` | Option avec valeur flottante | `OPTION_FLOAT('s', "echelle", HELP("Échelle"))` |
| **Booléen** | `OPTION_BOOL(short, long, help, ...)` | Option avec valeur vrai/faux | `OPTION_BOOL('d', "debug", HELP("Mode debug"))` |
| **Drapeau** | `OPTION_FLAG(short, long, help, ...)` | Drapeau booléen (sans valeur) | `OPTION_FLAG('v', "verbose", HELP("Verbose"))` |

### Options de Tableau

Ces macros définissent des options qui peuvent accepter plusieurs valeurs :

| Type de Tableau | Macro | Description | Exemple |
|-----------------|-------|-------------|---------|
| **Tableau de Chaînes** | `OPTION_ARRAY_STRING(short, long, help, ...)` | Plusieurs valeurs chaîne | `OPTION_ARRAY_STRING('t', "tags", HELP("Tags"))` |
| **Tableau d'Entiers** | `OPTION_ARRAY_INT(short, long, help, ...)` | Plusieurs valeurs entières | `OPTION_ARRAY_INT('p', "ports", HELP("Ports"))` |
| **Tableau de Flottants** | `OPTION_ARRAY_FLOAT(short, long, help, ...)` | Plusieurs valeurs flottantes | `OPTION_ARRAY_FLOAT('f', "facteurs", HELP("Facteurs"))` |

### Options de Map

Ces macros définissent des options qui acceptent des paires clé-valeur :

| Type de Map | Macro | Description | Exemple |
|-------------|-------|-------------|---------|
| **Map de Chaînes** | `OPTION_MAP_STRING(short, long, help, ...)` | Clé-valeur avec valeurs chaîne | `OPTION_MAP_STRING('e', "env", HELP("Environnement"))` |
| **Map d'Entiers** | `OPTION_MAP_INT(short, long, help, ...)` | Clé-valeur avec valeurs entières | `OPTION_MAP_INT('p', "port", HELP("Mapping de ports"))` |
| **Map de Flottants** | `OPTION_MAP_FLOAT(short, long, help, ...)` | Clé-valeur avec valeurs flottantes | `OPTION_MAP_FLOAT('s', "echelle", HELP("Échelles"))` |
| **Map de Booléens** | `OPTION_MAP_BOOL(short, long, help, ...)` | Clé-valeur avec valeurs booléennes | `OPTION_MAP_BOOL('f', "fonction", HELP("Fonctionnalités"))` |

### Arguments Positionnels

Ces macros définissent des arguments positionnels (sans tirets) :

| Type Positionnel | Macro | Description | Exemple |
|------------------|-------|-------------|---------|
| **Chaîne** | `POSITIONAL_STRING(name, help, ...)` | Positionnel avec valeur chaîne | `POSITIONAL_STRING("input", HELP("Fichier d'entrée"))` |
| **Entier** | `POSITIONAL_INT(name, help, ...)` | Positionnel avec valeur entière | `POSITIONAL_INT("compte", HELP("Nombre"))` |
| **Flottant** | `POSITIONAL_FLOAT(name, help, ...)` | Positionnel avec valeur flottante | `POSITIONAL_FLOAT("facteur", HELP("Facteur"))` |
| **Booléen** | `POSITIONAL_BOOL(name, help, ...)` | Positionnel avec valeur booléenne | `POSITIONAL_BOOL("actif", HELP("Activer"))` |

### Options Communes

Options prédéfinies pour les fonctionnalités standard :

| Option | Macro | Description | Exemple |
|--------|-------|-------------|---------|
| **Aide** | `HELP_OPTION(...)` | Ajoute l'option `-h/--help` | `HELP_OPTION()` |
| **Version** | `VERSION_OPTION(...)` | Ajoute l'option `-V/--version` | `VERSION_OPTION()` |

### Options de Base

Macros avancées pour les types d'options personnalisés :

| Type de Base | Macro | Objectif | Exemple |
|--------------|-------|----------|---------|
| **Base d'Option** | `OPTION_BASE(short, long, type, ...)` | Type d'option personnalisé | `OPTION_BASE('i', "ip", VALUE_TYPE_CUSTOM, ...)` |
| **Base Positionnelle** | `POSITIONAL_BASE(name, type, ...)` | Type positionnel personnalisé | `POSITIONAL_BASE("coord", VALUE_TYPE_CUSTOM, ...)` |

## Macros de Validation

Ces macros ajoutent des contraintes de validation aux options :

| Validateur | Macro | Objectif | Exemple |
|------------|-------|----------|---------|
| **Plage** | `RANGE(min, max)` | Valide les valeurs numériques dans une plage | `OPTION_INT('p', "port", HELP("Port"), RANGE(1, 65535))` |
| **Longueur** | `LENGTH(min, max)` | Valide la longueur d'une chaîne dans une plage | `OPTION_STRING('u', "user", HELP("Nom d'utilisateur"), LENGTH(3, 20))` |
| **Nombre** | `COUNT(min, max)` | Valide la taille d'une collection dans une plage | `OPTION_ARRAY_INT('n', "nums", HELP("Nombres"), COUNT(1, 5))` |
| **Regex** | `REGEX(pattern)` | Valide un texte selon un modèle | `OPTION_STRING('e', "email", HELP("Email"), REGEX(ARGUS_RE_EMAIL))` |
| **Modèle Personnalisé** | `MAKE_REGEX(pattern, hint)` | Crée un modèle regex avec explication | `REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000"))` |
| **Validateur Personnalisé** | `VALIDATOR(function, data)` | Logique de validation personnalisée | `VALIDATOR(validateur_pair, NULL)` |
| **Pré-validateur** | `PRE_VALIDATOR(function, data)` | Valide la chaîne brute avant traitement | `PRE_VALIDATOR(validateur_longueur, &longueur_min)` |

### Validateurs de Choix

Ces macros valident par rapport à un ensemble de valeurs autorisées :

| Type de Choix | Macro | Objectif | Exemple |
|---------------|-------|----------|---------|
| **Choix de Chaînes** | `CHOICES_STRING(...)` | Autorise des valeurs de chaîne spécifiques | `CHOICES_STRING("debug", "info", "warning", "error")` |
| **Choix d'Entiers** | `CHOICES_INT(...)` | Autorise des valeurs entières spécifiques | `CHOICES_INT(1, 2, 3, 4)` |
| **Choix de Flottants** | `CHOICES_FLOAT(...)` | Autorise des valeurs flottantes spécifiques | `CHOICES_FLOAT(0.5, 1.0, 2.0)` |

## Macros de Propriété d'Option

Ces macros modifient les propriétés des options :

| Propriété | Macro | Objectif | Exemple |
|-----------|-------|----------|---------|
| **Valeur par Défaut** | `DEFAULT(value)` | Définit la valeur par défaut | `DEFAULT("output.txt")` |
| **Texte d'Aide** | `HELP(text)` | Définit la description d'aide | `HELP("Chemin du fichier de sortie")` |
| **Indice de Valeur** | `HINT(text)` | Définit l'indice pour l'affichage d'aide | `HINT("FICHIER")` |
| **Drapeaux** | `FLAGS(flags)` | Définit les drapeaux de comportement | `FLAGS(FLAG_REQUIRED)` |
| **Exigences** | `REQUIRES(...)` | Définit les options dépendantes | `REQUIRES("nom_utilisateur", "mot_de_passe")` |
| **Conflits** | `CONFLICTS(...)` | Définit les options incompatibles | `CONFLICTS("silencieux")` |
| **Variable d'Environnement** | `ENV_VAR(name)` | Définit la variable d'environnement | `ENV_VAR("SORTIE")` |

## Macros de Groupe et de Sous-commande

Ces macros organisent les options et définissent des hiérarchies de commandes :

| Macro | Objectif | Exemple |
|-------|----------|---------|
| `GROUP_START(name, ...)` | Commence un groupe d'options | `GROUP_START("Connexion", GROUP_DESC("Options de connexion"))` |
| `GROUP_END()` | Termine un groupe d'options | `GROUP_END()` |
| `GROUP_DESC(text)` | Définit la description du groupe | `GROUP_DESC("Paramètres avancés")` |
| `SUBCOMMAND(name, options, ...)` | Définit une sous-commande | `SUBCOMMAND("ajouter", options_ajout, HELP("Ajouter des fichiers"))` |
| `ACTION(function)` | Définit l'action de la sous-commande | `ACTION(commande_ajout)` |

## Macros de Gestionnaire

Ces macros définissent comment les options traitent les entrées :

| Macro | Objectif | Exemple |
|-------|----------|---------|
| `HANDLER(function)` | Logique de traitement personnalisée | `HANDLER(gestionnaire_ip)` |
| `FREE_HANDLER(function)` | Nettoyage des ressources | `FREE_HANDLER(gestionnaire_liberation_ip)` |

## Exemple Complet

Voici un exemple complet montrant diverses macros en utilisation :

```c
// Définir les options pour la sous-commande "ajouter"
ARGUS_OPTIONS(
    options_ajout,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Forcer l'opération d'ajout"), 
               CONFLICTS("simulation")),
    POSITIONAL_STRING("fichier", HELP("Fichier à ajouter"))
)

// Définir les options principales
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Options standard avec validation
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), 
                 DEFAULT("output.txt"), 
                 HINT("FICHIER"),
                 LENGTH(1, 100)),  // Valider la longueur de la chaîne
    
    OPTION_INT('p', "port", HELP("Numéro de port"), 
              RANGE(1, 65535),     // Valider la plage numérique
              DEFAULT(8080)),
    
    // Tableau avec validation de nombre
    OPTION_ARRAY_INT('n', "nombres", HELP("Liste de nombres"),
                    COUNT(1, 10),   // Valider la taille de la collection
                    FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Groupes d'options
    GROUP_START("Affichage", GROUP_DESC("Options d'affichage")),
        OPTION_FLAG('q', "silencieux", HELP("Supprimer la sortie"), 
                   CONFLICTS("verbose")),
        OPTION_FLAG('c', "couleur", HELP("Coloriser la sortie")),
    GROUP_END(),
    
    // Groupe exclusif
    GROUP_START("Format", GROUP_DESC("Format de sortie"), 
                FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('j', "json", HELP("Sortie JSON")),
        OPTION_FLAG('x', "xml", HELP("Sortie XML")),
        OPTION_FLAG('y', "yaml", HELP("Sortie YAML")),
    GROUP_END(),
    
    // Sous-commandes
    SUBCOMMAND("ajouter", options_ajout, 
               HELP("Ajouter des fichiers à l'index"), 
               ACTION(commande_ajout))
)
```

Pour des informations plus détaillées sur des macros spécifiques, consultez les sections pertinentes de ce guide.
