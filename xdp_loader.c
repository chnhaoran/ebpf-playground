#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/if_link.h>
#include <signal.h>
#include <net/if.h>
#include <assert.h>

#include <bpf/bpf.h>
#include <xdp/libxdp.h>

static int ifindex;
struct xdp_program *prog = NULL;

static void int_exit(int sig) {
    xdp_program__detach(prog, ifindex, XDP_ATTACHED_SKB, 0);
    xdp_program__close(prog);
    exit(0);
}

static void poll_stats(int map_fd, int interval) {
    int ncpus = libbpf_num_possible_cpus();
    if (ncpus < 0) {
        printf("Error get possible cpus\n");
        return;
    }
    long values[ncpus], prev[ncpus], total_pkts;
    total_pkts = 0;
    int i, key = 0;

    memset(prev, 0, sizeof(prev));

    while (1) {
        long sum = 0;
        sleep(interval);
        assert(bpf_map_lookup_elem(map_fd, &key, values) == 0);
        for (i = 0; i < ncpus; i++) {
            sum += (values[i] - prev[i]);
        }
        if (sum) {
            total_pkts += sum;
            printf("total dropped %10llu, %10llu pkt/s\n", total_pkts, sum/interval);
        }
        memcpy(prev, values, sizeof(values));
    }
    
}

int main(int argc, char *argv[]) {
    int prog_fd, map_fd, ret;
    struct bpf_object *bpf_obj;

    if (argc != 2) {
        printf("Usage: %s INFNAME\n", argv[0]);
        return 1;
    }

    ifindex = if_nametoindex(argv[1]);
    if (!ifindex) {
        printf("failed to get ifindex from name\n");
        return 1;
    }

    prog = xdp_program__open_file("xdp_drop.o", "xdp_drop_ipv6", NULL);
    if (!prog) {
        printf("Error, load xdp prog failed \n");
        return 1;
    }

    ret = xdp_program__attach(prog, ifindex, XDP_MODE_SKB, 0);
    if (ret) {
        printf("Error, set xdp fd on %d failed\n", ifindex);
        return ret;
    }

    // find map fd
    bpf_obj = xdp_program__bpf_obj(prog);
    map_fd = bpf_object__find_map_fd_by_name(bpf_obj, "rxcnt");
    if (map_fd < 0) {
        printf("Error, get map fd from bpf obj failed\n");
        return map_fd;
    }

    // remove attached program when interrupted or killed
    signal(SIGINT, int_exit);
    signal(SIGTERM, int_exit);

    poll_stats(map_fd, 2);

    return 0;
}