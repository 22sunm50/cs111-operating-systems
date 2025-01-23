# A Kernel Seedling
Lab0 of CS111 for the Winter Quarter of 2025. This lab is to set up a virtual machine and write a kernel module that adds a file to /proc/ to expose internal kernel information.

## Building
```shell
make
```

## Running
```shell
sudo insmod proc_count.ko
cat /proc/count
```
Result: my proc/count contained: "149"

## Cleaning Up
```shell
sudo rmmod proc_count
```

## Testing
```python
python -m unittest
```
Output:
```shell
...
----------------------------------------------------------------------
Ran 3 tests in 30.757s

OK
```
Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
Kernel ver: Linux 5.14.8-arch1-1
