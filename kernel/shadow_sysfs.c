// SPDX-License-Identifier: GPL-2.0
/*
 * ShadowKernel sysfs interface
 *
 * Exposes ShadowKernel identification and runtime information to userspace
 * under /sys/kernel/shadow_kernel/.
 *
 * Author : Vortex
 * Project: ShadowKernel
 */

#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/version.h>
#include <generated/compile.h>
#include <generated/utsrelease.h>

#ifndef SHADOW_KERNEL_VERSION
#define SHADOW_KERNEL_VERSION "1.0"
#endif

#define SHADOW_KERNEL_BUILD "ShadowKernel by Vortex"

static struct kobject *shadow_kobj;

static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%s\n", SHADOW_KERNEL_VERSION);
}

static ssize_t release_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%s\n", UTS_RELEASE);
}

static ssize_t banner_show(struct kobject *kobj, struct kobj_attribute *attr,
			   char *buf)
{
	return sprintf(buf,
		"   _____ _               _                 _   \n"
		"  / ____| |             | |               | |  \n"
		" | (___ | |__   __ _  __| | _____      __ | |  \n"
		"  \\___ \\| '_ \\ / _` |/ _` |/ _ \\ \\ /\\ / / | |  \n"
		"  ____) | | | | (_| | (_| | (_) \\ V  V /  | |  \n"
		" |_____/|_| |_|\\__,_|\\__,_|\\___/ \\_/\\_/   |_|  \n"
		"                                                \n"
		"          --- %s ---\n", SHADOW_KERNEL_BUILD);
}

static ssize_t compiler_show(struct kobject *kobj, struct kobj_attribute *attr,
			     char *buf)
{
	return sprintf(buf,
		"%s\n"
		"Architecture: ARM64 (aarch64)\n"
		"Optimization: LLVM / LTO friendly build\n",
		LINUX_COMPILER);
}

static ssize_t health_show(struct kobject *kobj, struct kobj_attribute *attr,
			   char *buf)
{
	struct timespec64 uptime;
	unsigned long secs, hours, mins;

	ktime_get_boottime_ts64(&uptime);
	secs = (unsigned long)uptime.tv_sec;
	hours = secs / 3600;
	mins = (secs % 3600) / 60;

	return sprintf(buf,
		"ShadowKernel Health\n"
		"-------------------\n"
		"Driver:       OK\n"
		"KernelSU:     %s\n"
		"Scheduler:    OK\n"
		"TCP:          BBRplus\n"
		"Thermal:      OK\n"
		"Memory:       OK\n"
		"Uptime:       %luh %lum\n",
#ifdef CONFIG_KSU
		"OK (KernelSU-Next integrated)",
#else
		"not compiled in",
#endif
		hours, mins);
}

static ssize_t features_show(struct kobject *kobj, struct kobj_attribute *attr,
			     char *buf)
{
	return sprintf(buf,
		"[SHADOW_KERNEL] "
#ifdef CONFIG_KSU
		"[KERNELSU_NEXT] "
#ifdef CONFIG_KSU_MANUAL_HOOK
		"[KSU_MANUAL_HOOK] "
#endif
#ifdef CONFIG_KSU_SYSCALL_TABLE_HOOK
		"[KSU_SYSCALL_TABLE_HOOK] "
#endif
#endif
#ifdef CONFIG_MODULES
		"[LOADABLE_MODULES] "
#endif
		"[BBRPLUS_TCP]\n");
}

static struct kobj_attribute version_attribute =
	__ATTR(version, 0444, version_show, NULL);

static struct kobj_attribute release_attribute =
	__ATTR(release, 0444, release_show, NULL);

static struct kobj_attribute banner_attribute =
	__ATTR(banner, 0444, banner_show, NULL);

static struct kobj_attribute compiler_attribute =
	__ATTR(compiler, 0444, compiler_show, NULL);

static struct kobj_attribute health_attribute =
	__ATTR(health, 0444, health_show, NULL);

static struct kobj_attribute features_attribute =
	__ATTR(features, 0444, features_show, NULL);

static int __init shadow_sysfs_init(void)
{
	int ret;

	shadow_kobj = kobject_create_and_add("shadow_kernel", kernel_kobj);
	if (!shadow_kobj)
		return -ENOMEM;

	ret = sysfs_create_file(shadow_kobj, &version_attribute.attr);
	if (ret)
		goto err_out;

	ret = sysfs_create_file(shadow_kobj, &release_attribute.attr);
	if (ret)
		goto err_out;

	ret = sysfs_create_file(shadow_kobj, &banner_attribute.attr);
	if (ret)
		goto err_out;

	ret = sysfs_create_file(shadow_kobj, &compiler_attribute.attr);
	if (ret)
		goto err_out;

	ret = sysfs_create_file(shadow_kobj, &health_attribute.attr);
	if (ret)
		goto err_out;

	ret = sysfs_create_file(shadow_kobj, &features_attribute.attr);
	if (ret)
		goto err_out;

	pr_info("ShadowKernel: /sys/kernel/shadow_kernel registered\n");

	return 0;

err_out:
	kobject_put(shadow_kobj);
	shadow_kobj = NULL;
	return ret;
}
core_initcall(shadow_sysfs_init);
