MacBookPro11,3

inspired by:
* https://wiki.archlinux.org/index.php/MacBookPro10,x#Graphics
* https://github.com/Witko/nvidia-xrun
* https://github.com/torvalds/linux/blob/master/drivers/platform/x86/apple-gmux.c

How to use:
* Install https://github.com/0xbb/apple_set_os.efi
* Enable Intel GPU by default using https://github.com/0xbb/gpu-switch
* Disable Nvidia GPU using grub
```
function disable_nvidia {
  insmod iorw
  outb 0x7c2 1
  outb 0x7d4 0x28

  outb 0x7c2 2
  outb 0x7d4 0x10

  outb 0x7c2 2
  outb 0x7d4 0x40

  outb 0x7c2 1
  outb 0x7d4 0x50

  outb 0x7c2 0
  outb 0x7d4 0x50
}
```

after boot you can use nv-xrun to run enable nvidia gpu and run new X session on it.
