# Référence des expressions régulières

argus inclut un riche ensemble de motifs d'expressions régulières prédéfinis dans l'en-tête `argus/regex.h`. Ces motifs peuvent être utilisés avec le validateur `REGEX()` pour valider les valeurs d'options selon des formats courants.

!!! tip "Utilisation"
    Pour utiliser ces motifs, incluez l'en-tête des expressions régulières et appliquez le motif avec le validateur `REGEX()` :
    
    ```c
    #include "argus/regex.h"
    
    OPTION_STRING('e', "email", HELP("Adresse email"), 
                 REGEX(ARGUS_RE_EMAIL))
    ```

## Réseau et communication

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_IPV4` | Adresse IPv4 | `xxx.xxx.xxx.xxx` | 192.168.1.1 |
| `ARGUS_RE_IP4CIDR` | IPv4 avec suffixe CIDR | `xxx.xxx.xxx.xxx/xx` | 192.168.1.0/24 |
| `ARGUS_RE_IPV6` | Adresse IPv6 | `xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx` | 2001:0db8:85a3:0000:0000:8a2e:0370:7334 |
| `ARGUS_RE_MAC` | Adresse MAC | `xx:xx:xx:xx:xx:xx` ou `xx-xx-xx-xx-xx-xx` | 01:23:45:67:89:AB |
| `ARGUS_RE_DOMAIN` | Nom de domaine pleinement qualifié | `example.com` | mydomain.example.com |
| `ARGUS_RE_URL` | URL avec n'importe quel protocole | `protocole://domaine/chemin` | https://example.com/path |
| `ARGUS_RE_HTTP` | URL HTTP/HTTPS | `http(s)://domaine/chemin` | https://example.com |
| `ARGUS_RE_FILE_URL` | URL de fichier | `file://chemin` | file:///path/to/file |
| `ARGUS_RE_PORT` | Numéro de port (1-65535) | `1-65535` | 8080 |

## Email

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_EMAIL` | Validation d'email basique | `utilisateur@domaine.tld` | user@example.com |
| `ARGUS_RE_EMAIL_STRICT` | Validation d'email stricte (conforme RFC) | `utilisateur@domaine.tld` | user.name+tag@example.co.uk |

## Date et heure

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_ISO_DATE` | Date ISO 8601 | `AAAA-MM-JJ` | 2023-01-31 |
| `ARGUS_RE_ISOTIME` | Date-heure ISO 8601 | `AAAA-MM-JJThh:mm:ss` | 2023-01-31T14:30:00 |
| `ARGUS_RE_US_DATE` | Format de date US | `MM/JJ/AAAA` | 01/31/2023 |
| `ARGUS_RE_EU_DATE` | Format de date européen | `JJ/MM/AAAA` | 31/01/2023 |
| `ARGUS_RE_TIME24` | Format d'heure 24h | `hh:mm[:ss]` | 14:30:45 |

## Numéros de téléphone

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_PHONE_INTL` | Numéro de téléphone international | `+[pays][numéro]` | +12345678901 |
| `ARGUS_RE_PHONE_US` | Numéro de téléphone nord-américain | `xxx-xxx-xxxx` ou `(xxx) xxx-xxxx` | 123-456-7890 |
| `ARGUS_RE_PHONE_EU` | Numéro de téléphone européen (général) | `+xx xx xxx xx xx` | +33 1 23 45 67 89 |

## Identité et sécurité

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_USER` | Nom d'utilisateur | `3-20 caractères : a-z, A-Z, 0-9, _, -` | john_doe-123 |
| `ARGUS_RE_PASSWD` | Mot de passe simple | `8+ caractères, lettres & chiffres` | password123 |
| `ARGUS_RE_PASSWD_STRONG` | Mot de passe fort | `8+ caractères avec minuscules, majuscules, chiffres, symboles` | P@ssw0rd! |
| `ARGUS_RE_UUID` | UUID (version 4) | `xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx` | 123e4567-e89b-42d3-a456-556642440000 |

## Géographie et localisation

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_ZIP` | Code postal US | `xxxxx` ou `xxxxx-xxxx` | 12345-6789 |
| `ARGUS_RE_UK_POST` | Code postal britannique | `XX[X] X[XX]` | SW1A 1AA |
| `ARGUS_RE_CA_POST` | Code postal canadien | `XnX nXn` | A1A 1A1 |
| `ARGUS_RE_LATITUDE` | Latitude | `-90 à 90 avec décimales` | 40.7128 |
| `ARGUS_RE_LONGITUDE` | Longitude | `-180 à 180 avec décimales` | -74.0060 |

## Fichiers et chemins

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_UNIX_PATH` | Chemin absolu Unix | `/chemin/vers/fichier` | /home/user/document.txt |
| `ARGUS_RE_WIN_PATH` | Chemin absolu Windows | `X:\chemin\vers\fichier` | C:\Users\User\Document.txt |
| `ARGUS_RE_FILENAME` | Nom de fichier avec extension | `nom.ext` | document.txt |

## Nombres et codes

| Motif | Description | Format | Exemple |
|---------|-------------|--------|---------|
| `ARGUS_RE_HEX_COLOR` | Couleur hexadécimale | `#RRGGBB` ou `#RGB` | #FF00FF |
| `ARGUS_RE_RGB` | Couleur RGB | `rgb(r,g,b)` | rgb(255,0,255) |
| `ARGUS_RE_SEMVER` | Version sémantique | `X.Y.Z[-pre][+build]` | 1.2.3-alpha+001 |
| `ARGUS_RE_POS_INT` | Entier positif | `1+` | 42 |
| `ARGUS_RE_NEG_INT` | Entier négatif | `-1-` | -42 |
| `ARGUS_RE_FLOAT` | Nombre à virgule flottante | `±n.n` | -3.14 |
| `ARGUS_RE_HEX` | Nombre hexadécimal | `0xnnnn` | 0x1A3F |

## Motifs personnalisés

Vous pouvez définir vos propres motifs d'expressions régulières en utilisant la macro `MAKE_REGEX` :

=== "Définition du motif"
    ```c
    #define RE_PRODUCT_ID MAKE_REGEX(
        "^[A-Z]{2}\\d{4}$", 
        "Format : XX0000"
    )
    ```

=== "Utilisation"
    ```c
    OPTION_STRING('p', "product", HELP("ID du produit"), 
                 REGEX(RE_PRODUCT_ID))
    ```

La macro `MAKE_REGEX` prend deux paramètres :
1. Le motif d'expression régulière
2. Un indice ou message d'erreur affiché quand la validation échoue

!!! note "Échappement dans les chaînes C"
    Lors de l'écriture de motifs d'expressions régulières en code C, n'oubliez pas de doubler les caractères d'échappement spéciaux :
    
    - Utilisez `\\d` au lieu de `\d` pour les chiffres
    - Utilisez `\\w` au lieu de `\w` pour les caractères de mot
    - Utilisez `\\s` au lieu de `\s` pour les espaces

## Fonctionnalités PCRE2

argus utilise PCRE2 (Perl Compatible Regular Expressions) pour la fonctionnalité d'expressions régulières, donnant accès à des capacités avancées de correspondance de motifs :

- Classes de caractères (`\d`, `\w`, `\s`)
- Quantificateurs (`*`, `+`, `?`, `{n,m}`)
- Alternance (`a|b`)
- Groupement (`(abc)`)
- Ancres (`^`, `$`)
- Assertions (lookahead, lookbehind)
- Support Unicode

!!! tip "Apprendre les expressions régulières"
    Pour plus d'informations sur la création et l'utilisation d'expressions régulières, consultez ces ressources :
    
    - [Regular Expressions Info](https://www.regular-expressions.info/)
    - [Documentation PCRE2](https://www.pcre.org/current/doc/html/)
    - [RegExr : Apprendre, construire et tester les RegEx](https://regexr.com/)

## Documentation associée

Pour plus d'informations sur l'utilisation des expressions régulières avec argus :

- [Guide de validation](../guide/validation.md) - Validation de base avec expressions régulières
- [Guide avancé d'expressions régulières](../advanced/regex.md) - Utilisation détaillée des expressions régulières
