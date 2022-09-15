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

## Local board filesystem backup

The board rootfs can be downloaded using `rsync`
```bash
rsync -avAHXe --copy-unsafe-links ssh --numeric-ids --stats \
    --exclude /bin \
    --exclude /boot \
    --exclude /etc \
    --exclude /dev \
    --exclude /home \
    --exclude /lost+found \
    --exclude /media/ \
    --exclude /mnt/ \
    --exclude /proc \
    --exclude /root \
    --exclude /run \
    --exclude /sys \
    --exclude /swapfile1 \
    --exclude /tmp \
    --exclude /usr/bin \
    --exclude /usr/libexec \
    --exclude /usr/local/bin \
    --exclude /usr/local/etc \
    --exclude /usr/local/games \
    --exclude /usr/local/man \
    --exclude /usr/local/share \
    --exclude /usr/local/src \
    --exclude /usr/local/sbin \
    --exclude /usr/games \
    --exclude /usr/sbin \
    --exclude /usr/share \
    --exclude /usr/src \
    --exclude /var \
    odroid@${board_address}:/ \
    ${board_sysroot}
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
sudo dd if=/dev/mmcblk0 of=odroid-m1.img conv=notrunc status=progress
sync
```

Here is the procedure to shrink the disk image.

Request a new loopback device.
```bash
sudo losetup -f
```
Upon success, `losetup` prints the name of the newly created loopback device on the standar output.

Setup the loop device.
```bash
sudo losetup /dev/loop14 odroid-m1.img
```

Ask the kernel to load the partition table.
```bash
sudo partprobe /dev/loop14
```

The partitions can now be resized with `gparted` for example.

Detach the loopback device.
```bash
sudo losetup -d /dev/loop14
```

Use `fdisk` to find where the partition ends.

```bash
fdisk -l odroid-m1.img
```
The last line of the output gives the last block of the partition (here `11073535`).
```
odroid-m1.img2           526336 11073535 10547200     5G 83 Linux
```

The image is then truncated to `(last block + 1) * block size`. Here the block is `512`. The new image size is then `(11073535+1) * 512 = 5669650432`.
```bash
truncate --size=5669650432 odroid-m1.img
```

Fix symbolic links.

Fix some symlinks in the `sysroot` directory.
```bash
# mount the image to sysroot directory
cd sysroot/usr/lib/aarch64-linux-gnu/
ln -fs ../../../lib/aarch64-linux-gnu/librt.so.1 librt.so
ln -fs ../../../lib/aarch64-linux-gnu/libanl.so.1 libanl.so
ln -fs ../../../lib/aarch64-linux-gnu/libBrokenLocale.so.1 libBrokenLocale.so
ln -fs ../../../lib/aarch64-linux-gnu/libcrypt.so.1.1.0 libcrypt.so
ln -fs ../../../lib/aarch64-linux-gnu/libdl.so.2 libdl.so
ln -fs ../../../lib/aarch64-linux-gnu/libexpat.so.1.6.11 libexpat.so
ln -fs ../../../lib/aarch64-linux-gnu/libm.so.6 libm.so
ln -fs ../../../lib/aarch64-linux-gnu/libnsl.so.1 libnsl.so
ln -fs ../../../lib/aarch64-linux-gnu/libnss_compat.so.2 libnss_compat.so
ln -fs ../../../lib/aarch64-linux-gnu/libnss_dns.so.2 libnss_dns.so
ln -fs ../../../lib/aarch64-linux-gnu/libnss_files.so.2 libnss_files.so
ln -fs ../../../lib/aarch64-linux-gnu/libnss_hesiod.so.2 libnss_hesiod.so
ln -fs ../../../lib/aarch64-linux-gnu/libnss_nisplus.so.2 libnss_nisplus.so
ln -fs ../../../lib/aarch64-linux-gnu/libnss_nis.so.2 libnss_nis.so
ln -fs ../../../lib/aarch64-linux-gnu/libpthread.so.0 libpthread.so
ln -fs ../../../lib/aarch64-linux-gnu/libresolv.so.2 libresolv.so
ln -fs ../../../lib/aarch64-linux-gnu/librt.so.1 librt.so 
ln -fs ../../../lib/aarch64-linux-gnu/libthread_db.so.1 libthread_db.so
ln -fs ../../../lib/aarch64-linux-gnu/libutil.so.1 libutil.so
ln -fs ../../../lib/aarch64-linux-gnu/libuuid.so.1.3.0 libuuid.so
ln -fs ../../../lib/aarch64-linux-gnu/libz.so.1.2.11 libz.so
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

