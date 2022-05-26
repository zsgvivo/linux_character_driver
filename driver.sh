sudo rmmod hello
sudo rm /dev/hello
cat /proc/devices | grep hello
make
sudo insmod hello.ko
sudo cat /proc/devices | grep hello
sudo mknod /dev/hello c 111 0
gcc app.c -o app
sudo ./app