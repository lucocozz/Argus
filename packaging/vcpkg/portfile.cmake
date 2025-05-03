vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO lucocozz/argus
    REF v${VERSION}
    SHA512 74bb7eebb1f328d3b27c51c2cec74f87b0a11fd7c801bb3d2d4b56b8ed7448461043f8f01b68f69cee67c2c73217ebb8b641a5e76a632052910ddb13b750045f
    HEAD_REF main
)

set(OPTIONS "")
if(NOT "regex" IN_LIST FEATURES)
    list(APPEND OPTIONS -Ddisable_regex=true)
endif()
vcpkg_configure_meson(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${OPTIONS}
        -Dbenchmarks=false
        -Dexamples=false
        -Dtests=false
)

vcpkg_install_meson()
vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
