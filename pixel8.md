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
- Example code for opening ```/dev/mali0``` and sending an ioctl to retrieve version major and minor

```C
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>

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
```
  
- List of ```KBASE_IOCTL_TYPE``` in gpu/common/include/uapi/gpu/arm/midgard/mali_kbase_ioctl.h
  - ```_IO```: there's no parameters
  - ```_IOW```: userland is writing and kernel is reading
  - ```_IOR```: userland is reading and kernel is writing
  - ```_IORW```: ```_IOW``` and ```_IOR```

| Name | Description | Command ID | Data Direction | struct |
| -    | -           | -          | -              | -      |
| KBASE_IOCTL_SET_FLAGS | Set kernel context creation flags | 1 | _IOW | struct kbase_ioctl_set_flags {<br>  __u32 create_flags;<br>}; |
| KBASE_IOCTL_GET_GPUPROPS | Read GPU properties from the kernel | 3 | _IOW | struct kbase_ioctl_get_gpuprops {<br>  __u64 buffer;  <br>  __u32 size;<br>  __u32 flags;<br>}; |
| KBASE_IOCTL_MEM_ALLOC | Allocate memory on the GPU | 5 | _IOWR | union kbase_ioctl_mem_alloc {<br>  struct {<br>    __u64 va_pages;<br>    __u64 commit_pages;<br>    __u64 extension;<br>    __u64 flags;<br>  } in;<br>  struct {<br>    __u64 flags;<br>    __u64 gpu_va;<br>  } out;<br>}; |
| KBASE_IOCTL_MEM_QUERY | Query properties of a GPU memory region | 6 | _IOWR | union kbase_ioctl_mem_query {<br>  struct {<br>    __u64 gpu_addr;<br>    __u64 query;<br>  } in;<br>  struct {<br>    __u64 value;<br>  } out;<br>}; |
| KBASE_IOCTL_MEM_FREE | Free a memory region | 7 | _IOW | struct kbase_ioctl_mem_free {<br>  __u64 gpu_addr;<br>}; |
| KBASE_IOCTL_HWCNT_READER_SETUP | Setup HWC dumper/reader | 8 | _IOW | struct kbase_ioctl_hwcnt_reader_setup {<br>  __u32 buffer_count;<br>  __u32 fe_bm;<br>  __u32 shader_bm;<br>  __u32 tiler_bm;<br>  __u32 mmu_l2_bm;<br>}; |
| KBASE_IOCTL_DISJOINT_QUERY | Query the disjoint counter | 12 | _IOR | struct kbase_ioctl_disjoint_query {<br>  __u32 counter;<br>}; |
| KBASE_IOCTL_GET_DDK_VERSION | Query the kernel version | 13 | _IOW | struct kbase_ioctl_get_ddk_version {<br>  __u64 version_buffer;<br>  __u32 size;<br>  __u32 padding;<br>}; |
| KBASE_IOCTL_MEM_JIT_INIT_10_2 | Initialize the just-in-time memory allocator (between kernel driver version 10.2--11.4) | 14 | _IOW | struct kbase_ioctl_mem_jit_init_10_2 {<br>  __u64 va_pages;<br>}; |
| KBASE_IOCTL_MEM_JIT_INIT_11_5 | Initialize the just-in-time memory allocator (between kernel driver version 11.5--11.19) | 14 | _IOW | struct kbase_ioctl_mem_jit_init_11_5 {<br>  __u64 va_pages;<br>  __u8 max_allocations;<br>  __u8 trim_level;<br>  __u8 group_id;<br>  __u8 padding[5];<br>}; |
| KBASE_IOCTL_MEM_JIT_INIT | Initialize the just-in-time memory allocator | 14 | _IOW | struct kbase_ioctl_mem_jit_init {<br>  __u64 va_pages;<br>  __u8 max_allocations;<br>  __u8 trim_level;<br>  __u8 group_id;<br>  __u8 padding[5];<br>  __u64 phys_pages;<br>}; |
| KBASE_IOCTL_MEM_SYNC | Perform cache maintenance on memory | 15 | _IOW | struct kbase_ioctl_mem_sync {<br>  __u64 handle;<br>  __u64 user_addr;<br>  __u64 size;<br>  __u8 type;<br>  __u8 padding[7];<br>}; |
| KBASE_IOCTL_MEM_FIND_CPU_OFFSET | Find the offset of a CPU pointer | 16 | _IOWR | union kbase_ioctl_mem_find_cpu_offset {<br>  struct {<br>    __u64 gpu_addr;<br>    __u64 cpu_addr;<br>    __u64 size;<br>  } in;<br>  struct {<br>    __u64 offset;<br>  } out;<br>}; |
| KBASE_IOCTL_GET_CONTEXT_ID | Get the kernel context ID | 17 | _IOR | struct kbase_ioctl_get_context_id {<br>  __u32 id;<br>}; |
| KBASE_IOCTL_TLSTREAM_ACQUIRE | Acquire a tlstream fd | 18 | _IOR | struct kbase_ioctl_tlstream_acquire {<br>  __u32 flags;<br>}; |
| KBASE_IOCTL_TLSTREAM_FLUSH | Flush a tlstream | 19 | _IO |  |
| KBASE_IOCTL_MEM_COMMIT | Change the amount of memory backing a region | 20 | _IOW | struct kbase_ioctl_mem_commit {<br>  __u64 gpu_addr;<br>  __u64 pages;<br>}; |
| KBASE_IOCTL_MEM_ALIAS | Create an alias of memory regions | 21 | _IOWR | union kbase_ioctl_mem_alias {<br>  struct {<br>    __u64 flags;<br>    __u64 stride;<br>    __u64 nents;<br>    __u64 aliasing_info;<br>  } in;<br>  struct {<br>    __u64 flags;<br>    __u64 gpu_va;<br>    __u64 va_pages;<br>  } out;<br>}; |
| KBASE_IOCTL_MEM_IMPORT | Import memory for use by the GPU | 22 | _IOWR | union kbase_ioctl_mem_import {<br>  struct {<br>    __u64 flags;<br>    __u64 phandle;<br>    __u32 type;<br>    __u32 padding;<br>  } in;<br>  struct {<br>    __u64 flags;<br>    __u64 gpu_va;<br>    __u64 va_pages;<br>  } out;<br>}; |
| KBASE_IOCTL_MEM_FLAGS_CHANGE | Change the flags for a memory region | 23 | _IOW | struct kbase_ioctl_mem_flags_change {<br>  __u64 gpu_va;<br>  __u64 flags;<br>  __u64 mask;<br>}; |
| KBASE_IOCTL_STREAM_CREATE | Create a synchronisation stream. Note that this is also called a "timeline" | 24 | _IOW | struct kbase_ioctl_stream_create {<br>  char name[32];<br>}; |
| KBASE_IOCTL_FENCE_VALIDATE | Validate a fd refers to a fence | 25 | _IOW | struct kbase_ioctl_fence_validate {<br>  int fd;<br>}; |
| KBASE_IOCTL_MEM_PROFILE_ADD | Provide profiling information to kernel | 27 | _IOW | struct kbase_ioctl_mem_profile_add {<br>  __u64 buffer;<br>  __u32 len;<br>  __u32 padding;<br>}; |
| KBASE_IOCTL_STICKY_RESOURCE_MAP | Permanently map an external resource | 29 | _IOW | struct kbase_ioctl_sticky_resource_map {<br>  __u64 count;<br>  __u64 address;<br>}; |
| KBASE_IOCTL_STICKY_RESOURCE_UNMAP | Unmap a resource mapped which was previously permanently mapped | 30 | _IOW | struct kbase_ioctl_sticky_resource_unmap {<br>  __u64 count;<br>  __u64 address;<br>}; |
| KBASE_IOCTL_MEM_FIND_GPU_START_AND_OFFSET | Find the start address of the GPU memory region for the given gpu address and the offset of that address into the region | 31 | _IOWR | union kbase_ioctl_mem_find_gpu_start_and_offset {<br>  struct {<br>    __u64 gpu_addr;<br>    __u64 size;<br>  } in;<br>  struct {<br>    __u64 start;<br>    __u64 offset;<br>  } out;<br>}; |
| KBASE_IOCTL_HWCNT_SET | Values to set dummy the dummy counters to| 32 | _IOW | struct kbase_ioctl_hwcnt_values {<br>  __u64 data;<br>  __u32 size;<br>  __u32 padding;<br>}; |
| KBASE_IOCTL_CINSTR_GWT_START | Start the GPU write tracking | 33 | _IO | |
| KBASE_IOCTL_CINSTR_GWT_STOP | Stop the GPU write tracking | 34 | _IO | |
| KBASE_IOCTL_CINSTR_GWT_DUMP | Used to collect all GPU write fault addresses | 35 | _IOWR | union kbase_ioctl_cinstr_gwt_dump {<br>  struct {<br>    __u64 addr_buffer;<br>    __u64 size_buffer;<br>    __u32 len;<br>    __u32 padding;<br>  } in;<br>  struct {<br>    __u32 no_of_addr_collected;<br>    __u8 more_data_available;<br>    __u8 padding[27];<br>  } out;<br>}; |
| KBASE_IOCTL_MEM_EXEC_INIT | Initialise the EXEC_VA memory zone | 38 | _IOW | struct kbase_ioctl_mem_exec_init {<br>  __u64 va_pages;<br>}; |
| KBASE_IOCTL_GET_CPU_GPU_TIMEINFO | Request zero or more types of cpu/gpu time (counter values) | 50 | _IOWR | union kbase_ioctl_get_cpu_gpu_timeinfo {<br>  struct {<br>    __u32 request_flags;<br>    __u32 paddings[7];<br>  } in;<br>  struct {<br>    __u64 sec;<br>    __u32 nsec;<br>    __u32 padding;<br>    __u64 timestamp;<br>    __u64 cycle_counter;<br>  } out;<br>}; |
| KBASE_IOCTL_CONTEXT_PRIORITY_CHECK | Check the max possible priority | 54 | _IOWR | struct kbase_ioctl_context_priority_check {<br>  __u8 priority;<br>}; |
| KBASE_IOCTL_SET_LIMITED_CORE_COUNT | Set the limited core count | 55 | _IOW | struct kbase_ioctl_set_limited_core_count {<br>  __u8 max_core_count;<br>}; |
| KBASE_IOCTL_KINSTR_PRFCNT_ENUM_INFO | Enum Performance counter information | 56 | _IOWR | struct kbase_ioctl_kinstr_prfcnt_enum_info {<br>  __u32 info_item_size;<br>  __u32 info_item_count;<br>  __u64 info_list_ptr;<br>}; |
| KBASE_IOCTL_KINSTR_PRFCNT_SETUP | Setup HWC dumper/reader | 57 | _IOWR | union kbase_ioctl_kinstr_prfcnt_setup {<br>  struct {<br>    __u32 request_item_count;<br>    __u32 request_item_size;<br    __u64 requests_ptr;<br>  } in;<br>  struct {<br>    __u32 prfcnt_metadata_item_size;<br>    __u32 prfcnt_mmap_size_bytes;<br>  } out;<br>}; |
| KBASE_IOCTL_APC_REQUEST | (Pixel specific ioctl) GPU asynchronous power control (APC) request | 66 | _IOW | struct kbase_ioctl_apc_request {<br>  __u32 dur_usec;<br>}; |
| KBASE_IOCTL_BUFFER_LIVENESS_UPDATE | (Pixel specific ioctl) Update the live ranges of buffers from previous frame| 67 | _IOW | struct kbase_ioctl_buffer_liveness_update {<br>  __u64 live_ranges_address;<br>  __u64 live_ranges_count;<br>  __u64 buffer_va_address;<br>  __u64 buffer_sizes_address;<br>  __u64 buffer_count;<br>}; |


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
