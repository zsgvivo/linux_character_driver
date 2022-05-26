sudo rmmod hello
sudo rm /dev/hello
cat /proc/devices | grep hello
make
sudo insmod hello.ko
sudo cat /proc/devices | grep hello
id=`cat /proc/devices | grep hello`
id=${id:0:(${#id}-6)}
sudo mknod /dev/hello c ${id} 0
gcc app.c -o app
sudo ./app