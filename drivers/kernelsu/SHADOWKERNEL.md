# KernelSU-Next (vendored)

This directory is a **vendored copy of the upstream KernelSU-Next kernel driver**,
integrated into the ShadowKernel tree so that every build is reproducible and
needs no network access at compile time.

| Item | Value |
| --- | --- |
| Upstream | <https://github.com/KernelSU-Next/KernelSU-Next> |
| Branch | `legacy` (non-GKI / in-tree kernels) |
| Tag | `v3.4.0-legacy` |
| Commit | `8af3d4fec33be32fa5d3f8dae4f380fac45bf2cd` |
| Reported version | `33294` (pinned in `Kbuild`, matches manager versionCode) |
| Hook mode | `CONFIG_KSU_MANUAL_HOOK` (manual hooks patched into the kernel tree) |
| License | GPL-2.0 (see `LICENSE`) |

## Directory layout

Upstream keeps the headers that are shared between kernel and userspace in a
top-level `uapi/` directory and includes them through a symlink
(`kernel/include/uapi -> ../../uapi`). The vendored copy keeps that structure
self-contained:

```
drivers/kernelsu/
├── include/uapi -> ../uapi     # symlink, same layout as upstream
├── uapi/                       # app_profile.h feature.h ksu.h selinux.h sulog.h supercall.h
├── Kbuild Kconfig Makefile LICENSE
└── compat/ core/ feature/ hook/ infra/ manager/ policy/ runtime/ selinux/ sulog/ supercall/
```

Only the contents of upstream `kernel/` and upstream `uapi/` are vendored; the
manager APK, userspace `ksud` and website are not part of the kernel driver.

## Local patches on top of upstream (ShadowKernel)

1. **`Kbuild`** - pins `KSU_VERSION_OVERRIDE ?= 33294` and
   `KSU_VERSION_TAG_OVERRIDE ?= v3.4.0-legacy`. Without this a vendored copy
   reports version `1` (upstream derives the version from `git rev-list --count`)
   and the manager refuses to pair with it.
2. **`sulog/event.c`** - upstream calls `ktime_get_boottime_ts64()` only for
   >= 4.19 and `get_monotonic_boottime(&timespec64)` below that, which does not
   compile on 4.14 (the pre-4.19 helper takes a `struct timespec *`). The guard
   now starts at 4.11, where the `ts64` helper was introduced.
3. **`feature/selinux_hide.c`** - `phys_to_page()` exists on arm64 but not on
   every architecture; a guarded fallback (`pfn_to_page(phys >> PAGE_SHIFT)`) is
   provided so the driver also builds on x86_64. Inert on arm64.

## Why the `legacy` branch?

KernelSU-Next has two incompatible hook ABIs:

* the **mainline/dev** branch, which hooks syscalls through
  `kernel/hook/syscall_event_bridge.c` and `pt_regs`, and
* the **legacy** branch, which exports the classic manual-hook entry points
  (`ksu_handle_execveat`, `ksu_handle_faccessat`, `ksu_handle_stat`,
  `ksu_handle_newfstat_ret`, `ksu_handle_fstat64_ret`, `ksu_handle_sys_read`,
  `ksu_handle_sys_reboot`, `ksu_handle_input_handle_event`).

Linux 4.14 has no `syscall_event_bridge`/`CONFIG_GENERIC_ENTRY` plumbing, so the
manual-hook ABI is the one that this kernel implements. All hook call sites are
guarded by `CONFIG_KSU_MANUAL_HOOK`:

| File | Hook |
| --- | --- |
| `fs/open.c` | `ksu_handle_faccessat()` |
| `fs/exec.c` | `ksu_handle_execveat()` (64-bit and 32-bit `compat_do_execve`) |
| `fs/stat.c` | `ksu_handle_stat()`, `ksu_handle_newfstat_ret()`, `ksu_handle_fstat64_ret()` |
| `fs/read_write.c` | `ksu_init_rc_hook` + `ksu_handle_sys_read()` |
| `kernel/reboot.c` | `ksu_handle_sys_reboot()` |
| `drivers/input/input.c` | `ksu_input_hook` + `ksu_handle_input_handle_event()` |

The KSU-Next Kbuild verifies this automatically - it aborts the build with
*"No hooks were defined, please integrate manual hooks in your kernel!"* if
`ksu_handle_sys_reboot` is missing from `kernel/reboot.c`.

## Updating

```bash
git clone --branch legacy https://github.com/KernelSU-Next/KernelSU-Next /tmp/ksunext
git -C /tmp/ksunext checkout v3.4.0-legacy      # or a newer legacy tag
rm -rf drivers/kernelsu
cp -a /tmp/ksunext/kernel drivers/kernelsu
rm -f drivers/kernelsu/setup.sh                 # upstream auto-integrator, not used here
# then re-pin KSU_VERSION_OVERRIDE / KSU_VERSION_TAG_OVERRIDE in drivers/kernelsu/Kbuild
```

`KSU_VERSION_OVERRIDE` must stay in sync with the manager APK that users install:
upstream computes it as `30000 + <git commit count> + 289` (so `30000 + 3005 + 289 = 33294`
for `v3.4.0-legacy`). The matching manager is
`KernelSU_Next_v3.4.0_33294-release.apk` from the upstream `v3.4.0` release.
