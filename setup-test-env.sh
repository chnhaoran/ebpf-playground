#!/bin/bash -e

ip netns add ns1
ip link add veth0 type veth peer veth1
ip link
ip link set veth1 netns ns1
ip netns exec ns1 ip link set veth1 up
ip netns exec ns1 ip a add 192.168.1.2/24 dev veth1
ip link set veth0 up
ip a add 192.168.1.1/24 dev veth0

# ping ipv6 address
# ip netns exec ns1 ping ff02::1