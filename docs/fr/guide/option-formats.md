# Formats d'options pris en charge

argus prend en charge plusieurs formats d'options pour offrir une flexibilité maximale aux utilisateurs finaux. Cette flexibilité permet à votre programme de gérer différentes conventions de ligne de commande et de s'adapter aux diverses préférences des utilisateurs.

## Options standard

!!! abstract "Aperçu"
    argus reconnaît plusieurs formats standard pour les options de ligne de commande, notamment les options longues de style GNU, les options courtes de style POSIX, et diverses combinaisons.

### Options longues

Pour les options avec un nom long (`--option`) :

=== "Format avec espace"
    ```bash
    --option valeur
    ```

=== "Format avec égal"
    ```bash
    --option=valeur
    ```

!!! example "Exemple"
    ```bash
    # Les deux formats sont équivalents
    my_program --output file.txt
    my_program --output=file.txt
    ```

### Options courtes

Pour les options avec un nom court (`-o`) :

=== "Format avec espace"
    ```bash
    -o valeur
    ```

=== "Format sans espace"
    ```bash
    -ovaleur
    ```

=== "Drapeaux combinés"
    ```bash
    -abc    # Équivalent à -a -b -c
    ```

=== "Combiné avec une valeur"
    ```bash
    -abcvaleur    # Équivalent à -a -b -c valeur
    ```

!!! example "Exemple"
    ```bash
    # Tous les formats sont équivalents
    my_program -o file.txt
    my_program -ofile.txt
    
    # Combinaisons de drapeaux
    my_program -vxz       # Active les options -v, -x et -z
    my_program -vxooutput.txt  # Active -v et -x, puis définit -o à "output.txt"
    ```

### Terminateur d'options (`--`)

!!! tip
    Le double tiret `--` est reconnu comme un terminateur indiquant la fin des options. Tout ce qui suit est traité comme des arguments positionnels, même s'ils commencent par `-` ou `--`.

```bash
my_program --output=file.txt -- --file-with-dashes.txt
```

Dans cet exemple, `--file-with-dashes.txt` est traité comme un argument positionnel, pas comme une option.

## Collections multi-valeurs

argus prend en charge les options de collection qui peuvent contenir plusieurs valeurs (tableaux) ou paires clé-valeur (mappages).

### Formats d'options de tableau

Pour les options de tableau comme `OPTION_ARRAY_STRING`, `OPTION_ARRAY_INT`, etc. :

=== "Occurrences multiples d'options"
    ```bash
    # Chaque occurrence d'option ajoute au tableau
    ./program --names=john --names=alice --names=bob
    ```

=== "Listes séparées par des virgules"
    ```bash
    # Valeurs séparées par des virgules
    ./program --names=john,alice,bob
    ```

=== "Approches combinées"
    ```bash
    # Approches combinées
    ./program --names=john,alice --names=bob
    ```

=== "Format d'option courte"
    ```bash
    # Variantes d'options courtes
    ./program -n john -n alice -n bob
    ./program -njohn,alice,bob
    ```

!!! tip "Plages d'entiers"
    Les options de tableau d'entiers prennent en charge une syntaxe de plage pratique :
    
    ```bash
    ./program --ids=1-5,10,15-20
    # Équivalent à : --ids=1,2,3,4,5,10,15,16,17,18,19,20
    ```

### Formats d'options de mapping

Pour les options de mapping comme `OPTION_MAP_STRING`, `OPTION_MAP_INT`, etc. :

=== "Occurrences multiples d'options"
    ```bash
    ./program --env=USER=alice --env=HOME=/home/alice
    ```

=== "Paires clé-valeur séparées par des virgules"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice,TERM=xterm
    ```

=== "Approches combinées"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice --env=TERM=xterm
    ```

=== "Format d'option courte"
    ```bash
    ./program -e USER=alice -e HOME=/home/alice
    ./program -eUSER=alice,HOME=/home/alice
    ```

!!! tip "Mappings booléens"
    Pour les mappings booléens (`OPTION_MAP_BOOL`), les valeurs sont analysées comme des booléens :
    
    - **Valeurs vraies** : "true", "yes", "1", "on", "y" (insensible à la casse)
    - **Valeurs fausses** : "false", "no", "0", "off", "n" (insensible à la casse)
    
    ```bash
    ./program --features=debug=true,logging=yes,metrics=1,cache=false
    ```

## Arguments positionnels

Les arguments positionnels sont traités dans l'ordre où ils apparaissent sur la ligne de commande, après la correspondance de toutes les options :

```bash
my_program file1.txt file2.txt
```

Dans cet exemple, `file1.txt` et `file2.txt` sont des arguments positionnels.

### Arguments positionnels requis vs optionnels

argus distingue les arguments positionnels requis et optionnels :

```bash
my_program input.txt [output.txt]  # input.txt requis, output.txt optionnel
```

!!! warning
    Les arguments positionnels requis doivent toujours être définis avant les optionnels dans votre définition `ARGUS_OPTIONS`.

### Arguments positionnels avec options

Les arguments positionnels peuvent être utilisés avec des options dans n'importe quel ordre :

```bash
my_program --verbose input.txt --output=output.txt
my_program input.txt --verbose --output=output.txt
```

Les deux commandes sont équivalentes, car argus identifie les arguments positionnels après avoir fait correspondre toutes les options.

## Sous-commandes

argus prend en charge les sous-commandes de style Git/Docker, permettant des hiérarchies de commandes complexes :

### Format de base des sous-commandes

```bash
my_program [options globales] commande [options de commande] [arguments]
```

Exemples :
```bash
my_program --verbose add --force file.txt
my_program add --force file.txt --verbose  # Équivalent
```

### Sous-commandes imbriquées

argus prend également en charge les sous-commandes imbriquées pour des hiérarchies de commandes plus profondes :

```bash
my_program [options globales] commande sous-commande [options de sous-commande] [arguments]
```

Exemples :
```bash
my_program --debug service create --name myservice --image ubuntu
my_program config set server.port 8080
```

### Abréviations de commandes

argus prend en charge les abréviations de noms de commandes, permettant aux utilisateurs de taper des versions abrégées tant qu'elles sont sans ambiguïté :

```bash
# Ces commandes sont équivalentes si aucune autre commande ne commence par "i" :
my_program install package.tgz
my_program inst package.tgz
my_program i package.tgz

# Mais ambiguës s'il y a plusieurs correspondances
my_program i package.tgz  # ERREUR si "install" et "init" existent tous deux
```

## Mélange de différents formats

argus permet de mélanger différents styles de format dans une seule ligne de commande :

```bash
my_program -v --output=file.txt file1.txt file2.txt add --force extra.txt
```

Cette commande comprend :
- Un drapeau d'option courte (`-v`)
- Une option longue avec valeur (`--output=file.txt`)
- Des arguments positionnels (`file1.txt file2.txt`)
- Une sous-commande avec sa propre option (`add --force extra.txt`)

## Gestion des options ambiguës

Dans certains cas, l'entrée de ligne de commande peut être ambiguë. argus résout l'ambiguïté en utilisant ces règles :

1. Un argument commençant par un seul tiret (`-`) suivi d'un seul caractère est traité comme une option courte
2. Un argument commençant par des doubles tirets (`--`) est traité comme une option longue
3. Un argument après `--` est toujours traité comme un argument positionnel
4. Un argument ne commençant pas par un tiret est traité comme un argument positionnel ou une sous-commande (selon la définition)

Lors de l'analyse d'options courtes combinées (`-abc`) :
- Chaque caractère est traité comme une option booléenne séparée
- Si une option non booléenne est rencontrée, le reste de la chaîne est traité comme sa valeur

## Formes équivalentes en ligne de commande

Les formes de commande suivantes sont toutes équivalentes avec des définitions d'options appropriées :

| Format | Exemple |
|--------|---------|
| Options longues avec signe égal | `./my_program --verbose --output=file.txt input.txt` |
| Options longues avec espace | `./my_program --verbose --output file.txt input.txt` |
| Options courtes avec espace | `./my_program -v -o file.txt input.txt` |
| Options courtes sans espace | `./my_program -v -ofile.txt input.txt` |
| Options courtes combinées | `./my_program -vo file.txt input.txt` |
| Options courtes combinées avec valeur | `./my_program -vofile.txt input.txt` |
| Utilisation du terminateur `--` | `./my_program -v -- --input-with-dashes.txt` |
| Avec sous-commande | `./my_program -v add --force file.txt` |

## Notes importantes

!!! warning
    - Les options booléennes (drapeaux) ne nécessitent pas de valeur ; leur présence suffit à les activer.
    - Les options combinées (`-abc`) ne fonctionnent que pour les options booléennes.
    - Pour les options non booléennes, la dernière option d'une combinaison peut prendre une valeur.

!!! info "Exemples"
    - `-vo output.txt` (active l'option `-v` et définit `-o` à `output.txt`)
    - `-vooutput.txt` (active l'option `-v` et définit `-o` à `output.txt`)
    - `-vxooutput.txt` (active les options `-v` et `-x`, et définit `-o` à `output.txt`)

!!! note
    Les arguments positionnels sont toujours traités dans l'ordre défini dans `ARGUS_OPTIONS`.
