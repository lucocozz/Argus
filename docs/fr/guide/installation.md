# Guide d'Installation

Ce guide explique comment installer la bibliothèque argus sur différents systèmes d'exploitation.

!!! warning "Travail en cours"
    - Seule la méthode d'installation manuelle est disponible pour le moment. Les méthodes via gestionnaires de paquets sont réservées pour les futures versions.
    - Le support Windows est limité à gcc version 13.0.0 ou plus récente.

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

**PCRE2** : Requis uniquement pour la prise en charge de la validation par expressions régulières.

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

[Conan](https://conan.io/) fournit une gestion des paquets multi-plateformes pour les bibliothèques C/C++.

```bash
# Installer la dernière version
conan install argus/1.0.0@

# Désactiver le support des regex
conan install argus/1.0.0@ -o argus:regex=false
```

Ajoutez à votre `conanfile.txt` de projet :
```ini
[requires]
argus/1.0.0

[options]
argus:regex=True  # Mettre à False pour désactiver les regex
```

#### vcpkg

[vcpkg](https://github.com/microsoft/vcpkg) est le gestionnaire de bibliothèques C++ de Microsoft.

```bash
# Installer avec le support des regex
vcpkg install argus

# Installer sans le support des regex
vcpkg install argus[core]
```

Ajoutez à votre `vcpkg.json` de projet :
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
Réservé pour les futures versions
-->

<!--
#### Homebrew (macOS)
Réservé pour les futures versions
-->

<!--
#### Chocolatey (Windows)
Réservé pour les futures versions
-->

### Compilation à partir des Sources

#### Étape 1 : Installer les Outils de Compilation

=== "Windows"
    Installez les outils suivants :
    
    - **Visual Studio** ou **MinGW avec GCC 13.0.0+**
    - **Python** 3.7+
    - **Meson** et le système de compilation **Ninja**
    
    ```bash
    # Installer Python et Meson
    pip install meson ninja
    
    # Pour le support des regex (optionnel)
    vcpkg install pcre2:x64-windows
    ```
    
    !!! warning "Compatibilité Windows"
        La bibliothèque argus est uniquement compatible avec GCC version 13.0.0 ou plus récente sur les plateformes Windows.

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
    # Installer les outils de ligne de commande
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
# Désactiver le support des regex
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
    
    **Pour les projets MinGW/GCC :**
    - Nécessite GCC 13.0.0 ou plus récent
    - Ajoutez le chemin d'inclusion à votre projet
    - Liez avec libargus.a

=== "Linux/macOS"
    1. Téléchargez `argus-1.0.0.tar.gz`
    2. Extrayez : `tar -xzf argus-1.0.0.tar.gz`
    3. Exécutez l'installateur : `cd argus-1.0.0 && ./install.sh`

### Script d'Installation

Pour Linux et macOS, le script d'installation fournit une méthode d'installation simple :

```bash
# Installation système (par défaut)
./install.sh

# Installation dans le répertoire utilisateur (~/.local)
./install.sh --local

# Installation sans support des regex
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
| `--no-regex` | Désactiver le support des regex |
| `--uninstall` | Désinstaller la bibliothèque |
| `--verbose` | Afficher des informations détaillées |

## Intégration dans un Projet

### Flags du Compilateur

```bash
# Compilation directe
gcc votre_programme.c -o votre_programme -largus

# Avec pkg-config
gcc votre_programme.c -o votre_programme $(pkg-config --cflags --libs argus)
```

### Intégration CMake

```cmake
# Trouver argus installé
find_package(argus REQUIRED)
target_link_libraries(votre_cible PRIVATE argus::argus)
```

### Intégration Meson

```meson
# Dans votre meson.build
argus_dep = dependency('argus', version: '>=1.0.0', required: false)

# Solution de secours via sous-projet si non trouvé
if not argus_dep.found()
  argus_proj = subproject('argus')
  argus_dep = argus_proj.get_variable('argus_dep')
endif

executable('monapplication', 'main.c', dependencies: [argus_dep])
```

## Notes Spécifiques aux Plateformes

### Windows-Spécifique

=== "MSVC"
    ```powershell
    # Ouvrir l'Invite de Commande pour VS
    
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
    
    !!! important "Exigence de Version GCC"
        Pour Windows avec MinGW, vous devez utiliser GCC version 13.0.0 ou plus récente. Les versions plus anciennes ne sont pas compatibles avec la bibliothèque argus.
        
        Vérifiez votre version de GCC avec : `gcc --version`

### Linux-Spécifique

Si vous avez installé dans un emplacement non standard ou dans votre répertoire utilisateur, vous devrez peut-être mettre à jour votre environnement :

```bash
# Ajouter à votre .bashrc ou .zshrc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/.local/lib/pkgconfig
```

### macOS-Spécifique

Si vous avez installé dans un emplacement non standard :

```bash
# Ajouter à votre .bashrc ou .zshrc
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:~/.local/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/.local/lib/pkgconfig
```

## Résolution de Problèmes

### Problèmes Courants

| Problème | Solution |
|----------|----------|
| **Bibliothèque non trouvée lors de l'édition des liens** | Ajouter le chemin de la bibliothèque : `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib` |
| **En-têtes non trouvés** | Ajouter le chemin d'inclusion : `-I/chemin/vers/argus/include` |
| **PCRE2 non trouvé** | Installer PCRE2 ou désactiver les regex avec `-Dregex=false` |
| **DLL Windows non trouvée à l'exécution** | Ajouter le répertoire DLL au PATH ou copier la DLL dans le répertoire de l'exécutable |
| **Erreurs "undefined reference" sur Windows** | Assurez-vous d'utiliser GCC 13.0.0 ou plus récent |

### Version de GCC sur Windows

Sur Windows, argus nécessite GCC 13.0.0 ou plus récent. Si vous utilisez une version plus ancienne, vous devrez la mettre à jour :

```bash
# Vérifier votre version de GCC
gcc --version

# Si vous utilisez MSYS2/MinGW, mettre à jour vers la dernière version
pacman -Syu mingw-w64-x86_64-gcc
```

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

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    if (status == ARGUS_SUCCESS)
        printf("argus fonctionne correctement !\n");

    argus_free(&argus);
    return 0;
}
```

Compilez et exécutez :

```bash
gcc test.c -o test -largus
./test --help
```

## Étapes Suivantes

Une fois installé, passez au [Guide de Démarrage Rapide](quickstart.md) pour commencer à utiliser argus dans vos projets.
