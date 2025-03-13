# Hey! I'm Filing Here

In this lab, I successfully implemented a 1 MiB EXT2 filesystem: initializing the superblock, setting up the block group descriptor table, creating and managing block and inode bitmaps, defining inodes for directories, files, and symbolic links, and writing directory structures and file data. This filesystem includes: 
2 directories: `root` and `lost+found` directories, 
1 regular file: `hello-world`, and
1 symbolic link: `hello`

## Building

To compile the executable (extext2-create), we can run: 
```
make
```
To run the executable (ext2-create) to create the image `cs111-base.img`
```
./ext2-create
```

## Running

To dump the filesystem information to help debug, we can run:
```
dumpe2fs cs111-base.img
```
To check that your filesystem is correct, we can run:
```
fsck.ext2 cs111-base.img
```
To mount our filestystem, we will first create a directory and then run the loop to mount it:
```
mkdir mnt
sudo mount -o loop cs111-base.img mnt
```
To view the contents within our filesystem, we can run:
```
cd mnt
ls -ain
```
The results should give:
```
total 7
   2 drwxr-xr-x 3    0    0 1024 Mar 12 23:20 .
2728 drwxr-xr-x 4 1000 1000 4096 Mar 12 23:21 ..
  13 lrw-r--r-- 1 1000 1000   11 Mar 12 23:20 hello -> hello-world
  12 -rw-r--r-- 1 1000 1000   12 Mar 12 23:20 hello-world
  11 drwxr-xr-x 2    0    0 1024 Mar 12 23:20 lost+found
```
## Cleaning up

To unmount our filesystem from the folder we created and then delete the folder:
```
sudo umount mnt
rmdir mnt
```
To clean up the remaining excess binary files such as the executable and image, we can run:
```
make clean
```