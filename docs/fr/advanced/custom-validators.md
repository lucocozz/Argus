# Validateurs personnalisés

Ce guide explique comment créer et utiliser des validateurs personnalisés avec argus pour implémenter une logique de validation spécialisée pour les options de ligne de commande.

## Vue d'ensemble

La validation est essentielle pour s'assurer que les entrées en ligne de commande répondent aux exigences de votre application. Bien que argus fournisse des validateurs intégrés comme `RANGE()` et `REGEX()`, les validateurs personnalisés vous permettent d'implémenter une logique de validation spécifique à votre application.

Dans ce guide, vous apprendrez :

- Les deux types de validateurs : **validateurs** et **pré-validateurs**
- Comment créer et utiliser des validateurs personnalisés
- Les techniques pour transmettre des données de configuration aux validateurs
- Les bonnes pratiques pour l'implémentation des validateurs

## Comprendre les types de validateurs

Argus prend en charge deux types distincts de fonctions de validation personnalisées, chacune ayant un objectif spécifique :

### Validateurs

**Objectif** : Vérifier la valeur **traitée** après conversion de type

**Quand utiliser** : Lors de la validation basée sur le type de données final (int, float, string, etc.)

**Signature de fonction** :
```c
int validator_function(argus_t *argus, argus_option_t *option, validator_data_t data);
```

### Pré-validateurs

**Objectif** : Vérifier la **chaîne brute** avant qu'elle ne soit traitée

**Quand utiliser** : Quand vous avez besoin de :
- Valider le format de chaîne avant les tentatives d'analyse
- Effectuer une validation complexe de chaîne
- Prévenir les erreurs de conversion de type

**Signature de fonction** :
```c
int pre_validator_function(argus_t *argus, const char *value, validator_data_t data);
```

## Création de validateurs basiques

Commençons par des exemples simples des deux types de validateurs.

### Exemple : Validateur de nombres pairs

Ce validateur s'assure que les options entières ont des valeurs paires :

```c
int even_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Pas d'utilisation de données personnalisées dans cet exemple
    UNUSED(data);
    
    if (option->value.as_int % 2 != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Value must be an even number, got %d", option->value.as_int);
    }
    return ARGUS_SUCCESS;
}
```

**Utilisation du validateur** :

```c
OPTION_INT('n', "number", HELP("An even number"), 
          VALIDATOR(even_validator, NULL))
```

### Exemple : Pré-validateur de longueur de chaîne

Ce pré-validateur vérifie si une chaîne répond à une exigence de longueur minimale :

```c
int string_length_pre_validator(argus_t *argus, const char *value, validator_data_t data)
{
    // Obtenir la longueur minimale à partir des données du validateur
    size_t min_length = *(size_t *)data.custom;
    
    if (strlen(value) < min_length) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "String must be at least %zu characters long", min_length);
    }
    return ARGUS_SUCCESS;
}
```

**Utilisation du pré-validateur** :

```c
// Définir la contrainte de validation
size_t min_length = 8;

OPTION_STRING('p', "password", HELP("Password"),
             PRE_VALIDATOR(string_length_pre_validator, &min_length))
```

## Transmission de données aux validateurs

Le paramètre `validator_data_t` vous permet de transmettre des données de configuration à vos validateurs, les rendant plus flexibles et réutilisables.

### Utilisation de structures de données personnalisées

Pour des règles de validation complexes, vous pouvez créer une structure pour contenir plusieurs paramètres :

```c
typedef struct {
    int min_value;
    int max_value;
    bool allow_odd;
} number_constraints_t;

int number_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Obtenir les contraintes à partir des données du validateur
    number_constraints_t *constraints = (number_constraints_t *)data.custom;
    
    // Validation de plage
    if (option->value.as_int < constraints->min_value || 
        option->value.as_int > constraints->max_value) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE,
                          "Value must be between %d and %d", 
                          constraints->min_value, constraints->max_value);
    }
    
    // Validation pair/impair
    if (!constraints->allow_odd && (option->value.as_int % 2 != 0)) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return ARGUS_SUCCESS;
}
```

**Utilisation du validateur avec des données personnalisées** :

```c
// Définir les contraintes
static number_constraints_t constraints = {
    .min_value = 10,
    .max_value = 100,
    .allow_odd = false
};

OPTION_INT('n', "number", HELP("A number with constraints"), 
          VALIDATOR(number_validator, &constraints))
```

### Utilisation de "Inline Compound Literals"

Pour les cas simples, vous pouvez utiliser des "compound literals" C99 pour passer des données en ligne :

```c
OPTION_STRING('u', "username", HELP("Username"),
             PRE_VALIDATOR(string_length_pre_validator, &((size_t){3})))
```

Cela crée une variable anonyme `size_t` avec la valeur 3 et transmet son adresse au validateur.

## Techniques de validation avancées

### Validation contextuelle

Parfois, les validateurs doivent vérifier les valeurs par rapport à d'autres options :

```c
typedef struct {
    const char *related_option;
} option_relation_t;

int greater_than_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    option_relation_t *relation = (option_relation_t *)data.custom;
    argus_value_t other_value = argus_get(*argus, relation->related_option);
    
    if (option->value.as_int <= other_value.as_int) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be greater than '%s' (%d)",
                          relation->related_option, other_value.as_int);
    }
    
    return ARGUS_SUCCESS;
}
```

**Exemple d'utilisation** :
```c
static option_relation_t max_relation = { .related_option = "min" };

ARGUS_OPTIONS(
    options,
    OPTION_INT('n', "min", HELP("Minimum value")),
    OPTION_INT('x', "max", HELP("Maximum value"), 
               VALIDATOR(greater_than_validator, &max_relation))
)
```

### Création de macros d'aide

Pour les modèles de validation fréquemment utilisés, créez des macros d'aide :

```c
// Macro d'aide pour la validation des nombres pairs
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)

// Macro d'aide pour la longueur minimale de chaîne
#define MIN_LENGTH(min) \
    PRE_VALIDATOR(string_length_pre_validator, &((size_t){min}))

// Macro d'aide pour la longueur maximale de chaîne
#define MAX_LENGTH(max) \
    PRE_VALIDATOR(string_length_max_validator, &((size_t){max}))

// Vérification de longueur combinée
#define STRING_LENGTH(min, max) \
    PRE_VALIDATOR(string_length_range_validator, &((length_range_t){min, max}))
```

**Exemple d'utilisation** :
```c
ARGUS_OPTIONS(
    options,
    OPTION_INT('n', "number", HELP("An even number"), EVEN_NUMBER()),
    OPTION_STRING('p', "password", HELP("Password"), MIN_LENGTH(8)),
    OPTION_STRING('u', "username", HELP("Username"), STRING_LENGTH(3, 20))
)
```

## Combinaison de plusieurs validateurs

Argus vous permet d'appliquer plusieurs validateurs à une seule option en utilisant les macros de validateur numérotées :

```c
OPTION_INT('p', "port", HELP("Port number"), 
          VALIDATOR(is_even_validator, NULL),      // Premier validateur
          VALIDATOR2(range_validator, &port_range), // Deuxième validateur
          VALIDATOR3(port_validator, NULL))        // Troisième validateur
```

Argus a une limite de 4 validateurs par option, mais vous pouvez modifier la constante `ARGUS_MAX_VALIDATORS` pour augmenter cette limite.

Notez que les validateurs intégrés comme `RANGE()`, `LENGTH()`, et `COUNT()` utilisent le premier emplacement de validateur. Vous pouvez les combiner avec des validateurs personnalisés en utilisant le deuxième et les emplacements suivants :

```c
OPTION_INT('p', "port", HELP("Port number"),
          RANGE(1, 65535),                     // Utilise le premier emplacement de validateur
          VALIDATOR2(is_even_validator, NULL)) // Utilise le deuxième emplacement de validateur
```

## Signalement d'erreurs

Les validateurs doivent utiliser `ARGUS_REPORT_ERROR` pour fournir des messages d'erreur clairs :

```c
ARGUS_REPORT_ERROR(argus, error_code, format_string, ...);
```

**Codes d'erreur courants** :

| Code d'erreur | Description | Utilisation typique |
|------------|-------------|-------------|
| `ARGUS_ERROR_INVALID_VALUE` | La valeur ne répond pas aux exigences | Échecs de validation généraux |
| `ARGUS_ERROR_INVALID_RANGE` | Valeur hors de la plage autorisée | Validation de plage |
| `ARGUS_ERROR_INVALID_FORMAT` | La valeur a un format incorrect | Validation de format |
| `ARGUS_ERROR_MEMORY` | L'allocation de mémoire a échoué | Pendant le traitement de validation |

## Bonnes pratiques

### 1. Responsabilité unique

Chaque validateur devrait se concentrer sur une préoccupation de validation :

```c
// Bien : Deux validateurs ciblés
int is_even_validator(argus_t *argus, argus_option_t *option, validator_data_t data);
int in_range_validator(argus_t *argus, argus_option_t *option, validator_data_t data);

// Les utiliser ensemble
OPTION_INT('n', "number", HELP("Number"), 
          VALIDATOR(is_even_validator, NULL),
          VALIDATOR2(in_range_validator, &range))
```

### 2. Messages d'erreur descriptifs

Fournissez des messages d'erreur clairs et exploitables :

```c
// Bien : Message clair et spécifique
ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                  "Username must be 3-20 characters with only letters, numbers, and underscores");

// Mal : Message vague
ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Invalid input");
```

### 3. Sécurité des paramètres

Validez toujours les paramètres et gérez les cas limites :

```c
int string_length_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Vérifiez si la valeur est NULL avant de l'utiliser
    if (option->value.as_string == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "String cannot be NULL");
    }
    
    // Reste de la logique de validation...
    return ARGUS_SUCCESS;
}
```

### 4. Efficacité mémoire

Évitez les allocations de tas inutiles dans les validateurs :

```c
int efficient_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Utilisez des tampons basés sur la pile pour les opérations temporaires
    char buffer[256];
    
    // Traitez la valeur sans allocations de tas
    
    return ARGUS_SUCCESS;
}
```

### 5. Composants réutilisables

Concevez des validateurs pour être réutilisables entre les options :

```c
// Validateur générique pour vérifier si un nombre est divisible par n
int divisible_by_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    int divisor = *(int *)data.custom;
    
    if (option->value.as_int % divisor != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be divisible by %d", divisor);
    }
    
    return ARGUS_SUCCESS;
}

// Réutilisation avec différentes configurations
OPTION_INT('n', "number", HELP("Number divisible by 2"), 
          VALIDATOR(divisible_by_validator, &((int){2})));

OPTION_INT('m', "multiple", HELP("Multiple of 5"), 
          VALIDATOR(divisible_by_validator, &((int){5})));
```

## Exemple complet

Voici un exemple complet démontrant diverses techniques de validateur personnalisé :

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Validateur personnalisé pour les adresses email
int email_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    (void)data; // Paramètre non utilisé
    
    const char* email = option->value.as_string;
    if (!email) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email address cannot be NULL");
    }
    
    // Vérifier le caractère @
    const char* at = strchr(email, '@');
    if (!at) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email address must contain an '@' character");
    }
    
    // Vérifier le domaine
    const char* dot = strchr(at, '.');
    if (!dot) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email domain must contain a '.' character");
    }
    
    return ARGUS_SUCCESS;
}

// Validateur personnalisé pour les nombres pairs
int even_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    (void)data; // Paramètre non utilisé
    
    int number = option->value.as_int;
    if (number % 2 != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return ARGUS_SUCCESS;
}

// Pré-validateur personnalisé pour les exigences de casse des chaînes
int case_pre_validator(argus_t *argus, const char *value, validator_data_t data)
{
    typedef enum { LOWERCASE, UPPERCASE, MIXED } case_requirement_t;
    case_requirement_t req = *(case_requirement_t *)data.custom;
    
    bool has_upper = false;
    bool has_lower = false;
    
    for (const char *p = value; *p; p++) {
        if (isupper(*p)) has_upper = true;
        if (islower(*p)) has_lower = true;
    }
    
    switch (req) {
        case LOWERCASE:
            if (has_upper) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                 "Value must be lowercase only");
            }
            break;
            
        case UPPERCASE:
            if (has_lower) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                 "Value must be uppercase only");
            }
            break;
            
        case MIXED:
            if (!has_upper || !has_lower) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                 "Value must contain both uppercase and lowercase letters");
            }
            break;
    }
    
    return ARGUS_SUCCESS;
}

// Macros d'aide pour les validations courantes
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)
#define EMAIL_VALIDATOR() VALIDATOR(email_validator, NULL)
#define LOWERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){LOWERCASE}))
#define UPPERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){UPPERCASE}))
#define MIXED_CASE() PRE_VALIDATOR(case_pre_validator, &((int){MIXED}))

ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Validateur de plage intégré
    OPTION_INT('p', "port", HELP("Port number"), 
                DEFAULT(8080), RANGE(1, 65535)),
    
    // Validateur de choix intégré
    OPTION_STRING('l', "log-level", HELP("Log level"), 
                DEFAULT("info"), 
                CHOICES_STRING("debug", "info", "warning", "error")),
    
    // Validateur d'email personnalisé
    OPTION_STRING('e', "email", HELP("Email address"),
                EMAIL_VALIDATOR()),
    
    // Validateur de nombre pair personnalisé
    OPTION_INT('n', "number", HELP("An even number"),
                EVEN_NUMBER(),
                DEFAULT(42)),
    
    // Chaîne avec validation de casse
    OPTION_STRING('u', "username", HELP("Username (lowercase)"),
                LOWERCASE_ONLY()),
                
    // Chaîne avec plusieurs validateurs
    OPTION_STRING('p', "password", HELP("Password (mixed case)"),
                MIXED_CASE())
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "validators_example", "1.0.0");
    argus.description = "Example of custom validators";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Accéder aux valeurs analysées
    int port = argus_get(argus, "port").as_int;
    const char* log_level = argus_get(argus, "log-level").as_string;
    int number = argus_get(argus, "number").as_int;
    
    const char* email = argus_is_set(argus, "email") ? 
                        argus_get(argus, "email").as_string : "not set";
    
    const char* username = argus_is_set(argus, "username") ?
                          argus_get(argus, "username").as_string : "not set";
    
    const char* password = argus_is_set(argus, "password") ?
                          argus_get(argus, "password").as_string : "not set";
    
    printf("Validated values:\n");
    printf("  Port: %d (range: 1-65535)\n", port);
    printf("  Log level: %s (choices: debug, info, warning, error)\n", log_level);
    printf("  Even number: %d (must be even)\n", number);
    printf("  Email: %s (must be valid email format)\n", email);
    printf("  Username: %s (must be lowercase)\n", username);
    printf("  Password: %s (must contain mixed case)\n", password);
    
    argus_free(&argus);
    return 0;
}
```

## Documentation connexe

- [Guide de validation](../guide/validation.md) - Concepts de base de la validation
- [Guide des expressions régulières](regex.md) - Validation avec des motifs d'expressions régulières
