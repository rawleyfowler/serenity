#!/usr/bin/env -S bash ../.port_include.sh
port='Emacs'
version='29.1'
useconfigure='true'
depends=(
    "ncurses"
)
CC="/usr/bin/gcc"
files=(
    "https://mirrors.ocf.berkeley.edu/gnu/emacs/emacs-${version}.tar.xz#d2f881a5cc231e2f5a03e86f4584b0438f83edd7598a09d24a21bd8d003e2e01"
)
workdir="emacs-${version}"

post_fetch() {
    run sh -c "cd ${PWD}/emacs-${version} && make clean"
    run chmod -R +rw "${PWD}/emacs-${version}"
}

configure() {
    configopts=(
        "--with-xpm=no"
        "--with-jpeg=no"
        "--with-png=no"
        "--with-gif=no"
        "--with-tiff=no"
        "--with-x-toolkit=no"
        "--with-gnutls=ifavailable"
        "--without-x"
        "--host=x86_64-unknown-linux-gnu"
        "--build=$(cc -dumpmachine)"
    )
    run ./configure ${configopts[@]/#/}
}

build() {
    run sh -c "cd ${PWD}/emacs-${version} && gmake"
}
