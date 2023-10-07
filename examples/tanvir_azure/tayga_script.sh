#! /bin/bash
echo "configuring tayga"
sudo tayga --mktun 
sudo ip link set nat64 up
sudo ip addr add 192.168.0.1 dev nat64      # (replace with your router's IPv4 address)
sudo ip addr add 2a02:908:100:a::438f dev nat64    # (replace with your router's IPv6 address)
sudo ip route add 192.168.255.0/24 dev nat64	# (tayga ipv4 pool)

sudo iptables -t nat -A POSTROUTING -o nat64 -j MASQUERADE
sudo iptables -t nat -A POSTROUTING -s 192.168.255.0/24 -j MASQUERADE

sudo ip route add 64:ff9b::/96 dev nat64

sudo tayga
sudo ping6 64:ff9b::140.82.121.4

