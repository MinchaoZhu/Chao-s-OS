# Chao-s-OS

A simple OS kernel in GPLv3 license

## Build

- clone the whole repositery
- build the iso image
    ```
    $ chomd +x clean.sh
    $ chomd +x headers.sh
    $ chomd +x iso.sh
    $ ./clean.sh
    $ ./headers.sh
    $ ./iso.sh
    ```
- run chao's os
    ```
    $ ./qemu.sh
    ```
## File Organization
```kernel``` is folder for kernel part
    ```arch``` is for target architechture distribution
    ```include``` is for kernel part headers
    ```kernel``` is kernel source file

```libc``` is for c runtime lib
