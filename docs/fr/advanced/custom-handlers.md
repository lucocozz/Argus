# Gestionnaires personnalisés

Les gestionnaires personnalisés étendent les capacités de argus en vous permettant de transformer et de valider les données d'entrée selon vos besoins spécifiques.

!!! abstract "Aperçu"
    Ce guide couvre le traitement avancé des entrées avec des gestionnaires personnalisés :
    
    - **Signature de fonction du gestionnaire** - Création de fonctions de traitement personnalisées
    - **Gestion de la mémoire** - Allocation et libération de structures de données personnalisées
    - **Transformation de données** - Conversion d'entrées brutes en données structurées
    - **Création de types personnalisés** - Définition de types de données spécialisés
    - **Intégration avec les validateurs** - Combinaison de gestionnaires avec la validation
    
    Pour la gestion des options de base, consultez le [guide des options de base](../guide/basic-options.md).

## Comprendre les gestionnaires

Alors que argus fournit des gestionnaires standard pour les types courants (chaîne, entier, flottant, booléen), les gestionnaires personnalisés permettent un traitement plus avancé :

- Transformation des entrées en structures de données complexes
- Analyse de formats spécialisés (hôte:port, coordonnées, couleurs)
- Conversion entre différentes représentations
- Validation avec des règles métier pendant le traitement

Contrairement aux validateurs qui vérifient simplement les valeurs, les gestionnaires personnalisés peuvent transformer des chaînes d'entrée en structures de données sophistiquées.

## Signature de fonction du gestionnaire

Toutes les fonctions de gestionnaire doivent suivre cette signature :

```c
int handler_function(argus_t *argus, argus_option_t *option, char *arg);
```

Paramètres :
- `argus` : Le contexte argus, utilisé pour le rapport d'erreurs
- `option` : L'option en cours de traitement, où stocker la valeur
- `arg` : La valeur de chaîne brute à traiter (NULL pour les drapeaux booléens)

Valeur de retour :
- `ARGUS_SUCCESS` (0) en cas de succès
- Tout code d'erreur (non nul) en cas d'échec

## Création d'un gestionnaire personnalisé

Examinons un exemple complet d'un gestionnaire personnalisé qui analyse un point de terminaison au format "hôte:port" :

```c
typedef struct {
    char* host;
    int port;
} endpoint_t;

int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "Le point de terminaison est requis");
    }
    
    // Allouer la structure de point de terminaison
    endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
    if (!endpoint) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Échec de l'allocation mémoire");
    }
    
    // Trouver le séparateur
    char *colon = strchr(arg, ':');
    if (!colon) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, 
                          "Format de point de terminaison invalide : %s (format attendu : hôte:port)", arg);
    }
    
    // Extraire l'hôte
    size_t host_len = colon - arg;
    endpoint->host = strndup(arg, host_len);
    if (!endpoint->host) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Échec de l'allocation mémoire");
    }
    
    // Extraire le port
    long port = strtol(colon + 1, NULL, 10);
    if (port <= 0 || port > 65535) {
        free(endpoint->host);
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                          "Port invalide : %ld (doit être entre 1 et 65535)", port);
    }
    
    endpoint->port = (int)port;
    
    // Stocker la structure de point de terminaison
    option->value.as_ptr = endpoint;
    option->is_allocated = true;  // Important : marquer comme alloué pour un nettoyage approprié
    return ARGUS_SUCCESS;
}
```

## Gestionnaires de libération

Lorsque votre gestionnaire alloue de la mémoire, vous devez fournir un gestionnaire de libération personnalisé :

```c
int endpoint_free_handler(argus_option_t *option)
{
    endpoint_t *endpoint = (endpoint_t*)option->value.as_ptr;
    if (endpoint) {
        free(endpoint->host);
        free(endpoint);
    }
    return ARGUS_SUCCESS;
}
```

## Utilisation de gestionnaires personnalisés

Pour utiliser un gestionnaire personnalisé, définissez une option avec la macro `OPTION_BASE` :

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Option avec gestionnaire personnalisé
    OPTION_BASE('e', "endpoint", HELP("Point de terminaison du serveur (hôte:port)"), 
                VALUE_TYPE_CUSTOM,
                HANDLER(endpoint_handler),
                FREE_HANDLER(endpoint_free_handler))
)
```

## Création de macros d'aide

Pour la réutilisabilité et un code plus propre, vous pouvez créer votre propre macro :

```c
// Macro d'aide pour les options de point de terminaison
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

// Utilisation dans les définitions d'options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Beaucoup plus propre avec une macro dédiée
    OPTION_ENDPOINT('e', "endpoint", HELP("Point de terminaison du serveur"))
)
```

## Accès aux valeurs personnalisées

Pour accéder aux valeurs traitées par des gestionnaires personnalisés :

```c
if (argus_is_set(argus, "endpoint")) {
    // Récupérer et convertir la valeur au type correct
    endpoint_t *endpoint = (endpoint_t*)argus_get(argus, "endpoint").as_ptr;
    
    // Utiliser les données structurées
    printf("Hôte : %s\n", endpoint->host);
    printf("Port : %d\n", endpoint->port);
}
```

## Techniques avancées

### Combinaison de gestionnaires et de validateurs

Vous pouvez utiliser des gestionnaires personnalisés avec des validateurs pour un traitement complexe :

```c
// Gestionnaire personnalisé pour le traitement
OPTION_BASE('e', "endpoint", HELP("Point de terminaison du serveur (hôte:port)"), 
            VALUE_TYPE_CUSTOM,
            HANDLER(endpoint_handler),
            FREE_HANDLER(endpoint_free_handler),
            VALIDATOR(endpoint_validator, NULL))  // Ajouter une validation supplémentaire
```

### Types personnalisés avec traitement spécialisé

Pour des types plus complexes, vous pouvez implémenter un traitement spécialisé :

=== "Traitement de couleur"
    ```c
    typedef struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } rgb_color_t;
    
    int color_handler(argus_t *argus, argus_option_t *option, char *arg)
    {
        // Allouer et initialiser la structure de couleur
        rgb_color_t *color = calloc(1, sizeof(rgb_color_t));
        
        // Gérer différents formats :
        // - Hex : #RRGGBB ou #RGB
        // - RGB : rgb(r,g,b)
        // - Couleurs nommées : rouge, vert, bleu, etc.
        
        // Stocker la structure de couleur
        option->value.as_ptr = color;
        option->is_allocated = true;
        return ARGUS_SUCCESS;
    }
    ```

=== "Traitement de coordonnées"
    ```c
    typedef struct {
        double lat;
        double lon;
    } geo_coord_t;
    
    int coordinate_handler(argus_t *argus, argus_option_t *option, char *arg)
    {
        // Allouer la structure de coordonnées
        geo_coord_t *coord = calloc(1, sizeof(geo_coord_t));
        
        // Analyser les formats :
        // - Décimal : "lat,lon" 
        // - DMS : "40°45'30"N,74°0'23"O"
        
        // Stocker la structure de coordonnées
        option->value.as_ptr = coord;
        option->is_allocated = true;
        return ARGUS_SUCCESS;
    }
    ```

## Rapport d'erreurs

Les gestionnaires personnalisés doivent utiliser la macro `ARGUS_REPORT_ERROR` pour signaler des erreurs :

```c
ARGUS_REPORT_ERROR(argus, error_code, format_string, ...);
```

Cette macro :
1. Formate un message d'erreur
2. Ajoute l'erreur à la pile d'erreurs
3. Renvoie le code d'erreur spécifié depuis votre fonction

Codes d'erreur courants :
- `ARGUS_ERROR_MISSING_VALUE` : Valeur requise mais non fournie
- `ARGUS_ERROR_INVALID_FORMAT` : Format de valeur incorrect
- `ARGUS_ERROR_INVALID_VALUE` : Valeur sémantiquement invalide
- `ARGUS_ERROR_MEMORY` : Échec de l'allocation mémoire

## Points clés de la gestion de la mémoire

Lorsque votre gestionnaire alloue de la mémoire :

1. Stockez la valeur dans `option->value` (généralement `option->value.as_ptr` pour les structures personnalisées)
2. Définissez `option->is_allocated = true` pour indiquer que la mémoire doit être libérée
3. Fournissez un gestionnaire de libération personnalisé avec `FREE_HANDLER()`
4. Assurez un nettoyage approprié dans les cas d'erreur

## Bonnes pratiques

### 1. Gestion robuste des erreurs

Implémentez toujours une gestion d'erreurs complète :

```c
// Allouer de la mémoire
endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
if (!endpoint) {
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Échec de l'allocation mémoire");
}

// Vérifier les erreurs de format
char *colon = strchr(arg, ':');
if (!colon) {
    free(endpoint);  // Nettoyer en cas d'erreur
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, 
                       "Format invalide : format attendu hôte:port");
}
```

### 2. Messages d'erreur clairs

Fournissez des messages d'erreur informatifs qui aident les utilisateurs :

=== "Bon message"
    ```c
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                     "Format de point de terminaison invalide : %s (format attendu : hôte:port)", arg);
    ```

=== "Mauvais message"
    ```c
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, "Entrée incorrecte");
    ```

### 3. Gestion cohérente de la mémoire

Suivez des modèles cohérents de gestion de la mémoire :

```c
int my_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    // 1. Valider l'entrée
    if (arg == NULL) { ... }
    
    // 2. Allouer la structure principale
    my_type_t *obj = calloc(1, sizeof(my_type_t));
    if (!obj) { ... }
    
    // 3. Traiter l'entrée et allouer de la mémoire supplémentaire si nécessaire
    // ...
    
    // 4. En cas d'erreur, nettoyer toutes les allocations et signaler l'erreur
    // ...
    
    // 5. En cas de succès, stocker le résultat et marquer comme alloué
    option->value.as_ptr = obj;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}
```

### 4. Macros d'aide spécifiques au type

Créez des macros d'aide pour les types personnalisés :

```c
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

#define OPTION_COLOR(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(color_handler), \
                FREE_HANDLER(color_free_handler), \
                ##__VA_ARGS__)
```

## Exemple complet

Voici un exemple complet implémentant un gestionnaire de point de terminaison personnalisé :

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure pour le point de terminaison
typedef struct {
    char* host;
    int port;
} endpoint_t;

// Gestionnaire personnalisé pour le point de terminaison "hôte:port"
int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "Le point de terminaison est requis");
    }
    
    // Allouer la structure de point de terminaison
    endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
    if (!endpoint) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Échec de l'allocation mémoire");
    }
    
    // Trouver le séparateur
    char *colon = strchr(arg, ':');
    if (!colon) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, 
                          "Format de point de terminaison invalide : %s (format attendu : hôte:port)", arg);
    }
    
    // Extraire l'hôte
    size_t host_len = colon - arg;
    endpoint->host = strndup(arg, host_len);
    if (!endpoint->host) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Échec de l'allocation mémoire");
    }
    
    // Extraire le port
    long port = strtol(colon + 1, NULL, 10);
    if (port <= 0 || port > 65535) {
        free(endpoint->host);
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                          "Port invalide : %ld (doit être entre 1 et 65535)", port);
    }
    
    endpoint->port = (int)port;
    
    // Stocker la structure de point de terminaison
    option->value.as_ptr = endpoint;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

// Gestionnaire de libération pour la structure de point de terminaison
int endpoint_free_handler(argus_option_t *option)
{
    endpoint_t *endpoint = (endpoint_t*)option->value.as_ptr;
    if (endpoint) {
        free(endpoint->host);
        free(endpoint);
    }
    return ARGUS_SUCCESS;
}

// Macro d'aide pour les options de point de terminaison
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

// Définition des options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Point de terminaison du serveur utilisant le gestionnaire personnalisé
    OPTION_ENDPOINT('s', "server", HELP("Point de terminaison du serveur (hôte:port)")),
    
    // Point de terminaison de la base de données avec valeur par défaut
    OPTION_ENDPOINT('d', "database", HELP("Point de terminaison de la base de données (hôte:port)"), 
                   DEFAULT("localhost:5432"))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "custom_handlers_example", "1.0.0");
    argus.description = "Exemple de gestionnaires personnalisés";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Traiter le point de terminaison du serveur s'il est fourni
    if (argus_is_set(argus, "server")) {
        endpoint_t *server = argus_get(argus, "server").as_ptr;
        
        printf("Informations du serveur :\n");
        printf("  Hôte : %s\n", server->host);
        printf("  Port : %d\n", server->port);
        printf("\n");
    }
    
    // Traiter le point de terminaison de la base de données s'il est fourni
    if (argus_is_set(argus, "database")) {
        endpoint_t *db = argus_get(argus, "database").as_ptr;
        
        printf("Informations de la base de données :\n");
        printf("  Hôte : %s\n", db->host);
        printf("  Port : %d\n", db->port);
        printf("\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

## Documentation connexe

- [Guide des options de base](../guide/basic-options.md) - Comprendre les types d'options standard
- [Guide des validateurs personnalisés](custom-validators.md) - Techniques de validation personnalisées
