# Ocean-Modifier

Splatoon 2 Salmon Run seed modifier

## Requirements

- DevkitA64
- Unix like operating system(Ubuntu, macOS)
  - Recommend WSL2 on Window10/11

### Get Started for macOC

```sh
$ wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman-installer.pkg
$ sudo installer -pkg devkitpro-pacman-installer.pkg -target /
```

#### Install DevkitA64

```sh
$ sudo dkp-pacman -Sy
$ sudo dkp-pacman -Syu
$ sudo dkp-pacman -S switch-dev
```

#### Edit environment variables

`vi ~/.zshrc`

```sh
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
export DEVKITPPC=/opt/devkitpro/devkitPPC
```

For applying above settings without relaunch the console, input following command and press enter.

`source ~/.zshrc`

#### Clone the project

```sh
$ git clone https://github.com/container1234/Ocean-Modifier
$ cd Ocean-Modifier
$ git submodule update --init --recursive
```

#### Build

```sh
$ make
main.cpp
dmntcht.c
linking Ocean-Modifier.elf
built ... Ocean-Modifier.nacp
built ... Ocean-Modifier.ovl
```
