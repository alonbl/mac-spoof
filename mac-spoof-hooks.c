extern
int
mac_spoof_ioctl(
	int fd,
	unsigned long request,
	unsigned char *argp
);
int
ioctl(
	int fd,
	unsigned long request,
	unsigned char *argp
) {
	return mac_spoof_ioctl(fd, request, argp);
}
