# Sysroot setup

## Using the board filesystem

### **Warning:** Developement packages must have been installed on the board.

The board filesystem can be mounted using `sshfs`.
```bash
sudo sshfs -o follow_symlinks -o transform_symlinks odroid@${board_address}:/ ${board_sysroot}
```

* `board_address` is the network address of the board.
* `board_sysroot` is the directory where the board sysroot will be mounted. Note that this directory must be exist and empty.

The sysroot is unmounted with `fusermount`.
```bash
sudo fusermount -u sysroot
```

## Using the OS image

> **Warning:**
> Developement packages must be available in the image. 
> This means that you may have to recreate the disk image (see below).

We must determine the offset of the root filesystem.

```bash
fdisk -l odroid-m1.img 
```

The result will be
```
Disque odroid-m1.img : 3,78 GiB, 4059717120 octets, 7929135 secteurs
Unités : secteur de 1 × 512 = 512 octets
Taille de secteur (logique / physique) : 512 octets / 512 octets
taille d'E/S (minimale / optimale) : 512 octets / 512 octets
Type d'étiquette de disque : dos
Identifiant de disque : 0x72015463

Périphérique           Amorçage  Début     Fin Secteurs Taille Id Type
odroid-m1.img1                    2048  526335   524288   256M 83 Linux
odroid-m1.img2                  526336 7929134  7402799   3,5G 83 Linux
```

The first partition is `/boot`. The second contains the rest of the filesystem. This is the one we want to mount.

The offsets given by `fdisk` are the sector number. As indicated, they must be multiplied by 512 in order to be converted into byte offset.

Here the offset is `526336 * 512 = 269484032`

```bash
sudo mount -o loop,offset=269484032 odroid-m1.img ${board_sysroot}
```

It is unmounted by calling
```bash
sudo mount ./sysroot
```

## Creating disk image

Plug the SD card. Unmount if it has been automatically mounter.

Run `fdisk -l` to identify the SD card disk.

Dump the SD Card using `dd`. Here we assume that the SD Card is `/dev/mmcblk0`.
```bash
sudo dd if=/dev/mmcblk0 of=odroid-m1.img bs=1M status=progress
sync
```

# Build

Download and install the toolchain.
 * [Linaro GCC 7.4-2019.02](https://releases.linaro.org/components/toolchain/binaries/7.4-2019.02/aarch64-linux-gnu/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu.tar.xz)

We consider here that the sysroot is installed in `/opt/odroid/sysroot`and the toolchain in `/opt/odroid/toolchain`.

Create a build directory and launch `CMake` configuration.
 ```bash
 cmake ../.. \ 
   -DCMAKE_TOOLCHAIN_FILE:FILEPATH=../cmake/armv8-a-toolchainfile.cmake \
   -DSYSROOT=/opt/odroid/sysroot \
   -DTOOLCHAIN=/opt/odroid/toolchain
```

`hg-core` can now be built with:
```bash
cmake --build .
```

