#!/bin/bash
ip netns add fServer
ip netns add fPlay
ip link add dev virt01 type veth peer name virt02
ip link set virt01 netns fServer
ip link set virt02 netns fPlay
ip -n fServer addr add 10.0.0.1/24 dev virt01
ip -n fPlay addr add 10.0.0.2/24 dev virt02
ip -n fServer link set dev virt01 up
ip -n fPlay link set dev virt02 up
ip -n fServer addr show
ip -n fPlay addr show
ip netns exec fServer tc qdisc add dev virt01 root netem delay 0.1ms
echo "Задержка равна 0.1ms"
