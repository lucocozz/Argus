# Validation

argus offre de solides capacités de validation pour garantir que les entrées de ligne de commande répondent aux exigences de votre application.

!!! abstract "Aperçu"
    La validation dans argus est organisée en plusieurs catégories :
    
    - **Validateurs intégrés** - Validation simple avec `RANGE` et `CHOICES`
    - **Validation par expression régulière** - Validation de motifs de chaînes avec PCRE2
    - **Validateurs personnalisés** - Créez votre propre logique de validation
    
    Ce guide couvre les bases de chaque approche. Pour une utilisation avancée, consultez les guides spécialisés référencés tout au long du document.

## Validateurs intégrés

argus fournit plusieurs validateurs intégrés pour simplifier les scénarios de validation courants. Ceux-ci peuvent être appliqués directement aux définitions d'options.

### Validation de plage

Le validateur `RANGE` garantit que les valeurs numériques se situent dans une plage spécifiée :

=== "Définition"
    ```c
    OPTION_INT('p', "port", HELP("Numéro de port"),
               RANGE(1, 65535),  // Doit être entre 1 et 65535
               DEFAULT(8080))
    ```

=== "Exemple d'utilisation"
    ```bash
    $ ./my_program --port=9000  # Valide
    $ ./my_program --port=100000  # Erreur : La valeur 100000 est hors de la plage [1, 65535]
    ```

### Validation de choix

Le validateur `CHOICES` garantit que la valeur est l'une d'un ensemble spécifique :

=== "Choix de chaînes"
    ```c
    OPTION_STRING('l', "level", HELP("Niveau de journalisation"),
                  CHOICES_STRING("debug", "info", "warning", "error"),
                  DEFAULT("info"))
    ```

=== "Choix d'entiers"
    ```c
    OPTION_INT('m', "mode", HELP("Mode de fonctionnement"),
               CHOICES_INT(1, 2, 3),
               DEFAULT(1))
    ```

=== "Choix de flottants"
    ```c
    OPTION_FLOAT('s', "scale", HELP("Facteur d'échelle"),
                CHOICES_FLOAT(0.5, 1.0, 2.0),
                DEFAULT(1.0))
    ```

## Validation par expression régulière

argus utilise PCRE2 pour une validation puissante par expression régulière :

=== "Utilisation de base"
    ```c
    OPTION_STRING('e', "email", HELP("Adresse email"),
                  REGEX(ARGUS_RE_EMAIL))  // Doit être un email valide
    ```

=== "Motif personnalisé"
    ```c
    OPTION_STRING('i', "id", HELP("ID de produit"),
                  REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format : XX0000")))
    ```

!!! tip "Motifs prédéfinis"
    argus inclut de nombreux motifs prédéfinis dans `argus/regex.h` :
    
    | Constante | Valide | Exemple |
    |----------|-----------|---------|
    | `ARGUS_RE_EMAIL` | Adresses email | user@example.com |
    | `ARGUS_RE_IPV4` | Adresses IPv4 | 192.168.1.1 |
    | `ARGUS_RE_URL` | URLs | https://example.com |
    | `ARGUS_RE_ISO_DATE` | Dates au format ISO | 2023-01-31 |
    
    Pour une liste complète, consultez la [référence API des expressions régulières](../api/regex.md).
    
    Pour une utilisation avancée des regex, consultez le [guide des expressions régulières](../advanced/regex.md).

### Validation de longueur

Le validateur `LENGTH` garantit que les chaînes de caractères ont une longueur appropriée :

```c
OPTION_STRING('u', "username", HELP("Nom d'utilisateur"),
              LENGTH(3, 20),  // Doit comporter entre 3 et 20 caractères
              DEFAULT("utilisateur"))
```

### Validation de nombre d'éléments

Le validateur `COUNT` garantit que les collections (tableaux et maps) ont un nombre approprié d'éléments :

```c
OPTION_ARRAY_STRING('t', "tags", HELP("Tags pour la ressource"),
                   COUNT(1, 5),  // Doit avoir entre 1 et 5 tags
                   FLAGS(FLAG_UNIQUE))
```

## Validateurs personnalisés

Pour une logique de validation plus complexe, vous pouvez créer vos propres validateurs :

=== "Validateur simple"
    ```c
    int even_validator(argus_t *argus, void *option_ptr, validator_data_t data)
    {
        argus_option_t *option = (argus_option_t *)option_ptr;
        
        if (option->value.as_int % 2 != 0) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                             "La valeur doit être un nombre pair");
        }
        return ARGUS_SUCCESS;
    }
    
    // Utilisation avec MAKE_VALIDATOR
    #define V_EVEN() \
        MAKE_VALIDATOR(even_validator, _V_DATA_CUSTOM_(NULL), ORDER_POST)
    
    OPTION_INT('n', "number", HELP("Un nombre pair"), 
              VALIDATOR(V_EVEN()))
    ```

=== "Validateur avec données personnalisées"
    ```c
    int divisible_validator(argus_t *argus, void *option_ptr, validator_data_t data)
    {
        argus_option_t *option = (argus_option_t *)option_ptr;
        int divisor = (int)data.custom;
        
        if (option->value.as_int % divisor != 0) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                              "La valeur doit être divisible par %d", divisor);
        }
        return ARGUS_SUCCESS;
    }
    
    // Utilisation avec données personnalisées
    #define V_DIVISIBLE_BY(n) \
        MAKE_VALIDATOR(divisible_validator, _V_DATA_CUSTOM_(n), ORDER_POST)
    
    OPTION_INT('n', "number", HELP("Nombre divisible par 5"), 
              VALIDATOR(V_DIVISIBLE_BY(5)))
    ```

=== "Validateurs multiples"
    ```c
    // Combiner plusieurs validateurs
    OPTION_INT('p', "port", HELP("Port pair dans une plage valide"), 
              VALIDATOR(V_RANGE(1, 65535), V_EVEN()))
    
    OPTION_STRING('e', "email", HELP("Email avec validation de longueur"),
                 VALIDATOR(V_LENGTH(5, 50), V_REGEX(email_regex)))
    ```

!!! info "Types de validateurs et nouvelle API"
    argus prend en charge les validateurs avec ordre explicite :
    
    1. **ORDER_PRE** - Valider la **chaîne brute** avant tout traitement
    2. **ORDER_POST** - Valider la valeur **traitée** après conversion vers son type final
    
    Utilisez `MAKE_VALIDATOR(fonction, données, ordre)` pour créer des validateurs avec la nouvelle API.
    Les validateurs intégrés comme `V_RANGE()`, `V_LENGTH()`, et `V_COUNT()` sont prêts à utiliser.
    
    Pour une exploration détaillée des validateurs personnalisés, incluant des exemples et bonnes pratiques, 
    consultez le guide [Validateurs personnalisés](../advanced/custom-validators.md).

## Combinaison de validateurs

Vous pouvez appliquer plusieurs validateurs à une seule option en utilisant plusieurs entrées de validateurs dans la macro `VALIDATOR()` :

```c
OPTION_INT('p', "port", HELP("Numéro de port"), 
          VALIDATOR(V_RANGE(1, 65535), V_EVEN()),  // Validateurs multiples
          DEFAULT(8080))
```

## Rapport d'erreurs

Les validateurs doivent utiliser `ARGUS_REPORT_ERROR` pour fournir des messages d'erreur clairs :

```c
ARGUS_REPORT_ERROR(argus, error_code, format_string, ...);
```

!!! example "Exemple de message d'erreur"
    ```
    my_program : La valeur '70000' est en dehors de la plage [1, 65535] pour l'option 'port'
    ```

Les codes d'erreur courants incluent :

- `ARGUS_ERROR_INVALID_VALUE` : La valeur ne répond pas aux exigences
- `ARGUS_ERROR_INVALID_RANGE` : Valeur hors de la plage autorisée
- `ARGUS_ERROR_INVALID_FORMAT` : La valeur a un format incorrect

## Exemples complets

Pour des exemples fonctionnels complets de techniques de validation :

!!! tip "Fichiers d'exemple"
    - `examples/validators.c` - Démontre toutes les techniques de validation
    - `examples/regex.c` - Axé sur la validation par expression régulière

## Ressources supplémentaires

Pour une couverture plus approfondie des sujets de validation, consultez ces guides avancés :

- [Validateurs personnalisés](../advanced/custom-validators.md) - Création de validateurs personnalisés avec une logique spécialisée
- [Expressions régulières](../advanced/regex.md) - Guide détaillé de la validation par motifs regex
- [Motifs regex prédéfinis](../api/regex_patterns.md) - Liste des motifs d'expressions régulières prédéfinis dans argus
