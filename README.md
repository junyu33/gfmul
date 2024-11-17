# gfmul

This is a simple implementation of the Galois Field multiplication in c++.

This code is cross-ISA, including x86-64, ARM64 and riscv64.

The irriducible polynomial is $x^{128} + x^7 + x^2 + x + 1$.

## dependencies (in typical x86-64 machine)

> To run the control group, you also need to install NTL and gmp (instruction for debian based).
>
> ```bash
> sudo apt-get install libgmp-dev
> wget https://libntl.org/ntl-11.5.1.tar.gz
> cd ntl-11.5.1/src
> ./configure 
> make
> sudo make install
> ```


### debian based

```bash
sudo apt update
sudo apt install build-essential
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
sudo apt install qemu-system-arm qemu-efi-aarch64 qemu-user-static
sudo apt install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
sudo apt install qemu-system-misc 
```

### arch based

```bash
sudo pacman -Syu
sudo pacman -S base-devel
sudo pacman -S aarch64-linux-gnu-gcc aarch64-linux-gnu-g++ 
sudo pacman -S qemu-arch-extra qemu-user-static
sudo pacman -S riscv64-linux-gnu-gcc
```

## usage

in x86-64:

```bash
make run ARCH=x86_64
```

in ARM64:

```bash
make run ARCH=arm64 RUN=qemu
```

in riscv64:

```bash
make run ARCH=riscv64 RUN=qemu
```

in x86-64 using NTL:

```bash
make run ARCH=x86_64 USE_NTL=1
```
