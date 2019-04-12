#ifdef PLATFORM_CHECK_OK
#include "platform.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctx.h"

static int dump_current_ip_addresses(char c_ipv4v6_selector)
{
	ctx_ptr_t ctx;

	ctx = ctx_malloc();
	if (!ctx_is_valid(ctx)) {
		return(-1);
	}
	ctx_set_ipv4v6_selector(ctx, c_ipv4v6_selector);
	ctx_run_my_test(ctx);
	ctx_free(ctx);
	ctx = NULL;
	return (0);
}

int main(int argc, char *argv[])
{
	char c_ipv4v6;
	int err_code;

	if (argc > 2) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		fprintf(stderr, "       %s -4\n", argv[0]);
		fprintf(stderr, "       %s -6\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	c_ipv4v6 = '4';
	if (argc == 2 && strcmp(argv[1], "-6") == 0) {
		c_ipv4v6 = '6';
	}

	dump_current_ip_addresses(c_ipv4v6);
	return 0;
}
