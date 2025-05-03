# Guide d'Installation

Ce guide explique comment installer la bibliothèque argus dans différents environnements.

## Référence Rapide

| Méthode | Commande | Idéal Pour |
|--------|---------|----------|
| **Gestionnaires de Paquets** | `conan install argus/1.0.0@` | Utilisation en production |
| **Depuis les Sources** | `meson setup .build && meson compile -C .build` | Développement |
| **Avec Just** | `just build` | Flux de développement |

## Prérequis

argus n'a qu'une seule dépendance optionnelle :

!!! info "Dépendance PCRE2"
    **PCRE2** : Requis uniquement pour la validation par expressions régulières.
    
    === "Ubuntu/Debian"
        ```bash
        apt install libpcre2-dev
        ```
    
    === "Fedora/CentOS"
        ```bash
        dnf install pcre2-devel
        ```
    
    === "macOS"
        ```bash
        brew install pcre2
        ```

## Méthodes d'Installation

### Gestionnaires de Paquets (Recommandé)

#### Conan

```bash
# Installation depuis Conan Center
conan install argus/1.0.0@

# Avec des options spécifiques
conan install argus/1.0.0@ -o argus:disable_regex=true
```

Dans votre fichier `conanfile.txt` :
```
[requires]
argus/1.0.0

[options]
argus:disable_regex=False
```

#### vcpkg

```bash
# Installation depuis le registre vcpkg
vcpkg install argus

# Sans support des regex
vcpkg install argus[core]
```

Dans votre fichier `vcpkg.json` :
```json
{
  "dependencies": [
    {
      "name": "argus",
      "features": ["regex"]
    }
  ]
}
```

### Compilation Depuis les Sources

#### Utilisation de Meson

```bash
# Cloner le dépôt
git clone https://github.com/lucocozz/argus.git
cd argus

# Compiler
meson setup .build
meson compile -C .build

# Installer (nécessite des permissions)
meson install -C .build
```

#### Utilisation de Just (Flux de Développement)

!!! tip "Just"
    [Just](https://github.com/casey/just) est un exécuteur de commandes qui simplifie les tâches courantes.

```bash
# Cloner le dépôt
git clone https://github.com/lucocozz/argus.git
cd argus

# Compiler les bibliothèques statiques et partagées
just build

# Installer
just install
```

### Utilisation comme Bibliothèque Statique

Si vous préférez ne pas installer au niveau système :

1. Compilez le projet en utilisant n'importe quelle méthode ci-dessus
2. Copiez `argus.a` dans votre projet
3. Copiez le répertoire `includes/` dans votre projet
4. Liez avec la bibliothèque statique :

```bash
gcc votre_programme.c -o votre_programme -L/chemin/vers/argus.a -largus
```

### Comme Dépendance Meson

```meson
# Dans votre meson.build
argus_dep = dependency('argus', version: '>=1.0.0', required: false)

# Repli sur un sous-projet si non trouvé au niveau système
if not argus_dep.found()
  argus_proj = subproject('argus')
  argus_dep = argus_proj.get_variable('argus_dep')
endif
```

## Options de Configuration

### Désactiver le Support des Regex

Si vous n'avez pas besoin de validation par regex, vous pouvez compiler sans la dépendance PCRE2 :

=== "Meson"
    ```bash
    meson setup -Ddisable_regex=true .build
    ```

=== "Just"
    ```bash
    just disable_regex=true build
    ```

=== "Conan"
    ```bash
    conan install . -o argus:disable_regex=true
    ```

=== "vcpkg"
    ```bash
    vcpkg install argus --features=""
    ```

Quand le support des regex est désactivé :
- Aucune dépendance PCRE2 n'est requise
- Le validateur `REGEX()` devient une fonction sans effet
- Tous les motifs prédéfinis dans `argus/regex.h` sont définis mais ne fonctionneront pas
- La macro `ARGUS_NO_REGEX` est définie pour la compilation conditionnelle

### Optimisation des Performances

Pour les déploiements en production, activez le mode release pour ignorer la validation lors de l'initialisation :

=== "Compilation Manuelle"
    ```bash
    gcc votre_programme.c -o votre_programme -DARGUS_RELEASE -largus
    ```

=== "Meson"
    ```meson
    add_project_arguments('-DARGUS_RELEASE', language: 'c')
    ```

## Vérification de l'Installation

=== "Vérifier les Fichiers"
    ```bash
    # Vérifier la bibliothèque partagée
    ls -la /usr/local/lib/argus.so*
    
    # Vérifier les en-têtes
    ls -la /usr/local/include/argus*
    ```

=== "Test de Compilation"
    ```bash
    # Compiler un programme exemple
    gcc -o test_argus test.c -largus
    
    # Exécuter le programme
    ./test_argus --help
    ```
