---
layout: default
---

[leland.zip](https://leland.zip/)

## Pixel 8

[The Rising Sea](https://ncatlab.org/nlab/show/The+Rising+Sea) A hard nut may be cracked not immediately by sheer punctual force, but eventually by gently immersing it into a whole body of water.

![binjadeath](./images/pixel8.png)

- [Background](https://leland.zip/pixel8.html#background)
- [Keeping up to date with Pixel 8 security](https://leland.zip/pixel8.html#keeping-up-to-date-with-pixel-8-security)
- [Git Logs](https://leland.zip/pixel8.html#git-logs)
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


## Background
- [Project Zero: First handset with MTE on the market](https://googleprojectzero.blogspot.com/2023/11/first-handset-with-mte-on-market.html)
- [Android Universal Root: Exploiting Mobile GPU / Command Queue Drivers](https://www.youtube.com/watch?v=kEl5qbLZVeY)
- [Make KSMA Great Again: The Art of Rooting Android Devices by GPU MMU Features](https://www.youtube.com/watch?v=2qkwSPnQqrU)
- [Bad io_uring: A New Era of Rooting for Android](https://i.blackhat.com/BH-US-23/Presentations/US-23-Lin-bad_io_uring-wp.pdf?_gl=1*164nll3*_gcl_au*MTAzODk3OTg4NC4xNzA5NTMwNTU5*_ga*ODQ1NjU1MzQ1LjE3MDk1MzA1NjA.*_ga_K4JK67TFYV*MTcwOTUzMDU1OS4xLjAuMTcwOTUzMDU1OS4wLjAuMA..&_ga=2.169786529.1236663164.1709530561-845655345.1709530560)
- [DIFUZE: Android Kernel Driver Fuzzing](https://www.blackhat.com/docs/eu-17/materials/eu-17-Corina-Difuzzing-Android-Kernel-Drivers.pdf)
- ioctl data direction meaning:
  - ```_IO```: there's no parameters
  - ```_IOW```: userland is writing and kernel is reading
  - ```_IOR```: userland is reading and kernel is writing
  - ```_IORW```: ```_IOW``` and ```_IOR```

## Keeping up to date with Pixel 8 security
- Pixel Update (security) Bulletins. This comes out monthly [link](https://source.android.com/docs/security/bulletin/pixel)

## Git Logs
- GPU: [log]([https://android.googlesource.com/kernel/google-modules/gpu/+log/refs/heads/android-gs-shusky-5.15-android14-d1](https://android.googlesource.com/kernel/google-modules/gpu/+log/refs/heads/android-gs-shusky-5.15-android14-qpr1))
- Kernel device: [log]([https://android.googlesource.com/kernel/devices/google/shusky/+log/refs/heads/android-gs-shusky-5.15-android14-qpr1](https://android.googlesource.com/device/google/shusky-kernel/+log/refs/heads/master))


## Setup Environment

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
- To copy a file from host to phone: ```scp -P 8022 ioctl.h u0_a265@localhost:/data/data/com.termux/files/home```

Function I put in my bashrc
```
pixel8connect() {
  adb forward tcp:8022 tcp:8022
  ssh localhost -p 8022
}
```

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

- (light) Documentation: [developer arm docs](https://developer.arm.com/Processors/Immortalis-G715#Technical-Specifications)

#### Public Vulns
- [CVE-2023-48409 / CVE-2023-48421](https://nvd.nist.gov/vuln/detail/CVE-2023-48409): In gpu_pixel_handle_buffer_liveness_update_ioctl of private/google-modules/gpu/mali_kbase/mali_kbase_core_linux.c, there is a possible out of bounds write due to an integer overflow. This could lead to local escalation of privilege with no additional execution privileges needed. User interaction is not needed for exploitation.
- [CVE-2023-4211](https://nvd.nist.gov/vuln/detail/CVE-2023-4211) A local non-privileged user can make improper GPU memory processing operations to gain access to already freed memory. 


#### Driver
- GPU character device: ```/dev/mali0``` with permission 666
- ioctls in a single header file: [ioctl.h](./files/pixel8/gpu/ioctl.h)
- Mali Kernel Drivers, loaded based on [vendor_boot_modules.slider](./files/pixel8/vendor_boot_modules.slider.html) 
  - mali_kbase.ko: Handles requests from a corresponding user side library to perform GPU operations. This driver is not open source.
  - mali_pixel.ko: Built via [BUILD.bazel](https://android.googlesource.com/kernel/google-modules/gpu/+/refs/heads/android-gs-shusky-5.15-android14-d1/mali_pixel/)
- Shusky git repo: ```google-modules/gpu``` provides file_operation structs / ioctl handler for interacting with ```/dev/mali0``` from userspace:
  - Clone the repo: git clone https://android.googlesource.com/kernel/google-modules/gpu
  - Checkout the shusky branch: git checkout android-gs-shusky-5.15-android14-d1
  - ioctl handler of interest is in ```mali_kbase/mali_kbase_core_linux.c``` with signature ```static long kbase_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)```
  - file_operations struct with ioctl handler for mali_kbase:
  
~~~ C
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
~~~

- Example code for opening ```/dev/mali0``` and sending an ioctl to retrieve version

~~~ C
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ioctl.h"

struct kbase_ioctl_version_check {
  __u16 major;
  __u16 minor;
};

#define KBASE_IOCTL_TYPE 0x80

#define KBASE_IOCTL_VERSION_CHECK \
  _IOWR(KBASE_IOCTL_TYPE, 52, struct kbase_ioctl_version_check)

int main(void) {

  int fd = open("/dev/mali0", O_RDWR);
  if (-1 == fd)
  {
    printf("Failed to open /dev/mali0\n");
    return -1;
  }
  else
  {
    printf("Opened /dev/mali0\n");
  }

  struct kbase_ioctl_version_check param = {0};
  int res = ioctl(fd, KBASE_IOCTL_VERSION_CHECK, &param);
  if (res < 0) {
    int err = errno;
    printf("version check failed with code: %d\n", err);
  }
  else
  {
    printf("version check success\n - code: %d\n - major: %d\n - minor: %d\n",
           res, param.major, param.minor);
  }

  return 0;
}
~~~


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
