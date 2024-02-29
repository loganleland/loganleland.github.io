---
layout: default
---

[leland.zip](https://leland.zip/)

## Pixel 8

![binjadeath](./images/pixel8.png)


- [Setup Environment](https://leland.zip/pixel8.html#setup-environment)
  - [Setup (rootless) Phone](https://leland.zip/pixel8.html#setup-rootless-phone)
  - [Setup Host](https://leland.zip/pixel8.html#setup-host)
- [Setup Host](https://leland.zip/pixel8.html#setup-host)
- [WorkFlow](https://leland.zip/pixel8.html#workflow)
- [Misc Files](https://leland.zip/pixel8.html#misc-files)
- [Kernel](https://leland.zip/pixel8.html#kernel)
  - [Generic Kernel Image (GKI)](https://leland.zip/pixel8.html#generic-kernel-image-gki)
  - [Building Pixel Kernel](https://leland.zip/pixel8.html#building-pixel-kernels)
- [GPU](https://leland.zip/pixel8.html#gpu)
  - [Public Vulns](https://leland.zip/pixel8.html#public-vulns)
  - [Driver](https://leland.zip/pixel8.html#driver)
  - [Vulkan](https://leland.zip/pixel8.html#vulkan)

## Keeping up to date with Pixel 8 security
- Pixel Update (security) Bulletins. This comes out monthly [link](https://source.android.com/docs/security/bulletin/pixel)

## Setup Environment

[The Rising Sea](https://ncatlab.org/nlab/show/The+Rising+Sea) A hard nut may be cracked not immediately by sheer punctual force, but eventually by gently immersing it into a whole body of water.

George Hotz: You gotta spend time to setup your environment nice because once your environment is nice everything else is nice. 

### Setup (rootless) Phone

#### Enable Developer Options
- Settings
- About phone
- Click Build number until Developer Options are enabled
- In settings search for and enable "USB debugging"
#### Install Termux
 - It is highly recommended to not install Termux apps from Play Store
 - Download the latest https://github.com/termux/termux-app/releases
 - Move the apk to a flash drive then open the apk to install
 - In Termux: passwd (to set a password for your user)
 - In Termux: pkg update && pkg install openssh nmap clang vim gdb binutils-is-llvm && ssh-keygen -A
 - Create a password, in Termux: passwd

### Setup Host
- Download adb: https://developer.android.com/tools/releases/platform-tools#downloads
- Download ssh

### WorkFlow
 Currently I'm ssh'ing into Termux over USB to interact with the phone. Here's how to do that. 

#### On Phone
- Start ssh: sshd
- Confirm ssh is running (usually port 8022): nmap localhost

#### On Host
- adb forward tcp:8022 tcp:8022
- ssh localhost -p 8022
- To copy a file from the phone to the host: ```scp u0_a265@192.168.1.121:/system/lib64/libvulkan.so .```

#### Debug Host
- If the following is seen:
    ```
    harvest@binja-box:~$ adb devices
    List of devices attached 59253ADYT113RN no permissions (user in plugdev group; are your udev rules wrong?); see [http://developer.android.com/tools/device.html]
    ```
    
    Run lsusb and record the ID of the pixel:
  
    ```
    Bus 001 Device 007: ID 18d1:4ee7 Google Inc. Nexus/Pixel Device (charging + debug)
    Then create /etc/udev/rules.d/51-android.rules with contents:
    SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", ATTR{idProduct}=="4ee7", MODE="0666", GROUP="plugdev"
    ```
    
    Then create /etc/udev/rules.d/51-android.rules with contents

    ```
    SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", ATTR{idProduct}=="4ee7", MODE="0666", GROUP="plugdev"
    ```

    Finally restart adb: ```adb kill-server && adb start-server```
  
- If the following is seen:
    ```
    harvest@binja-box:~$ adb devices
    List of devices attached
    59253ADYT113RN unauthorized
    ```
    Allow usb debugging on the phone. 

### Misc Files
- [/vendor/etc/ueventd.rc](./files/pixel8/ueventd.html)
- [/vendor/etc/selinux/vendor_file_contexts](./files/pixel8/vendor_file_contexts.html)
- [vendor_boot_modules.slider](./files/pixel8/vendor_boot_modules.slider.html)

### Kernel

 The Pixel 8 has a codeword of shiba and the Pixel 8 Pro has a codeword of husky. Combining these codewords produces shusky. 
 
- Binary path in AOSP tree: [device/google/shusky-kernel](https://android.googlesource.com/device/google/shusky-kernel/)
- Repository branch: [android-gs-shusky-5.15-android14-d1](https://android.googlesource.com/kernel/google-modules/soc/gs/+/refs/heads/android-gs-shusky-5.15-android14-d1)
- Clone and branch into shusky:
  - git clone https://android.googlesource.com/kernel/google-modules/soc/gs
  - cd gs
  - git checkout android-gs-shusky-5.15-android14-d1

#### Generic Kernel Image (GKI)

 Link to GKI doc: [Android GKI](https://source.android.com/docs/core/architecture/kernel/generic-kernel-image)
 
 ![gki](./images/generic-kernel-image-overview.png)


#### Building Pixel Kernels
 
 This [link](https://source.android.com/docs/setup/build/building-pixel-kernels) provides step-by-step instructions on how to download, compile, and flash a custom Pixel kernel for development

#### Loadable Kernel Modules
  This [link](https://source.android.com/docs/core/architecture/kernel/loadable-kernel-modules) introduces loadable kernel modules in Android.

### GPU
 
 The Pixel 8 uses ARM Immortalis-G715

- Google's shusky gpu git log: [log](https://android.googlesource.com/kernel/google-modules/gpu/+log/refs/heads/android-gs-shusky-5.15-android14-d1)
- (some) Documentation: [developer arm docs](https://developer.arm.com/Processors/Immortalis-G715#Technical-Specifications)

#### Public Vulns
- [CVE-2023-48409 / CVE-2023-48421](https://nvd.nist.gov/vuln/detail/CVE-2023-48409): In gpu_pixel_handle_buffer_liveness_update_ioctl of private/google-modules/gpu/mali_kbase/mali_kbase_core_linux.c, there is a possible out of bounds write due to an integer overflow. This could lead to local escalation of privilege with no additional execution privileges needed. User interaction is not needed for exploitation.
- [CVE-2023-4211](https://nvd.nist.gov/vuln/detail/CVE-2023-4211) A local non-privileged user can make improper GPU memory processing operations to gain access to already freed memory. 


#### Driver
- GPU character device: ```/dev/mali0``` with permission 666
- Mali Kernel Drivers, loaded based on [vendor_boot_modules.slider](./files/pixel8/vendor_boot_modules.slider.html) 
  - mali_kbase.ko: Handles requests from a corresponding user side library to perform GPU operations. This driver is not open source.
  - mali_pixel.ko: TBD
- Shusky git repo: ```google-modules/gpu``` provides file_operation structs / ioctl handler for interacting with ```mali_kbase.ko``` from userspace:
  - Clone the repo: git clone https://android.googlesource.com/kernel/google-modules/gpu
  - Checkout the shusky branch: git checkout android-gs-shusky-5.15-android14-d1
  - ioctl handler of interest is in ```mali_kbase/mali_kbase_core_linux.c``` with signature ```static long kbase_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)```
  - file_operations struct with ioctl handler for mali_kbase:
```
static const struct file_operations kbase_fops = {
  .owner = THIS_MODULE,
  .open = kbase_open,
  .release = kbase_release,
  .read = kbase_read,
  .poll = kbase_poll,
  .unlocked_ioctl = kbase_ioctl,
  .compat_ioctl = kbase_ioctl,
  .mmap = kbase_mmap,
  .check_flags = kbase_check_flags,
  .get_unmapped_area = kbase_get_unmapped_area,
};
```
- List of ```KBASE_IOCTL_TYPE``` in gpu/common/include/uapi/gpu/arm/midgard/mali_kbase_ioctl.h
  - KBASE_IOCTL_SET_FLAGS: _IOW(KBASE_IOCTL_TYPE, 1, struct kbase_ioctl_set_flags)
  - KBASE_IOCTL_GET_GPUPROPS: _IOW(KBASE_IOCTL_TYPE, 3, struct kbase_ioctl_get_gpuprops)
  - KBASE_IOCTL_MEM_ALLOC: _IOWR(KBASE_IOCTL_TYPE, 5, union kbase_ioctl_mem_alloc)
  - KBASE_IOCTL_MEM_QUERY: _IOWR(KBASE_IOCTL_TYPE, 6, union kbase_ioctl_mem_query)
  - KBASE_IOCTL_MEM_FREE: _IOW(KBASE_IOCTL_TYPE, 7, struct kbase_ioctl_mem_free)
  - KBASE_IOCTL_HWCNT_READER_SETUP: _IOW(KBASE_IOCTL_TYPE, 8, struct kbase_ioctl_hwcnt_reader_setup)
  - KBASE_IOCTL_HWCNT_SET: _IOW(KBASE_IOCTL_TYPE, 32, struct kbase_ioctl_hwcnt_values)
  - KBASE_IOCTL_DISJOINT_QUERY: _IOR(KBASE_IOCTL_TYPE, 12, struct kbase_ioctl_disjoint_query)
  - KBASE_IOCTL_GET_DDK_VERSION: _IOW(KBASE_IOCTL_TYPE, 13, struct kbase_ioctl_get_ddk_version)
  - KBASE_IOCTL_MEM_JIT_INIT_10_2: _IOW(KBASE_IOCTL_TYPE, 14, struct kbase_ioctl_mem_jit_init_10_2)
  - KBASE_IOCTL_MEM_JIT_INIT_11_5: _IOW(KBASE_IOCTL_TYPE, 14, struct kbase_ioctl_mem_jit_init_11_5)
  - KBASE_IOCTL_MEM_JIT_INIT: _IOW(KBASE_IOCTL_TYPE, 14, struct kbase_ioctl_mem_jit_init)
  - KBASE_IOCTL_MEM_SYNC: _IOW(KBASE_IOCTL_TYPE, 15, struct kbase_ioctl_mem_sync)
  - KBASE_IOCTL_MEM_FIND_CPU_OFFSET: _IOWR(KBASE_IOCTL_TYPE, 16, union kbase_ioctl_mem_find_cpu_offset)
  - KBASE_IOCTL_GET_CONTEXT_ID: _IOR(KBASE_IOCTL_TYPE, 17, struct kbase_ioctl_get_context_id)
  - KBASE_IOCTL_TLSTREAM_ACQUIRE: _IOW(KBASE_IOCTL_TYPE, 18, struct kbase_ioctl_tlstream_acquire)
  - KBASE_IOCTL_TLSTREAM_FLUSH _IO(KBASE_IOCTL_TYPE, 19)
  - KBASE_IOCTL_MEM_COMMIT: _IOW(KBASE_IOCTL_TYPE, 20, struct kbase_ioctl_mem_commit)
  - KBASE_IOCTL_MEM_ALIAS: _IOWR(KBASE_IOCTL_TYPE, 21, union kbase_ioctl_mem_alias)
  - KBASE_IOCTL_MEM_IMPORT: _IOWR(KBASE_IOCTL_TYPE, 22, union kbase_ioctl_mem_import)
  - KBASE_IOCTL_MEM_FLAGS_CHANGE: _IOW(KBASE_IOCTL_TYPE, 23, struct kbase_ioctl_mem_flags_change)
  - KBASE_IOCTL_STREAM_CREATE: _IOW(KBASE_IOCTL_TYPE, 24, struct kbase_ioctl_stream_create)
  - KBASE_IOCTL_FENCE_VALIDATE: _IOW(KBASE_IOCTL_TYPE, 25, struct kbase_ioctl_fence_validate)
  - KBASE_IOCTL_MEM_PROFILE_ADD: _IOW(KBASE_IOCTL_TYPE, 27, struct kbase_ioctl_mem_profile_add)
  - KBASE_IOCTL_STICKY_RESOURCE_MAP: _IOW(KBASE_IOCTL_TYPE, 29, struct kbase_ioctl_sticky_resource_map)
  - KBASE_IOCTL_STICKY_RESOURCE_UNMAP: _IOW(KBASE_IOCTL_TYPE, 30, struct kbase_ioctl_sticky_resource_unmap)
  - KBASE_IOCTL_MEM_FIND_GPU_START_AND_OFFSET:_IOWR(KBASE_IOCTL_TYPE, 31, union kbase_ioctl_mem_find_gpu_start_and_offset)
  - KBASE_IOCTL_CINSTR_GWT_START _IO(KBASE_IOCTL_TYPE, 33)
  - KBASE_IOCTL_CINSTR_GWT_STOP _IO(KBASE_IOCTL_TYPE, 34)
  - KBASE_IOCTL_CINSTR_GWT_DUMP: _IOWR(KBASE_IOCTL_TYPE, 35, union kbase_ioctl_cinstr_gwt_dump)
  - KBASE_IOCTL_MEM_EXEC_INIT: _IOW(KBASE_IOCTL_TYPE, 38, struct kbase_ioctl_mem_exec_init)
  - KBASE_IOCTL_GET_CPU_GPU_TIMEINFO: _IOWR(KBASE_IOCTL_TYPE, 50, union kbase_ioctl_get_cpu_gpu_timeinfo)
  - KBASE_IOCTL_CONTEXT_PRIORITY_CHECK: _IOWR(KBASE_IOCTL_TYPE, 54, struct kbase_ioctl_context_priority_check)
  - KBASE_IOCTL_SET_LIMITED_CORE_COUNT: _IOW(KBASE_IOCTL_TYPE, 55, struct kbase_ioctl_set_limited_core_count)
  - KBASE_IOCTL_KINSTR_PRFCNT_ENUM_INFO: _IOWR(KBASE_IOCTL_TYPE, 56, struct kbase_ioctl_kinstr_prfcnt_enum_info)
  - KBASE_IOCTL_KINSTR_PRFCNT_SETUP: _IOWR(KBASE_IOCTL_TYPE, 57, union kbase_ioctl_kinstr_prfcnt_setup)
  - KBASE_IOCTL_APC_REQUEST: _IOW(KBASE_IOCTL_TYPE, 66, struct kbase_ioctl_apc_request)
  - KBASE_IOCTL_BUFFER_LIVENESS_UPDATE: _IOW(KBASE_IOCTL_TYPE, 67, struct kbase_ioctl_buffer_liveness_update)
  - (not compiled in release builds) KBASE_IOCTL_TEST_TYPE (KBASE_IOCTL_TYPE + 1)
  - (not compiled in release builds) KBASE_IOCTL_EXTRA_TYPE (KBASE_IOCTL_TYPE + 2)

#### Vulkan

Vulkan is a low-overhead, cross-platform API for high-performance ~~vulnerabilities~~ 3D graphics


- [Home page](https://www.vulkan.org/)
- [Documentation](https://docs.vulkan.org/spec/latest/chapters/introduction.html)
- [Code Samples](https://github.com/KhronosGroup/Vulkan-Samples)

 ![Vulkan Components](./images/ape_graphics_vulkan.png)

| Component name           | Provider             | Description                                                                                                                                                                                                                                                                                                                                                              |
|--------------------------|----------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Vulkan Validation Layers | Android (in the NDK) | Libraries used during the development of Vulkan apps to find errors in an app's use of the Vulkan API. After API usage errors are found, these libraries should be removed.                                                                                                                                                                                              |
| Vulkan Runtime           | Android              | A native library, ```/system/lib64/libvulkan.so```, that provides a native Vulkan API. Most of Vulkan Runtime's functionality is implemented by a driver  provided by the GPU vendor. Vulkan Runtime wraps the driver, provides API  interception capabilities (for debugging and other developer tools), and  manages the interaction between the driver and the platform  dependencies. |
| Vulkan Driver            | SoC                  | Located: ```lib64/hw/vulkan.mali.so``` on the Pixel 8. Maps the Vulkan API onto hardware-specific GPU commands and interactions with the kernel graphics driver.                                                                                                                                                                                                                                                                |
