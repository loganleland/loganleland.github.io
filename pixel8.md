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

- Documentation: [developer arm docs](https://developer.arm.com/Processors/Immortalis-G715#Technical-Specifications)

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
  - To find all file op structs: ```grep -nr "file_operations"```
 

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
