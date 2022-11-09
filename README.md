# lift-lockdown

A simple kernel module that helps you disable linux kernel lockdown temporarily while secure boot is still on.

Create issues if you have any questions.

## 1. How to use?

Before going further, make sure you have MOK (Machine-Owner-Key) and the MOK has been enrolled.

1. Compile:

   ```bash
   $ git clone https://github.com/HyperSine/lift-lockdown.git
   $ cd lift-lockdown
   $ make all
   ```

2. Sign kernel module by your MOK. You can use `kmodsign` to do it.

   ```bash
   $ sudo kmodsign sha256 /var/lib/shim-signed/mok/MOK.priv /var/lib/shim-signed/mok/MOK.der ./lift_lockdown.ko 
   ```

   If you store your MOK in different place, replace the paths above to yours.

3. Load the module and check if lockdown is lifted.

   ```bash
   $ sudo insmod ./lift_lockdown.ko
   ```

   Then check the output of `dmesg`. There should be something looking like the following:

   ```
   [ 5713.928665] lift_lockdown: kallsyms_lookup_name = 0xffffffffb63ba120
   [ 5713.942717] lift_lockdown: kernel_locked_down = 0xffffffffb8b64d08
   [ 5713.942718] lift_lockdown: lockdown is lifted.
   ```

   And check `/sys/kernel/security/lockdown`. It should point to `none`:

   ```bash
   $ cat /sys/kernel/security/lockdown
   [none] integrity confidentiality
   ```

4. Unload the module and have fun and enjoy.

   ```bash
   $ sudo rmmod lift_lockdown
   ```

## 2. What if I want to restore lockdown?

Write `integrity` or `confidentiality` to `/sys/kernel/security/lockdown`, then you will see lockdown is restored.

```bash
$ sudo sh -c 'echo integrity > /sys/kernel/security/lockdown'
```
