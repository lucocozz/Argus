#!/bin/bash
set -e

VERSION="1.0.0"

PREFIX="/usr/local"
LIBDIR="${PREFIX}/lib"
INCLUDEDIR="${PREFIX}/include"
INSTALL_LOCAL=0
VERBOSE=0
REGEX=1
UNINSTALL=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

show_help() {
    echo -e "${BLUE}argus installer${NC} - v${VERSION}"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --prefix=PATH      Installation prefix (default: /usr/local)"
    echo "  --libdir=PATH      Library directory (default: \$prefix/lib)"
    echo "  --includedir=PATH  Include directory (default: \$prefix/include)"
    echo "  --local            Install to ~/.local instead of system directories"
    echo "  --no-regex    Disable regex support"
    echo "  --uninstall        Uninstall the library"
    echo "  --verbose          Show detailed output"
    echo "  --help             Display this help message"
    echo ""
    echo "Example: $0 --prefix=/opt --verbose"
}

for arg in "$@"; do
    case $arg in
        --no-regex)
            REGEX=0
            ;;
        --prefix=*)
            PREFIX="${arg#*=}"
            ;;
        --libdir=*)
            LIBDIR="${arg#*=}"
            ;;
        --includedir=*)
            INCLUDEDIR="${arg#*=}"
            ;;
        --local)
            INSTALL_LOCAL=1
            PREFIX="$HOME/.local"
            LIBDIR="$HOME/.local/lib"
            INCLUDEDIR="$HOME/.local/include"
            ;;
        --verbose)
            VERBOSE=1
            ;;
        --uninstall)
            UNINSTALL=1
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            echo -e "${RED}Error:${NC} Unknown option: $arg"
            show_help
            exit 1
            ;;
    esac
done

log() {
    if [ $VERBOSE -eq 1 ]; then
        echo -e "$1"
    fi
}

check_dependencies() {
    log "${BLUE}Checking dependencies...${NC}"

    # Skip PCRE2 check if regex is disabled
    if [ "$REGEX" -ne 0 ]; then    
        if ! pkg-config --exists libpcre2-8; then
            echo -e "${RED}Error:${NC} pcre2 library not found"
            echo "Please install pcre2 development files:"
            echo "  - Ubuntu/Debian: sudo apt install libpcre2-dev"
            echo "  - Fedora/RHEL:   sudo dnf install pcre2-devel"
            echo "  - macOS:         brew install pcre2"
            exit 1
        fi
    fi
    
    log "${GREEN}All dependencies satisfied.${NC}"
}

check_permissions() {
    if [ $INSTALL_LOCAL -eq 0 ]; then
        if [ "$(id -u)" -ne 0 ] && [ -w "$PREFIX" ] ; then
            echo -e "${YELLOW}Warning:${NC} Installing system-wide without root privileges."
            echo "If installation fails, try with sudo or use --local"
        elif [ "$(id -u)" -ne 0 ]; then
            echo -e "${RED}Error:${NC} Installing system-wide requires root privileges."
            echo "Either run with sudo or use --local option to install in your home directory."
            exit 1
        fi
    fi
}

install_library() {
    log "${BLUE}Installing argus to:${NC}"
    log "  Library: $LIBDIR"
    log "  Headers: $INCLUDEDIR"
    
    mkdir -p "$LIBDIR" "$INCLUDEDIR"
    
    log "${BLUE}Copying files...${NC}"
    cp -r lib/* "$LIBDIR/"
    cp -r include/* "$INCLUDEDIR/"
    
    if [ "$(id -u)" -eq 0 ] && [ "$(uname)" = "Linux" ]; then
        log "${BLUE}Updating library cache...${NC}"
        ldconfig
    fi
    
    if [ -d "$LIBDIR/pkgconfig" ] || mkdir -p "$LIBDIR/pkgconfig"; then
        log "${BLUE}Creating pkg-config file...${NC}"
        cat > "$LIBDIR/pkgconfig/argus.pc" << EOF
prefix=$PREFIX
exec_prefix=\${prefix}
libdir=$LIBDIR
includedir=$INCLUDEDIR

Name: argus
Description: Modern C library for command-line argument parsing
Version: $VERSION
Requires.private: libpcre2-8
Libs: -L\${libdir} -largus
Cflags: -I\${includedir}
EOF
    fi
    
    echo -e "${GREEN}argus has been successfully installed!${NC}"
    
    echo ""
    echo "You can now use argus in your projects:"
    echo ""
    if [ $INSTALL_LOCAL -eq 1 ]; then
        echo "  With pkg-config: gcc -o myapp myapp.c \$(pkg-config --cflags --libs argus)"
        echo "  Or directly:     gcc -o myapp myapp.c -I$INCLUDEDIR -L$LIBDIR -largus"
    else
        echo "  Direct linking:  gcc -o myapp myapp.c -largus"
        echo "  Or with pkg-config: gcc -o myapp myapp.c \$(pkg-config --cflags --libs argus)"
    fi
    
    if [ $INSTALL_LOCAL -eq 1 ]; then
        echo ""
        echo "Since you installed in a non-standard location, you might need to:"
        echo "  - Add $LIBDIR to LD_LIBRARY_PATH"
        echo "  - Add $LIBDIR/pkgconfig to PKG_CONFIG_PATH"
        echo ""
        echo "For example, add these lines to your ~/.bashrc:"
        echo "  export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$LIBDIR"
        echo "  export PKG_CONFIG_PATH=\$PKG_CONFIG_PATH:$LIBDIR/pkgconfig"
    fi
}

uninstall_library() {
    log "${BLUE}Uninstalling argus from:${NC}"
    log "  Library: $LIBDIR"
    log "  Headers: $INCLUDEDIR"
    
    rm -f "$LIBDIR/argus.a" "$LIBDIR/argus.so"*
    rm -f "$LIBDIR/pkgconfig/argus.pc"
    rm -rf "$INCLUDEDIR/argus"
    rm -f "$INCLUDEDIR/argus.h"
    
    if [ "$(id -u)" -eq 0 ] && [ "$(uname)" = "Linux" ]; then
        log "${BLUE}Updating library cache...${NC}"
        ldconfig
    fi
    
    echo -e "${GREEN}argus has been successfully uninstalled!${NC}"
}

if [ $UNINSTALL -eq 1 ]; then
    check_permissions
    uninstall_library
else
    check_dependencies
    check_permissions
    install_library
fi

exit 0
