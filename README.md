# ⚡ ShadowKernel

### by Vortex

[![ShadowKernel Build](https://github.com/ShadowDarkXd00/ShadowKernel/actions/workflows/build.yml/badge.svg)](https://github.com/ShadowDarkXd00/ShadowKernel/actions/workflows/build.yml)
![Kernel](https://img.shields.io/badge/Kernel-4.14.357-blue.svg)
![Toolchain](https://img.shields.io/badge/Toolchain-Clang%20%2F%20LLVM-purple.svg)
![Root](https://img.shields.io/badge/Root-KernelSU--Next-success.svg)
![Modules](https://img.shields.io/badge/Kernel%20Modules-Loadable-orange.svg)
![Arch](https://img.shields.io/badge/Arch-ARM64--v8a-informational.svg)
![License](https://img.shields.io/badge/License-GPL--2.0-lightgrey.svg)

A clean, performance- and battery-focused Linux 4.14 kernel for the **Xiaomi Miatoll
(SM6250)** family, built for **custom ROMs** as well as stock MIUI/HyperOS, with
**KernelSU-Next** integrated in-tree and a **module loader that actually works**
(unsigned LKMs can be inserted).

---

## 📱 Supported devices (Miatoll unified)

| Device | Codename |
| --- | --- |
| Redmi Note 9 Pro / Pro Max (India) | `curtana` |
| Poco M2 Pro | `gram` |
| Redmi Note 9S | `joyeuse` |
| Redmi Note 9 Pro Max | `excalibur` |
| Any other Miatoll unified build | `miatoll` |

The AnyKernel3 installer aborts on unknown devices, so it is safe to try.

---

## ✨ Highlights

- **KernelSU-Next in-tree** — manual-hook mode, reports kernel version `33294`
  (upstream tag `v3.4.0-legacy`). No ramdisk patching needed, works on
  MIUI/HyperOS and every AOSP-based custom ROM.
- **Real module (LKM) support** — `CONFIG_MODULES=y`, `CONFIG_MODULE_SIG` disabled,
  `CONFIG_KALLSYMS_ALL=y`. External modules such as the KernelSU-Next LKM,
  `susfs.ko`, KernelPatch/APatch payloads and other kernel loaders can be loaded
  with `insmod` on a production build.
- **Custom ROM ready** — Android-critical options are enforced and validated on
  every build (`ANDROID_BINDER_IPC`, `SECURITY_SELINUX`, `EXT4_FS_SECURITY`,
  `TMPFS_XATTR`, `OVERLAY_FS`, verity/overlay, f2fs, erofs…).
- **Flash-anything packaging** — AnyKernel3 zip that TWRP, OrangeFox, Kernel
  Flasher, Horizon Kernel Flasher and similar apps can install, plus raw images
  (`Image.gz-dtb`, `System.map`, config) as CI artifacts.
- **Reproducible automation** — one GitHub Actions workflow builds, validates the
  configuration, packages the zip and optionally publishes a GitHub release.
- **ShadowKernel sysfs interface** — `/sys/kernel/shadow_kernel/` exposes
  `version`, `release`, `banner`, `compiler`, `health` and `features`.
- **Network & scheduler tuning** — BBRplus TCP, tuned CFS, IRQ balancing.

---

## 📥 Installation

### 1. Flash the kernel

Download the flashable zip from **Releases** or from the latest **Actions** run
artifact (`ShadowKernel-<version>-<date>.zip`), then:

- **Recovery**: TWRP / OrangeFox → *Install* → select the zip → flash → reboot.
- **Kernel flasher apps**: Kernel Flasher / Horizon / EX Kernel Manager → flash
  the same zip (they read `kernel.string` and the AnyKernel3 layout).
- **Fastboot** (advanced): `fastboot flash boot <boot.img>` using the raw
  `Image.gz-dtb` artifact to rebuild a boot image.

The installer picks the real boot block automatically
(`/dev/block/bootdevice/by-name/boot` with fallbacks) and is **systemless** —
your existing ramdisk/Magisk setup is preserved.

### 2. Install the root manager

KernelSU-Next is built into the kernel; it only needs its manager app:

> **KernelSU-Next v3.4.0 (33294)** —
> [`KernelSU_Next_v3.4.0_33294-release.apk`](https://github.com/KernelSU-Next/KernelSU-Next/releases/download/v3.4.0/KernelSU_Next_v3.4.0_33294-release.apk)

The kernel advertises `33294`, so the manager pairs with it exactly. Grant root
per-app from the manager — no Magisk-style ramdisk modification required.

---

## 🧩 Loading your own kernel modules (LKM)

ShadowKernel is deliberately built so that **unsigned external modules load**:

| Option | Value | Why |
| --- | --- | --- |
| `CONFIG_MODULES` | `y` | loadable module support |
| `CONFIG_MODULE_UNLOAD` | `y` | allow `rmmod` |
| `CONFIG_MODULE_FORCE_LOAD` | `y` | `insmod -f` / `modprobe --force` |
| `CONFIG_MODULE_SIG` | *not set* | no signature requirement |
| `CONFIG_MODVERSIONS` | *not set* | no symbol CRC mismatch |
| `CONFIG_KALLSYMS` / `CONFIG_KALLSYMS_ALL` | `y` | symbol resolution for LKMs |
| `CONFIG_TRIM_UNUSED_KSYMS` | *not set* | exported symbols stay resolvable |

Build an external module against this kernel and insert it:

```bash
# on a host, with this kernel's out/ tree available
make -C <kernel-src> O=<kernel-src>/out ARCH=arm64 \
     CC=clang CLANG_TRIPLE=aarch64-linux-gnu- \
     CROSS_COMPILE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1 \
     M=$PWD modules

# on the device
su -c 'insmod ./your_module.ko'
su -c 'dmesg | tail'
```

Because the kernel uses manual KSU hooks instead of kprobes, both in-tree
KernelSU-Next and additional LKM payloads (for example `susfs.ko` or the
KernelSU-Next LKM used by other kernels) can coexist with it.

---

## 🔧 Configuration

The kernel is configured from `arch/arm64/configs/vendor/xiaomi/miatoll_defconfig`
(a `savedefconfig`-style minimal config) and the build **fails fast** if any of
these regress: KernelSU, binder, SELinux, ext4 security labels, tmpfs xattrs,
overlayfs, or module support. It also refuses to finish if the generated `.config`
contains `=m` entries, because the AnyKernel3 zip only ships the kernel image.

Selected options:

```
CONFIG_LOCALVERSION="-ShadowKernel"
CONFIG_KSU=y
CONFIG_KSU_MANUAL_HOOK=y
CONFIG_MODULES=y
CONFIG_MODULE_UNLOAD=y
CONFIG_MODULE_FORCE_LOAD=y
CONFIG_KALLSYMS=y
CONFIG_KALLSYMS_ALL=y
CONFIG_TMPFS_XATTR=y
```

ShadowKernel identification at runtime:

```bash
cat /sys/kernel/shadow_kernel/version   # ShadowKernel feature version
cat /sys/kernel/shadow_kernel/release   # 4.14.357-ShadowKernel
cat /sys/kernel/shadow_kernel/features  # [SHADOW_KERNEL] [KERNELSU_NEXT] [KSU_MANUAL_HOOK] [LOADABLE_MODULES] [BBRPLUS_TCP]
cat /sys/kernel/shadow_kernel/health
uname -a
```

---

## 🏗️ Building

### GitHub Actions (recommended)

1. Open the **Actions** tab → **ShadowKernel Build** → *Run workflow*.
2. Pick a `defconfig` (default `vendor/xiaomi/miatoll_defconfig`) and a
   `toolchain` (`neutron`, `zyc-clang-16`, `zyc-clang-14`).
3. Optional: `extra_configs` to force additional options
   (e.g. `CONFIG_LTO_CLANG=y`), and `publish_release` to publish the zip as a
   GitHub release with your own `release_tag`.

The workflow installs the dependencies, verifies the KernelSU-Next hooks,
downloads and caches the clang toolchain, caches `ccache`, generates and
**validates** the config, builds with `LLVM=1 LLVM_IAS=1`, packages the
AnyKernel3 zip, uploads artifacts and prints a build summary.

### Local build

```bash
# dependencies (Debian/Ubuntu)
sudo apt-get install -y build-essential bc bison flex libssl-dev libelf-dev \
  libncurses5-dev python3 ccache zip curl

# toolchain (example: Neutron Clang)
mkdir -p clang && (cd clang && curl -LO \
  https://github.com/Neutron-Toolchains/clang-build-catalogue/releases/download/06092026/neutron-clang-06092026.tar.zst \
  && tar --zstd -xf neutron-clang-06092026.tar.zst)
export PATH="$PWD/clang/bin:$PATH"

# configure
make O=out ARCH=arm64 vendor/xiaomi/miatoll_defconfig

# build
make -j"$(nproc)" O=out ARCH=arm64 \
  CC="ccache clang" \
  CLANG_TRIPLE=aarch64-linux-gnu- \
  CROSS_COMPILE=aarch64-linux-gnu- \
  CROSS_COMPILE_ARM32=arm-linux-gnueabi- \
  LLVM=1 LLVM_IAS=1 \
  LOCALVERSION="-ShadowKernel" \
  KBUILD_BUILD_USER=Vortex KBUILD_BUILD_HOST=ShadowKernel

# package (AnyKernel3)
git clone --depth=1 https://github.com/osm0sis/AnyKernel3.git AK3
rm -rf AK3/.git AK3/README.md
cp out/arch/arm64/boot/Image.gz-dtb AK3/
cp anykernel.sh AK3/
(cd AK3 && zip -r9 ../ShadowKernel.zip .)
```

---

## 📂 Project layout

```
arch/arm64/configs/vendor/xiaomi/miatoll_defconfig   device config
arch/arm64/configs/stock_defconfig                   vendor stock reference
drivers/kernelsu/                                    KernelSU-Next v3.4.0-legacy (vendored)
  SHADOWKERNEL.md                                    provenance + update notes
kernel/shadow_sysfs.c                                /sys/kernel/shadow_kernel
fs/open.c fs/exec.c fs/stat.c fs/read_write.c        KSU manual hooks
kernel/reboot.c drivers/input/input.c                KSU manual hooks
anykernel.sh                                         AnyKernel3 installer definition
.github/workflows/build.yml                          CI: build + package + release
```

---

## 🤝 Credits

- **Vortex** — ShadowKernel maintainer, rebrand, KSU-Next integration, LKM/loader
  bring-up and CI.
- **heySaish** — original kernel base (formerly *Void Kernel*) whose tuning and
  hook work this project builds upon.
- **KernelSU-Next** team — in-tree root engine (`legacy` branch, `v3.4.0-legacy`).
- **osm0sis** — AnyKernel3 installer.
- **Neutron Toolchains**, **ZyCromerZ** — Clang/LLVM toolchains used by CI.
- **Linux kernel community**, **Qualcomm** and **Xiaomi** — upstream sources.

---

## 📜 License

Released under the **GNU General Public License v2.0** (`COPYING`), matching the
Linux kernel and KernelSU-Next (GPL-2.0, see `drivers/kernelsu/LICENSE`).
Kernel source must remain available to anyone you distribute binaries to.

---

<p align="center"><b>ShadowKernel by Vortex</b> — built for the Miatoll community 🖤</p>
