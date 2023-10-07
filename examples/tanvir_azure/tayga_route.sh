#! /bin/bash
echo "configuring tayga route"

sudo iptables -t nat -A POSTROUTING -o nat64 -j MASQUERADE
sudo iptables -t nat -A POSTROUTING -s 192.168.255.0/24 -j MASQUERADE
sudo ip route add 192.168.255.0/24 dev nat64
sudo ip addr add 192.168.0.145 dev nat64 #router ipv4
sudo ip addr add 2a02:908:1a2:8a00::9252 dev nat64 #enp2s0  ip 
sudo ip route add 64:ff9b::/96 dev nat64

#sudo dd iptables -A FORWARD -i nat64 -o tapbr0 -j ACCEPT
#sudo dd iptables -A FORWARD -i tapbr0 -o nat64 -j ACCEPT

