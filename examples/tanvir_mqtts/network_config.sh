#! /bin/bash
echo "configuring network interface"
sudo ../../dist/tools/tapsetup/tapsetup -d
sudo ../../dist/tools/tapsetup/tapsetup -c 2
sudo ip a a fec0:affe::1/64 dev tapbr0
ifconfig tapbr0
#make clean all term
echo "ifconfig 6 add fec0:affe::99"
echo "con fec0:affe::1 1883"
echo "configuration done !"
echo "ifconfig 6 add fec0:affe::100"
