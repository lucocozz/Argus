# Guide d'Installation

Ce guide explique comment installer la bibliothèque argus sur différents systèmes d'exploitation.

!!! warning "Travaux en cours"
    - Seule la méthode d'installation manuelle est disponible pour le moment. Les méthodes par gestionnaire de paquets sont réservées pour les versions futures.
    - La prise en charge de Windows n'est pas encore disponible.

## Méthodes d'Installation Rapide

Choisissez la méthode qui convient le mieux à votre environnement :

<div class="grid cards" markdown>

-   :material-package-down:{ .lg .middle } **Gestionnaires de Paquets**

    ---
    
    === "Windows"
        ```bash
        # Utilisation de vcpkg
        vcpkg install argus
        ```
    
    === "Linux"
        ```bash
        # Utilisation de vcpkg
        vcpkg install argus

        # Utilisation de conan
        conan install argus/1.0.0
        ```
    
    === "macOS"
        ```bash
        # Utilisation de vcpkg
        vcpkg install argus
        ```

-   :material-code-tags:{ .lg .middle } **À partir des Sources**

    ---
    
    ```bash
    # Cloner le dépôt
    git clone https://github.com/lucocozz/argus.git
    cd argus
    
    # Compiler et installer
    meson setup .build
    meson compile -C .build
    sudo meson install -C .build
    ```

-   :material-script:{ .lg .middle } **Script d'Installation**

    ---
    
    ```bash
    # Télécharger et extraire
    curl -LO https://github.com/lucocozz/argus/releases/download/v1.0.0/argus-1.0.0.tar.gz
    tar -xzf argus-1.0.0.tar.gz
    
    # Exécuter l'installateur
    cd argus-1.0.0
    ./install.sh
    ```

-   :material-download:{ .lg .middle } **Binaires Précompilés**

    ---
    
    1. Télécharger depuis [GitHub Releases](https://github.com/lucocozz/argus/releases)
    2. Extraire le paquet
    3. Utiliser directement les bibliothèques et les en-têtes

</div>

## Dépendances

argus n'a qu'une seule dépendance optionnelle :

**PCRE2** : Nécessaire uniquement pour la prise en charge de la validation par expressions régulières.

=== "Windows"
    ```bash
    # Utilisation de vcpkg
    vcpkg install pcre2:x64-windows
    ```

=== "Linux"
    ```bash
    # Ubuntu/Debian
    sudo apt install libpcre2-dev
    
    # Fedora/RHEL
    sudo dnf install pcre2-devel
    
    # Arch Linux
    sudo pacman -S pcre2
    ```

=== "macOS"
    ```bash
    # Utilisation de Homebrew
    brew install pcre2
    ```

## Méthodes d'Installation Détaillées

### Gestionnaires de Paquets

#### Conan

[Conan](https://conan.io/) fournit une gestion de paquets multiplateforme pour les bibliothèques C/C++.

```bash
# Installer la dernière version
conan install argus/1.0.0@

# Désactiver la prise en charge des expressions régulières
conan install argus/1.0.0@ -o argus:regex=false
```

Ajoutez à votre fichier `conanfile.txt` du projet :
```ini
[requires]
argus/1.0.0

[options]
argus:regex=True  # Mettre à False pour désactiver les expressions régulières
```

#### vcpkg

[vcpkg](https://github.com/microsoft/vcpkg) est le gestionnaire de bibliothèques C++ de Microsoft.

```bash
# Installer avec prise en charge des expressions régulières
vcpkg install argus

# Installer sans prise en charge des expressions régulières
vcpkg install argus[core]
```

Ajoutez à votre fichier `vcpkg.json` du projet :
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

<!--
#### APT (Debian/Ubuntu)
Réservé pour les versions futures
-->

<!--
#### Homebrew (macOS)
Réservé pour les versions futures
-->

<!--
#### Chocolatey (Windows)
Réservé pour les versions futures
-->

### Compilation depuis les Sources

#### Étape 1 : Installer les Outils de Compilation

=== "Windows"
    Installez les outils suivants :
    
    - **Visual Studio** ou **MinGW**
    - **Python** 3.7+
    - Système de compilation **Meson** et **Ninja**
    
    ```bash
    # Installer Python et Meson
    pip install meson ninja
    
    # Pour la prise en charge des expressions régulières (optionnel)
    vcpkg install pcre2:x64-windows
    ```

=== "Linux"
    ```bash
    # Ubuntu/Debian
    sudo apt install build-essential meson ninja-build pkg-config
    
    # Fedora/RHEL
    sudo dnf install gcc meson ninja-build pkg-config
    
    # Arch Linux
    sudo pacman -S base-devel meson ninja
    ```

=== "macOS"
    ```bash
    # Installer les outils en ligne de commande
    xcode-select --install
    
    # Installer les outils de compilation
    brew install meson ninja
    ```

#### Étape 2 : Cloner le Dépôt

```bash
git clone https://github.com/lucocozz/argus.git
cd argus
```

#### Étape 3 : Configurer et Compiler

```bash
# Configurer la compilation
meson setup .build

# Compiler la bibliothèque
meson compile -C .build
```

Options de configuration :

```bash
# Désactiver la prise en charge des expressions régulières
meson setup .build -Dregex=false

# Compilation en mode release
meson setup .build --buildtype=release

# Activer les exemples
meson setup .build -Dexamples=true

# Activer les tests
meson setup .build -Dtests=true
```

#### Étape 4 : Installer

=== "Windows (Invite de Commandes Administrateur)"
    ```bash
    meson install -C .build
    ```

=== "Linux/macOS"
    ```bash
    sudo meson install -C .build
    ```

### Utilisation de Just (Développement)

Si vous avez [Just](https://github.com/casey/just) installé, vous pouvez utiliser ces commandes pratiques :

```bash
# Compiler les bibliothèques
just build

# Exécuter les tests
just test

# Compiler et installer
just build install

# Compiler avec des options spécifiques
just regex=false build
```

### Binaires Précompilés

Téléchargez les paquets précompilés depuis [GitHub Releases](https://github.com/lucocozz/argus/releases).

=== "Windows"
    1. Téléchargez `argus-1.0.0-windows.zip`
    2. Extrayez à n'importe quel emplacement
    3. Ajoutez le répertoire `bin` au PATH ou utilisez directement les bibliothèques
    
    **Pour les projets MSVC :**
    - Ajoutez le chemin d'inclusion à votre projet
    - Liez avec argus.lib

=== "Linux/macOS"
    1. Téléchargez `argus-1.0.0.tar.gz`
    2. Extrayez : `tar -xzf argus-1.0.0.tar.gz`
    3. Exécutez l'installateur : `cd argus-1.0.0 && ./install.sh`

### Script d'Installation

Pour Linux et macOS, le script d'installation fournit une méthode d'installation simple :

```bash
# Installation système (par défaut)
./install.sh

# Installer dans le répertoire utilisateur (~/.local)
./install.sh --local

# Installer sans prise en charge des expressions régulières
./install.sh --no-regex

# Spécifier un répertoire d'installation personnalisé
./install.sh --prefix=/opt/argus
```

Options disponibles :

| Option | Description |
|--------|-------------|
| `--prefix=CHEMIN` | Préfixe d'installation (par défaut : `/usr/local`) |
| `--libdir=CHEMIN` | Répertoire des bibliothèques (par défaut : `$prefix/lib`) |
| `--includedir=CHEMIN` | Répertoire des en-têtes (par défaut : `$prefix/include`) |
| `--local` | Installer dans `~/.local` au lieu des répertoires système |
| `--no-regex` | Désactiver la prise en charge des expressions régulières |
| `--uninstall` | Désinstaller la bibliothèque |
| `--verbose` | Afficher une sortie détaillée |

## Intégration au Projet

### Options du Compilateur

```bash
# Compilation directe
gcc your_program.c -o your_program -largus

# Avec pkg-config
gcc your_program.c -o your_program $(pkg-config --cflags --libs argus)
```

### Intégration CMake

```cmake
# Trouver argus installé
find_package(argus REQUIRED)
target_link_libraries(your_target PRIVATE argus::argus)
```

### Intégration Meson

```meson
# Dans votre meson.build
argus_dep = dependency('argus', version: '>=1.0.0', required: false)

# Repli sur un sous-projet si non trouvé
if not argus_dep.found()
  argus_proj = subproject('argus')
  argus_dep = argus_proj.get_variable('argus_dep')
endif

executable('myapp', 'main.c', dependencies: [argus_dep])
```

## Notes Spécifiques aux Plateformes

### Spécifique à Windows

=== "MSVC"
    ```powershell
    # Ouvrir l'Invite de Commandes Développeur pour VS
    
    # Configurer et compiler
    meson setup .build
    meson compile -C .build
    
    # Avec PCRE2 de vcpkg
    meson setup .build -Dpcre2_include_dir=C:/vcpkg/installed/x64-windows/include -Dpcre2_lib_dir=C:/vcpkg/installed/x64-windows/lib
    ```

=== "MinGW"
    ```bash
    # Configurer et compiler
    meson setup .build
    meson compile -C .build
    ```

### Spécifique à Linux

Si vous avez installé dans un emplacement non standard ou dans votre répertoire utilisateur, vous devrez peut-être mettre à jour votre environnement :

```bash
# Ajouter à votre .bashrc ou .zshrc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/.local/lib/pkgconfig
```

### Spécifique à macOS

Si vous avez installé dans un emplacement non standard :

```bash
# Ajouter à votre .bashrc ou .zshrc
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:~/.local/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/.local/lib/pkgconfig
```

## Dépannage

### Problèmes Courants

| Problème | Solution |
|-------|----------|
| **Bibliothèque introuvable lors de l'édition de liens** | Ajouter le chemin de la bibliothèque : `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib` |
| **En-têtes introuvables** | Ajouter le chemin d'inclusion : `-I/chemin/vers/argus/include` |
| **PCRE2 introuvable** | Installer PCRE2 ou désactiver les expressions régulières avec `-Dregex=false` |
| **DLL Windows introuvable à l'exécution** | Ajouter le répertoire DLL au PATH ou copier la DLL dans le répertoire de l'exécutable |

### PCRE2 sur Windows

Sur Windows, la détection de PCRE2 peut être délicate. Spécifiez explicitement les répertoires :

```bash
meson setup .build -Dpcre2_include_dir=C:/chemin/vers/pcre2/include -Dpcre2_lib_dir=C:/chemin/vers/pcre2/lib
```

## Vérification de l'Installation

Créez un programme de test simple (`test.c`) :

```c
#include <argus.h>
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION()
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    if (status == ARGUS_SUCCESS) {
        printf("argus fonctionne correctement !\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

Compilez et exécutez :

```bash
gcc test.c -o test -largus
./test --help
```

## Prochaines Étapes

Une fois installé, passez au [Guide de Démarrage Rapide](quickstart.md) pour commencer à utiliser argus dans vos projets.
