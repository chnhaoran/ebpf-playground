# ebpf-playground
This is my simple ebpf playground. Test env is CentOS 9.

Run `setup-test-env.sh` to set up a veth pair (veth0 in root netns and veth1 in ns1).
Run `make xdp_drop` to build xdp_drop which will drop ipv6 packets.
Run `make myloader` to build my custom xdp loader. myloader will load xdp_drop.

Run `ip netns exec ns1 ping ff02::1` to generate ipv6 packets.

The result is as follows
```
➜  ebpf-playground git:(main) ✗ ./myloader veth0
libbpf: elf: skipping unrecognized data section(7) xdp_metadata
libbpf: elf: skipping unrecognized data section(7) xdp_metadata
libxdp: Compatibility check for dispatcher program failed: Unknown error 524
libxdp: Falling back to loading single prog without dispatcher
total dropped          1,          0 pkt/s
total dropped          3,          1 pkt/s
total dropped          5,          1 pkt/s
total dropped          7,          1 pkt/s
total dropped          9,          1 pkt/s
total dropped         10,          0 pkt/s
total dropped         12,          1 pkt/s
```

---
Reference:
https://developers.redhat.com/blog/2021/04/01/get-started-with-xdp#task_3__map_and_count_the_processed_packets
