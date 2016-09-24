On the process of fully ditching my shaddy Qualcomm based pocket spy I felt the need to make a few changes to my Replicant experience, they are mostly contained in this repository.

While it pains me to taint such a great project with non-free dependencies, it was necessary in order to be able to get rid of my current shaddy device.

##Changes applied to Replicant 4.2
- Switched the internal storage from physical to emulated to allow encryption.
- Re-enabled video recording, hardware video encoding and decoding. 
[Based on Wolfgang's proposal for the S3.](https://www.mail-archive.com/replicant@lists.osuosl.org/msg00444.html) (Requires non-free MFC firmware)
- Changed camera preview from rgb565 to yuv420sp to allow for QR code scanning. (Requires EGL, Gralloc and Mali non-free drivers)

##Recent builds and signatures
Public Key: 5E1C EF76 A78A A66B 0701 37C7 426E C780 9555 34E6

<https://github.com/GrimKriegor/replicant-device_samsung_galaxys2-common/releases/>

##Repartitioning, expanding /data and shrinking /emmc
(!!) **User discretion is advised** (!!)

**Repartitioning the internal memory can brick your phone, be careful!**

I've prepared a partition table that expands /data (mmcblk0p10) as much as possible, shrinks /sdcard (mmcblk0p11) and /preload (mmcblk0p12) by changing the position of their border blocks, resulting in a 14GiB /data partition.

PIT file and signature are avaliable here:

<https://grimkriegor.zalkeen.pw/replicant/pit-files/>

Unless you change the coordinates of a partition its contents should be safe, simply flash the partition table using Heimdall:

    heimdall flash --repartition --PIT I9100_14GB_grim.pit

##Required non-free files, firmware and drivers
Extracted from CyanogenMod 10.1.3 (cm-10.1.3-i9100.zip)

    system
    ├── cameradata
    │   ├── datapattern_420sp.yuv
    │   └── datapattern_front_420sp.yuv
    └── lib
        ├── egl
        │   ├── egl.cfg
        │   ├── libEGL_mali.so
        │   ├── libGLESv1_CM_mali.so
        │   └── libGLESv2_mali.so
        ├── hw
        │   └── gralloc.exynos4.so
        ├── libMali.so
        └── libUMP.so
    vendor
    └── firmware
        └── mfc_fw.bin

Check out [Paul Kocialkowski's guide](http://code.paulk.fr/article16/missing-proprietary-firmwares-in-android-systems) on how to extract non-free firmware from CyanogenMod.

Regarding the drivers and miscelaneous files, you can manually extract them from the CM zip, place them in a folder structure similar to the one displayed above and do something like

    find system -type f -exec sh -c "adb push {} /{} ; adb shell chmod 644 /{}" \;
