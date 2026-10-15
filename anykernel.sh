### AnyKernel3 Ramdisk Mod Script
## osm0sis @ xda-developers
## Modified for ShadowKernel by Vortex

### AnyKernel setup
# global properties
properties() { '
kernel.string=ShadowKernel by Vortex
do.devicecheck=1
do.modules=0
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=curtana
device.name2=joyeuse
device.name3=excalibur
device.name4=gram
device.name5=miatoll
supported.versions=
supported.patchlevels=
supported.vendorpatchlevels=
'; } # end properties

# boot shell variables
# NOTE: BLOCK must be a single partition name (resolved by ak3-core.sh across
# every by-name path variant), a single /dev/... path, or "auto". A `;`-separated
# device list is NOT supported by AnyKernel3 and aborts with
# "Unable to determine <value> partition". Custom dd args go after one space.
BLOCK=boot;
# "auto" does not abort when the device has no A/B slots (miatoll is A-only);
# only "1" hard-aborts with "Unable to determine active slot".
IS_SLOT_DEVICE=auto;
RAMDISK_COMPRESSION=auto;
PATCH_VBMETA_FLAG=auto;

# import functions/variables and setup patching - see for reference (DO NOT REMOVE)
. tools/ak3-core.sh;

# Banner
ui_print " ";
ui_print "  ____  _               _                 _   ";
ui_print " / ___|| |__   __ _  __| | _____      __ | |  ";
ui_print " \\___ \\| '_ \\ / _\` |/ _\` |/ _ \\ \\ /\\ / / | |  ";
ui_print "  ___) | | | | (_| | (_| | (_) \\ V  V /  | |  ";
ui_print " |____/|_| |_|\\__,_|\\__,_|\\___/ \\_/\\_/   |_|  ";
ui_print " ";
ui_print "  ==========================================  ";
ui_print "     ShadowKernel by Vortex  |  Linux 4.14    ";
ui_print "  ==========================================  ";
ui_print " ";
ui_print "  Device : Xiaomi Miatoll (SM6250) family     ";
ui_print "  Root   : KernelSU-Next (manual hook mode)   ";
ui_print " ";
ui_print "  Installing kernel image ...                 ";
ui_print " ";

# boot install
dump_boot;

# write the new kernel image into the boot ramdisk
write_boot;

ui_print " ";
ui_print "  ShadowKernel installed successfully.        ";
ui_print "  Reboot and install the KernelSU-Next        ";
ui_print "  manager APK (v3.4.0 / 33294) to use root.   ";
ui_print " ";
## end boot install
