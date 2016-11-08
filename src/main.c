/* simple test program for netmap library */
#include <stdio.h>
#include <stdlib.h>
#define NETMAP_WITH_LIBS
#include <net/netmap_user.h>
#include <poll.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <uthash.h>

static uint8_t myip[] = {0xa,0x14,0x28,0xa};

static void
usage(const char *progname)
{
	fprintf(stderr, "usage: %s IFNAME [w|r]\n", progname);
	exit(EXIT_FAILURE);
}

static uint32_t
bytetoint(u_int8_t ip[]) {
	uint32_t i = ip[0] << 24;
	i += ip[1] << 16;
	i += ip[2] << 8;
	i += ip[3];

	return i;
}

static void
my_cb(u_char *arg, const struct nm_pkthdr *h, const u_char *buf)
{
	int *count = (int *)arg;
	struct ethhdr *ehdr;
	struct iphdr *ihdr;
	struct ether_arp *ahdr;

	(*count)++;
	if (h->flags == 0) {
		ehdr = (struct ethhdr *)buf;
		//fprintf(stderr, "received %d bytes at %p count %d slot %p buf_index %d\n",
		//		h->len, buf, *count, h->slot, h->slot->buf_idx);

		if ((ehdr->h_proto == htons(ETH_P_ARP)) || (ehdr->h_proto == htons(ETH_P_RARP))) {
			ahdr = (struct ether_arp *)(buf + sizeof(struct ethhdr));

			if (bytetoint(ahdr->arp_tpa) == bytetoint(myip)) {
				printf("myip");
				nm_inject(h->d, buf, h->len);
				return;
			}

			printf("r:%u, m:%u\t", ahdr->arp_tpa[2], bytetoint(myip));
			printf("Arp op:%hu", htons(ahdr->ea_hdr.ar_op));
			printf(" proto:%hu", htons(ahdr->ea_hdr.ar_pro));
			printf(" Req_ip: %d.%d.%d.%d", ahdr->arp_tpa[0], ahdr->arp_tpa[1], ahdr->arp_tpa[2], ahdr->arp_tpa[3]);
			printf(" Reqr_ip: %d.%d.%d.%d", ahdr->arp_spa[0], ahdr->arp_spa[1], ahdr->arp_spa[2], ahdr->arp_spa[3]);


			printf("\t\t\tARP Packet s: %x:%x:%x:%x:%x:%x", ehdr->h_source[0],ehdr->h_source[1],ehdr->h_source[2],
												ehdr->h_source[3],ehdr->h_source[4],ehdr->h_source[5]);
			printf("d: %x:%x:%x:%x:%x:%x\n", ehdr->h_dest[0],ehdr->h_dest[1],ehdr->h_dest[2],
														ehdr->h_dest[3],ehdr->h_dest[4],ehdr->h_dest[5]);
		} else if (ehdr->h_proto == htons(ETH_P_IP)) {
			struct in_addr addr;
			ihdr = (struct iphdr *)(buf + sizeof(struct ethhdr));
			addr.s_addr = ihdr->saddr;
			printf("\t\t\tip packet ip:%s\tproto:%d", inet_ntoa(addr), ihdr->protocol);

			if (ihdr->protocol == 0x11) {
				struct udphdr *uhdr;
				uhdr = (struct udphdr *)(buf + sizeof(struct ethhdr) + sizeof(struct iphdr));
				printf("sport:%d\tdport:%d\n", ntohs(uhdr->source), ntohs(uhdr->dest));

			} else {
				printf("\n");
			}

		}
	}





}

int
main(int argc, char *argv[])
{
	struct nm_desc *d;
	struct pollfd pfd;
	char buf[2048];
	int count = 0;

	if (argc < 2) {
		usage(argv[0]);
	}

	bzero(&pfd, sizeof(pfd));

	d = nm_open(argv[1], NULL, 0, 0);
	if (d == NULL) {
		fprintf(stderr, "no netmap\n");
		exit(0);
	}

	pfd.fd = d->fd;
	pfd.events = argv[2] && argv[2][0] == 'w' ? POLLOUT : POLLIN;
	fprintf(stderr, "working on %s in %s mode\n", argv[1], pfd.events == POLLIN ? "read" : "write");

	for (;;) {
		if (pfd.events == POLLIN) {
			nm_dispatch(d, -1, my_cb, (void *)&count);
		} else {
			if (nm_inject(d, buf, 60) > 0) {
				count++;
				continue;
			}
			fprintf(stderr, "polling after sending %d\n", count);
			count = 0;
		}
		poll(&pfd, 1, 1000);
	}
	nm_close(d);
	return 0;
}
