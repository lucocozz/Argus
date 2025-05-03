# Référence des types

Cette page documente les types de données et structures de base fournis par la bibliothèque argus. La compréhension de ces types est essentielle pour une utilisation avancée et des extensions personnalisées.

!!! abstract "Aperçu"
    argus définit plusieurs types de données et structures clés qui forment la base de la bibliothèque :
    
    - **Structures de base** - Structures de données principales comme `argus_t` et `argus_option_t`
    - **Types de valeurs** - Types pour stocker et accéder à différentes sortes de valeurs
    - **Énumérations** - Classificateurs de types et définitions de drapeaux
    - **Collections** - Structures de données de tableaux et de maps, et itérateurs
    - **Types de callbacks** - Types de pointeurs de fonction pour les gestionnaires et validateurs

## Structures de base

### argus_t

La structure de contexte principale qui contient tous les états d'analyse et la configuration.

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
    struct { /* ... */ } context;     // Contexte interne
} argus_t;
```

Les champs publics peuvent être accédés et modifiés directement après l'initialisation avec `argus_init()` :

```c
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.description = "Mon super programme";
argus.env_prefix = "MYAPP";  // Optionnel : préfixe pour les variables d'environnement
```

!!! warning "Champs internes"
    Les champs internes ne doivent pas être accédés directement. Utilisez les fonctions API fournies pour interagir avec eux.

### argus_option_t

Définit une option de ligne de commande avec toutes ses propriétés et son comportement.

```c
typedef struct argus_option_s {
    /* Métadonnées de base */
    argus_optype_t type;        // Type d'option (drapeau, positionnel, etc.)
    const char *name;          // Nom interne pour les références
    char sname;                // Nom court (ex., 'v' pour -v)
    const char *lname;         // Nom long (ex., "verbose" pour --verbose)
    const char *help;          // Texte d'aide
    const char *hint;          // Indice de valeur pour l'affichage d'aide
    
    /* Métadonnées de valeur */
    argus_valtype_t value_type;   // Type de valeur
    argus_value_t value;             // Valeur actuelle
    argus_value_t default_value;     // Valeur par défaut
    bool have_default;         // Si une valeur par défaut est définie
    /* Champs de valeur supplémentaires... */
    
    /* Callbacks */
    argus_handler_t handler;       // Fonction gestionnaire de valeur
    argus_free_handler_t free_handler;  // Fonction de nettoyage des ressources
    argus_validator_t validator;    // Validateur de valeur
    /* Champs de callback supplémentaires... */
    
    /* Dépendances */
    const char **requires;     // Options qui doivent être définies avec celle-ci
    const char **conflicts;    // Options qui ne peuvent pas être utilisées avec celle-ci
    
    /* État */
    argus_optflags_t flags;      // Drapeaux de comportement d'option
    bool is_set;               // Si l'option a été définie en ligne de commande
    
    /* Champs de sous-commande */
    argus_action_t action;     // Action pour les sous-commandes
    struct argus_option_s *sub_options;  // Options pour les sous-commandes
} argus_option_t;
```

!!! note
    Vous ne créez généralement pas de structures `argus_option_t` directement. Utilisez plutôt les macros fournies comme `OPTION_STRING`, `OPTION_INT`, etc.

## Types de valeurs

### argus_value_t

Un type union qui peut contenir des valeurs de différents types :

```c
typedef union argus_value_u {
    uintptr_t raw;          // Valeur brute en entier
    void     *as_ptr;       // Pointeur générique
    
    // Types de base
    char  as_char;          // Caractère
    char *as_string;        // Chaîne
    int   as_int;           // Entier
    long long as_int64;     // Entier 64 bits
    double as_float;        // Flottant
    bool   as_bool;         // Booléen
    
    // Types de collection
    argus_value_t      *as_array;  // Tableau de valeurs
    argus_pair_t *as_map;    // Map clé-valeur
} argus_value_t;
```

### argus_pair_t

Une paire clé-valeur utilisée dans les collections de type map :

```c
typedef struct argus_pair_s {
    const char *key;    // Clé chaîne
    argus_value_t     value;  // Valeur de n'importe quel type supporté
} argus_pair_t;
```

## Énumérations

### argus_optype_t

Définit les différents types d'éléments de ligne de commande :

```c
typedef enum argus_optype_e {
    TYPE_NONE = 0,        // Terminateur pour les tableaux d'options
    TYPE_OPTION,          // Option standard avec préfixe - ou --
    TYPE_GROUP,           // Groupement logique d'options
    TYPE_POSITIONAL,      // Argument positionnel
    TYPE_SUBCOMMAND,      // Sous-commande avec ses propres options
} argus_optype_t;
```

### argus_valtype_t

Définit les types de valeurs qu'une option peut contenir :

```c
typedef enum argus_valtype_e {
    VALUE_TYPE_NONE = 0,        // Pas de valeur
    
    // Types primitifs
    VALUE_TYPE_INT    = 1 << 0,  // Entier
    VALUE_TYPE_STRING = 1 << 1,  // Chaîne
    VALUE_TYPE_FLOAT  = 1 << 2,  // Flottant
    VALUE_TYPE_BOOL   = 1 << 3,  // Booléen
    VALUE_TYPE_FLAG   = 1 << 4,  // Drapeau (booléen sans valeur)
    
    // Types de tableau
    VALUE_TYPE_ARRAY_STRING = 1 << 5,  // Tableau de chaînes
    VALUE_TYPE_ARRAY_INT    = 1 << 6,  // Tableau d'entiers
    VALUE_TYPE_ARRAY_FLOAT  = 1 << 7,  // Tableau de flottants
    
    // Types de map
    VALUE_TYPE_MAP_STRING = 1 << 8,    // Map de chaînes
    VALUE_TYPE_MAP_INT    = 1 << 9,    // Map d'entiers
    VALUE_TYPE_MAP_FLOAT  = 1 << 10,    // Map de flottants
    VALUE_TYPE_MAP_BOOL   = 1 << 11,   // Map de booléens
    
    VALUE_TYPE_CUSTOM = 1 << 12,       // Type personnalisé
} argus_valtype_t;
```

### argus_optflags_t

Définit des drapeaux qui modifient le comportement des options :

```c
typedef enum argus_optflags_e {
    FLAG_NONE = 0,
    /* Drapeaux d'option */
    FLAG_REQUIRED      = 1 << 0,  // L'option doit être spécifiée
    FLAG_HIDDEN        = 1 << 1,  // L'option est masquée dans l'aide
    FLAG_ADVANCED      = 1 << 2,  // L'option est marquée comme avancée
    FLAG_DEPRECATED    = 1 << 3,  // L'option est marquée comme obsolète
    FLAG_EXPERIMENTAL  = 1 << 4,  // L'option est marquée comme expérimentale
    FLAG_EXIT          = 1 << 5,  // Le programme se termine après le traitement de l'option
    FLAG_ENV_OVERRIDE  = 1 << 6,  // La valeur de l'option peut être remplacée par une variable d'environnement
    FLAG_AUTO_ENV      = 1 << 7,  // Générer automatiquement le nom de variable d'environnement
    FLAG_NO_ENV_PREFIX = 1 << 8,  // Ne pas utiliser le préfixe de variable d'environnement
    
    /* Drapeaux de type tableau et map */
    FLAG_SORTED       = 1 << 9,   // Les valeurs du tableau sont triées
    FLAG_UNIQUE       = 1 << 10,  // Les valeurs du tableau sont uniques
    FLAG_SORTED_VALUE = 1 << 11,  // Les valeurs de la map sont triées
    FLAG_SORTED_KEY   = 1 << 12,  // Les clés de la map sont triées
    FLAG_UNIQUE_VALUE = 1 << 13,  // Les valeurs de la map sont uniques
    
    /* Drapeaux de groupe */
    FLAG_EXCLUSIVE = 1 << 14,     // Une seule option du groupe peut être définie
} argus_optflags_t;
```

## Itérateurs de collections

### argus_array_it_t

Itérateur pour les collections de type tableau :

```c
typedef struct argus_array_iterator_s {
    argus_value_t *_array;      // Pointeur interne du tableau
    size_t   _count;      // Nombre d'éléments
    size_t   _position;   // Position actuelle
    argus_value_t  value;       // Valeur actuelle
} argus_array_it_t;
```

Exemple d'utilisation :
```c
argus_array_it_t it = argus_array_it(argus, "names");
while (argus_array_next(&it)) {
    printf("Nom : %s\n", it.value.as_string);
}
```

### argus_map_it_t

Itérateur pour les collections de type map :

```c
typedef struct argus_map_iterator_s {
    argus_pair_t *_map;    // Pointeur interne de la map
    size_t        _count;  // Nombre d'éléments
    size_t        _position; // Position actuelle
    const char   *key;     // Clé actuelle
    argus_value_t       value;   // Valeur actuelle
} argus_map_it_t;
```

Exemple d'utilisation :
```c
argus_map_it_t it = argus_map_it(argus, "env");
while (argus_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

## Types de callbacks

### argus_handler_t

Fonction gestionnaire pour traiter les valeurs d'options :

```c
typedef int (*argus_handler_t)(argus_t *, argus_option_t *, char *);
```

Les gestionnaires personnalisés traitent la valeur de chaîne brute et la stockent dans la structure d'option.

### argus_free_handler_t

Fonction gestionnaire pour libérer les ressources des options :

```c
typedef int (*argus_free_handler_t)(argus_option_t *);
```

Les gestionnaires de libération personnalisés nettoient les ressources allouées par une fonction gestionnaire.

### argus_validator_t

Fonction validateur pour vérifier les valeurs d'options :

```c
typedef int (*argus_validator_t)(argus_t *, argus_value_t, validator_data_t);
```

Les validateurs s'assurent que les valeurs répondent à certains critères après traitement.

### argus_pre_validator_t

Fonction pré-validateur pour vérifier les valeurs de chaînes brutes :

```c
typedef int (*argus_pre_validator_t)(argus_t *, const char *, validator_data_t);
```

Les pré-validateurs vérifient les chaînes d'entrée avant qu'elles ne soient traitées.

### argus_action_t

Fonction d'action pour les sous-commandes :

```c
typedef int (*argus_action_t)(argus_t *, void *);
```

Les actions sont exécutées lorsqu'une sous-commande est invoquée.

## Types de données de validateur

### validator_data_t

Données passées aux validateurs :

```c
typedef union validator_data_u {
    void        *custom;  // Données de validateur personnalisé
    range_t      range;   // Limites de plage pour la validation numérique
    regex_data_t regex;   // Motif regex et informations
} validator_data_t;
```

### range_t

Limites de plage pour la validation numérique :

```c
typedef struct range_s {
    long long min;  // Valeur minimum
    long long max;  // Valeur maximum
} range_t;
```

### regex_data_t

Données de validation par expression régulière :

```c
typedef struct regex_data_s {
    const char *pattern;  // Chaîne du motif regex
    const char *hint;     // Indice de message d'erreur
} regex_data_t;
```

## Types de gestion d'erreurs

### argus_error_type_t

Codes d'erreur retournés par les fonctions argus :

```c
typedef enum argus_error_type_e {
    ARGUS_SUCCESS = 0,             // Pas d'erreur
    ARGUS_SOULD_EXIT,              // Sortie normale après traitement d'option
    
    /* Erreurs de structure */
    ARGUS_ERROR_DUPLICATE_OPTION,  // Collision de nom d'option
    ARGUS_ERROR_INVALID_HANDLER,   // Fonction gestionnaire invalide
    /* Plus de codes d'erreur... */
    
    /* Erreurs d'analyse */
    ARGUS_ERROR_INVALID_ARGUMENT,  // Argument invalide
    ARGUS_ERROR_MISSING_VALUE,     // Valeur requise non fournie
    /* Plus de codes d'erreur... */
} argus_error_type_t;
```

### argus_error_t

Structure pour stocker des informations d'erreur détaillées :

```c
typedef struct argus_error_s {
    argus_error_context_t context;  // Contexte d'erreur
    int code;                       // Code d'erreur
    char message[ARGUS_MAX_ERROR_MESSAGE_SIZE];  // Message d'erreur
} argus_error_t;
```

## Constantes et limites

Argus définit plusieurs constantes qui contrôlent son comportement :

```c
#define MAX_SUBCOMMAND_DEPTH 8  // Profondeur maximale des sous-commandes imbriquées
#define ARGUS_MAX_ERROR_MESSAGE_SIZE 256  // Longueur maximale des messages d'erreur
#define ARGUS_MAX_ERRORS_STACK 16  // Nombre maximum d'erreurs dans la pile
#define MULTI_VALUE_INITIAL_CAPACITY 8  // Capacité initiale pour les collections
```

## Macros et catégories de types

Argus fournit plusieurs macros d'aide pour travailler avec les types de valeurs :

```c
// Macros de catégorie de type
#define VALUE_TYPE_PRIMITIVE \
    (VALUE_TYPE_INT | VALUE_TYPE_STRING | VALUE_TYPE_FLOAT | VALUE_TYPE_BOOL)
    
#define VALUE_TYPE_ARRAY \
    (VALUE_TYPE_ARRAY_STRING | VALUE_TYPE_ARRAY_INT | VALUE_TYPE_ARRAY_FLOAT)
    
#define VALUE_TYPE_MAP \
    (VALUE_TYPE_MAP_STRING | VALUE_TYPE_MAP_INT | VALUE_TYPE_MAP_FLOAT | VALUE_TYPE_MAP_BOOL)

// Macros de catégorie de drapeau
#define OPTION_FLAG_MASK \
    (FLAG_REQUIRED | FLAG_HIDDEN | FLAG_ADVANCED | FLAG_EXIT | VERSIONING_FLAG_MASK)
    
#define OPTION_ARRAY_FLAG_MASK \
    (FLAG_SORTED | FLAG_UNIQUE | VERSIONING_FLAG_MASK)
    
// Plus de masques de drapeaux...
```

## Types utilitaires

La bibliothèque inclut également plusieurs types utilitaires pour le suivi des erreurs et la gestion du contexte :

```c
typedef struct argus_error_context_s {
    const char *option_name;     // Nom d'option actuel
    const char *group_name;      // Nom de groupe actuel 
    const char *subcommand_name; // Nom de sous-commande actuel
} argus_error_context_t;

typedef struct argus_error_stack_s {
    argus_error_t errors[ARGUS_MAX_ERRORS_STACK];  // Tableau d'erreurs
    size_t count;                                  // Nombre d'erreurs
} argus_error_stack_t;
```

## Documentation associée

- [Référence des macros](macros.md) - Liste complète des macros de définition d'options
- [Référence des fonctions](functions.md) - Référence complète des fonctions API
- [Aperçu](overview.md) - Vue d'ensemble de haut niveau de l'API argus
