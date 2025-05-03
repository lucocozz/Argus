# Expressions régulières

argus offre un support puissant pour les expressions régulières via PCRE2 (Perl Compatible Regular Expressions) afin de valider les entrées utilisateur avec une correspondance de motifs sophistiquée.

!!! abstract "Aperçu"
    Ce guide couvre l'utilisation avancée des expressions régulières dans argus :
    
    - Utilisation des motifs prédéfinis
    - Création de motifs personnalisés
    - Syntaxe des motifs et techniques
    - Bonnes pratiques pour la validation par expressions régulières
    
    Pour une liste complète des motifs prédéfinis, consultez la [Référence API des motifs d'expressions régulières](../api/regex_patterns.md).

## Utilisation des motifs prédéfinis

argus fournit une large gamme de motifs prédéfinis dans `argus/regex.h` qui couvrent des scénarios de validation courants.

=== "Utilisation de base"
    ```c
    #include "argus/regex.h"
    
    ARGUS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        // Validation d'email
        OPTION_STRING('e', "email", HELP("Adresse email"),
                    REGEX(ARGUS_RE_EMAIL)),
        
        // Validation d'adresse IP
        OPTION_STRING('i', "ip", HELP("Adresse IP"),
                    REGEX(ARGUS_RE_IPV4)),
        
        // Validation d'URL
        OPTION_STRING('u', "url", HELP("URL"),
                    REGEX(ARGUS_RE_URL))
    )
    ```

=== "Motifs multiples"
    ```c
    // Options avec différents types de validation
    ARGUS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        // Date au format ISO (AAAA-MM-JJ)
        OPTION_STRING('d', "date", HELP("Date (AAAA-MM-JJ)"),
                    REGEX(ARGUS_RE_ISO_DATE),
                    HINT("AAAA-MM-JJ")),
        
        // Mot de passe avec validation forte
        OPTION_STRING('p', "password", HELP("Mot de passe (8+ caractères, casse mixte, chiffres, symboles)"),
                    REGEX(ARGUS_RE_PASSWD_STRONG),
                    HINT("MotDePasse1@")),
        
        // Numéro de version sémantique
        OPTION_STRING('v', "version", HELP("Version sémantique"),
                    REGEX(ARGUS_RE_SEMVER),
                    HINT("X.Y.Z"),
                    FLAGS(FLAG_REQUIRED))
    )
    ```

## Création de motifs personnalisés

Bien que les motifs prédéfinis couvrent de nombreux besoins courants, vous pouvez également créer vos propres motifs pour des exigences spécifiques.

### Utilisation de MAKE_REGEX

La macro `MAKE_REGEX` crée un nouveau motif d'expression régulière avec une indication d'erreur :

=== "Définition de motif personnalisé"
    ```c
    // Définir un motif personnalisé pour les identifiants de produit
    #define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format : XX0000-XXXXXX")
    
    // Définir un motif personnalisé pour les noms simples
    #define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Lettres, chiffres, underscore, tiret")
    ```

=== "Utilisation des motifs personnalisés"
    ```c
    ARGUS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        // Utiliser des motifs personnalisés
        OPTION_STRING('p', "product", HELP("ID du produit"),
                    REGEX(RE_PRODUCT_ID)),
        
        OPTION_STRING('n', "name", HELP("Nom d'utilisateur"),
                    REGEX(RE_SIMPLE_NAME))
    )
    ```

### Motifs en ligne

Vous pouvez également définir des motifs en ligne sans créer de constante :

```c
OPTION_STRING('z', "zipcode", HELP("Code postal US"),
            REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format : 12345 ou 12345-6789")))
```

!!! note "Structure de MAKE_REGEX"
    La macro `MAKE_REGEX` prend deux paramètres :
    
    1. Le motif d'expression régulière
    2. Un texte d'explication qui sera affiché en cas d'échec de validation

## Combinaison de motifs

Pour des exigences de validation plus complexes, vous pouvez créer des motifs composites :

```c
// Motif pour les numéros de téléphone américains valides (formats multiples)
#define RE_US_PHONE_COMPLEX MAKE_REGEX(
    "^(\\+1[-\\s]?)?(\\([0-9]{3}\\)|[0-9]{3})[-\\s]?[0-9]{3}[-\\s]?[0-9]{4}$",
    "Format de téléphone US : 123-456-7890, (123) 456-7890, ou +1 123 456 7890"
)
```

## Messages d'erreur

Un avantage clé de l'implémentation des expressions régulières de argus est la possibilité de fournir des messages d'erreur utiles :

=== "Motif avec erreur détaillée"
    ```c
    // Définir un motif avec un message d'erreur utile
    #define RE_PASSWORD MAKE_REGEX(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$",
        "Le mot de passe doit contenir au moins 8 caractères incluant une majuscule, une minuscule, un chiffre et un caractère spécial"
    )
    ```

=== "Sortie d'erreur"
    ```
    my_program: Valeur invalide 'password123' : Le mot de passe doit contenir au moins 8 caractères incluant une majuscule, une minuscule, un chiffre et un caractère spécial
    ```

## Syntaxe des motifs

Puisque argus utilise PCRE2, vous avez accès à des fonctionnalités puissantes de correspondance de motifs :

### Éléments courants des motifs

| Élément | Description | Exemple |
|---------|-------------|---------|
| `^` | Début de chaîne | `^abc` - La chaîne commence par "abc" |
| `$` | Fin de chaîne | `xyz$` - La chaîne se termine par "xyz" |
| `[]` | Classe de caractères | `[abc]` - Correspond à a, b, ou c |
| `[^]` | Classe de caractères négative | `[^abc]` - Correspond à n'importe quel caractère sauf a, b, ou c |
| `\d` | Chiffre | `\d{3}` - Trois chiffres |
| `\w` | Caractère de mot | `\w+` - Un ou plusieurs caractères de mot |
| `\s` | Espace blanc | `\s*` - Zéro ou plusieurs espaces blancs |
| `*` | Zéro ou plus | `a*` - Zéro ou plusieurs caractères "a" |
| `+` | Un ou plus | `a+` - Un ou plusieurs caractères "a" |
| `?` | Zéro ou un | `a?` - Zéro ou un caractère "a" |
| `{n}` | Exactement n fois | `a{3}` - Exactement trois caractères "a" |
| `{n,m}` | Entre n et m fois | `a{2,4}` - Entre 2 et 4 caractères "a" |
| `(...)` | Groupe de capture | `(abc)+` - Une ou plusieurs occurrences de "abc" |
| `\1, \2, ...` | Références arrière | `(a)\\1` - "a" suivi d'un autre "a" |
| `a|b` | Alternative | `cat|dog` - Soit "cat" soit "dog" |

!!! warning "Échappement dans les chaînes C"
    Lors de l'écriture de motifs d'expressions régulières en C, n'oubliez pas de doubler les caractères d'échappement spéciaux :
    
    ```c
    // Double barre oblique inverse pour les caractères spéciaux des expressions régulières
    #define RE_DIGITS MAKE_REGEX("^\\d+$", "Chiffres uniquement")  // Notez la double barre oblique inverse
    ```

### Éléments avancés des motifs

PCRE2 prend également en charge des fonctionnalités avancées pour des besoins de validation complexes :

| Élément | Description | Exemple |
|---------|-------------|---------|
| `(?:...)` | Groupe sans capture | `(?:abc)+` - Une ou plusieurs occurrences de "abc" sans capture |
| `(?=...)` | Assertion positive anticipée | `a(?=b)` - "a" seulement s'il est suivi par "b" |
| `(?!...)` | Assertion négative anticipée | `a(?!b)` - "a" seulement s'il n'est pas suivi par "b" |
| `(?<=...)` | Assertion positive rétrospective | `(?<=a)b` - "b" seulement s'il est précédé par "a" |
| `(?<!...)` | Assertion négative rétrospective | `(?<!a)b` - "b" seulement s'il n'est pas précédé par "a" |
| `(*UTF)` | Mode Unicode | `(*UTF)\\p{L}+` - Une ou plusieurs lettres Unicode |
| `\p{...}` | Propriété Unicode | `\p{Lu}` - Lettre majuscule |

## Considérations de performance

La validation par expressions régulières peut être puissante, mais des motifs complexes peuvent impacter les performances :

!!! tip "Bonnes pratiques de performance"
    1. **Gardez les motifs simples** quand c'est possible
    2. **Évitez le retour en arrière excessif** (`.*` suivi de correspondances spécifiques)
    3. **Utilisez des ancres** (`^` et `$`) pour éviter les analyses inutiles
    4. **Préférez les groupes sans capture** (`(?:...)`) quand vous n'avez pas besoin de captures

## Détails d'implémentation

argus implémente la validation par expressions régulières grâce à la bibliothèque PCRE2 :

```c
int regex_validator(argus_t *argus, const char *value, validator_data_t data)
{
    const char *pattern = data.regex.pattern;
    
    // Compiler l'expression régulière
    pcre2_code *re = pcre2_compile(...);
    
    // Exécuter l'expression régulière sur la chaîne d'entrée
    int rc = pcre2_match(...);
    
    // Retourner le résultat de validation
    if (rc < 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                          "Valeur invalide '%s' : %s", value, data.regex.hint);
    }
    return ARGUS_SUCCESS;
}
```

## Bonnes pratiques

Lorsque vous utilisez la validation par expressions régulières avec argus :

1. **Utilisez des motifs prédéfinis** quand c'est possible pour les validations courantes
2. **Créez des messages d'erreur descriptifs** qui aident l'utilisateur à comprendre ce qui est requis
3. **Gardez les motifs concentrés** sur une seule préoccupation de validation
4. **Documentez les motifs complexes** avec des commentaires expliquant leur objectif
5. **Testez minutieusement** avec des entrées valides et invalides

## Exemple complet

Voici un exemple complet démontrant plusieurs techniques de validation par expressions régulières :

```c
#include "argus.h"
#include "argus/regex.h"
#include <stdio.h>

/* Motifs d'expressions régulières personnalisés */
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format : XX0000-XXXXXX")
#define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Lettres, chiffres, underscore, tiret")

ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),

    // Utilisation de motifs prédéfinis
    GROUP_START("Réseau et Communication", GROUP_DESC("Options liées au réseau")),
       OPTION_STRING('i', "ip", HELP("Adresse IPv4"),
                    REGEX(ARGUS_RE_IPV4)),

       OPTION_STRING('e', "email", HELP("Adresse email"),
                    REGEX(ARGUS_RE_EMAIL)),

        OPTION_STRING('u', "url", HELP("URL avec n'importe quel protocole"),
                    REGEX(ARGUS_RE_URL)),
    GROUP_END(),

    // Motifs personnalisés définis ci-dessus
    GROUP_START("Formats personnalisés", GROUP_DESC("Options avec des motifs d'expressions régulières personnalisés")),
        OPTION_STRING('p', "product", HELP("ID de produit (format : XX0000-XXXXXX)"),
                    REGEX(RE_PRODUCT_ID)),

        OPTION_STRING('n', "name", HELP("Nom d'utilisateur (lettres, chiffres, underscore, tiret)"),
                    REGEX(RE_SIMPLE_NAME)),
    GROUP_END(),

    // Motifs en ligne
    GROUP_START("Motifs en ligne", GROUP_DESC("Options avec des motifs d'expressions régulières en ligne")),
        OPTION_STRING('z', "zipcode", HELP("Code postal US"),
                    REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format : 12345 ou 12345-6789"))),

        OPTION_STRING('t', "time", HELP("Heure (format : HH:MM)"),
                    REGEX(MAKE_REGEX("^([01]?[0-9]|2[0-3]):[0-5][0-9]$", "Format : HH:MM"))),
    GROUP_END()
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "regex_example", "1.0.0");
    argus.description = "Exemple d'utilisation de la validation par expressions régulières avec des motifs prédéfinis et personnalisés";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    printf("Validation réussie ! Toutes les valeurs fournies correspondent aux motifs attendus.\n\n");

    // Afficher les valeurs validées
    printf("Réseau & Communication :\n");
    printf("  Adresse IP : %s\n", argus_is_set(argus, "ip") ?
           argus_get(argus, "ip").as_string : "(non fournie)");
    printf("  Email : %s\n", argus_is_set(argus, "email") ?
           argus_get(argus, "email").as_string : "(non fourni)");
    printf("  URL : %s\n", argus_is_set(argus, "url") ?
           argus_get(argus, "url").as_string : "(non fournie)");

    printf("\nFormats personnalisés :\n");
    printf("  ID de produit : %s\n", argus_is_set(argus, "product") ?
           argus_get(argus, "product").as_string : "(non fourni)");
    printf("  Nom d'utilisateur : %s\n", argus_is_set(argus, "name") ?
           argus_get(argus, "name").as_string : "(non fourni)");

    printf("\nMotifs en ligne :\n");
    printf("  Code postal : %s\n", argus_is_set(argus, "zipcode") ?
           argus_get(argus, "zipcode").as_string : "(non fourni)");
    printf("  Heure : %s\n", argus_is_set(argus, "time") ?
           argus_get(argus, "time").as_string : "(non fournie)");

    argus_free(&argus);
    return 0;
}
```

## Ressources

Pour plus d'informations sur les expressions régulières :

!!! tip "Ressources d'apprentissage"
    - [Regular Expressions Info](https://www.regular-expressions.info/)
    - [Syntaxe des motifs PCRE2](https://www.pcre.org/current/doc/html/pcre2pattern.html)
    - [RegExr : Outil de test en ligne](https://regexr.com/)

## Documentation associée

- [Guide de validation](../guide/validation.md) - Concepts de base de validation incluant les expressions régulières
- [Référence API des motifs d'expressions régulières](../api/regex_patterns.md) - Liste complète des motifs prédéfinis
- [Validateurs personnalisés](custom-validators.md) - Création de validateurs personnalisés, y compris les pré-validateurs
