#! /bin/bash
echo "configuring tayga"
sudo tayga --mktun 
sudo ip link set nat64 up
sudo ip addr add 192.168.0.1 dev nat64      # (replace with your router's IPv4 address)
#ip addr add 2001:db8:1::1 dev nat64    # (replace with your router's IPv6 address)
sudo ip addr add 2a02:908:100:a::438f dev nat64    # (replace with your router's IPv6 address)
sudo ip route add 192.168.255.0/24 dev nat64
sudo ip route add 2001:db8:1:ffff::/96 dev nat64
sudo tayga
sudo ping6 2001:db8:1:ffff::192.168.0.1
