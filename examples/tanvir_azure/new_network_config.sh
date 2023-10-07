#! /bin/bash
echo "configuring network interface"
sudo ../../dist/tools/tapsetup/tapsetup -d
sudo ../../dist/tools/tapsetup/tapsetup -c 1 -a fec0:affe::1/64 -r 64:ff9b::1/96 2001:db8:1::1 #nat64ip6
#sudo ip a a fec0:affe::1/64 dev tapbr0
#sudo ip a a 192.168.1.10/24 dev tapbr0
ifconfig tapbr0
#make clean all term
echo "ifconfig 6 add fec0:affe::99"
echo "con fec0:affe::1 1883"
echo "configuration done !"
echo "ifconfig 6 add 2001:db8:1::3"
