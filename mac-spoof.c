#include <dlfcn.h>
#include <errno.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

static int (*_orig_ioctl)(int fd, unsigned long request, unsigned char *);

static inline int nibble(const char c) {
	if (c>='0' && c<='9') return c-'0';
	else if (c>='a' && c<='f') return c-'a'+10;
	else if (c>='A' && c<='F') return c-'A'+10;
	else return 0;
}

static int _hook(void *t, const char * const name) {
	void *p = dlsym(RTLD_NEXT, name);
	if (p == NULL) {
		return 0;
	}
	memcpy(t, &p, sizeof(p));
	return 1;
}

__attribute__ ((__constructor__))
static void _init(void) {
	_hook(&_orig_ioctl, "ioctl");
}

int
mac_spoof_ioctl(
	int fd,
	unsigned long request,
	unsigned char *argp
) {
	int ret = _orig_ioctl(fd, request, argp);
	char *p = getenv("MAC_ADDRESS");
	if (request == SIOCGIFHWADDR && ret == 0 && p != NULL) {

		struct ifreq *ifreq = (struct ifreq *)argp;

		if (ifreq->ifr_hwaddr.sa_family != ARPHRD_ETHER) {
			goto cleanup;
		}

		if (strlen(p) != 3 * 6 - 1) {
			errno = EINVAL;
			ret = -1;
			goto cleanup;
		}

		char *ptr = ifreq->ifr_hwaddr.sa_data;
		int i = 0;
		for (i = 0; i < 6; i++) {
			*(ptr++) = (char)(nibble(p[0]) << 4 | nibble(p[1]));
			if (p[2] != '\0' && p[2] != ':') {
				errno = EINVAL;
				ret = -1;
				goto cleanup;
			}
			p += 3;
		}
	}

cleanup:
	return ret;
}
