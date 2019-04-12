#ifdef PLATFORM_CHECK_OK
#include "platform.h"
#endif

/* Based on public domain demo source code.
 * See: https://git.netfilter.org/libmnl/tree/examples/rtnl/rtnl-addr-dump.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include <libmnl/libmnl.h>
#include <linux/if.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>

static int data_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	/* skip unsupported attribute in user-space */
	if (mnl_attr_type_valid(attr, IFA_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case IFA_ADDRESS:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;
	return MNL_CB_OK;
}

static int data_cb(const struct nlmsghdr *nlh, void *data)
{
	struct nlattr *tb[IFA_MAX + 1] = {};
	struct ifaddrmsg *ifa = mnl_nlmsg_get_payload(nlh);

	printf("index=%d family=%d ", ifa->ifa_index, ifa->ifa_family);

	mnl_attr_parse(nlh, sizeof(*ifa), data_attr_cb, tb);
	printf("addr=");
	if (tb[IFA_ADDRESS]) {
		void *addr = mnl_attr_get_payload(tb[IFA_ADDRESS]);
		char out[INET6_ADDRSTRLEN];

		if (inet_ntop(ifa->ifa_family, addr, out, sizeof(out)))
			printf("%s ", out);
	}
	printf("scope=");
	switch(ifa->ifa_scope) {
	case 0:
		printf("global ");
		break;
	case 200:
		printf("site ");
		break;
	case 253:
		printf("link ");
		break;
	case 254:
		printf("host ");
		break;
	case 255:
		printf("nowhere ");
		break;
	default:
		printf("%d ", ifa->ifa_scope);
		break;
	}

	printf("\n");
	return MNL_CB_OK;
}

static int run_foobar_test(struct mnl_socket *sock_ctx, char c_ipv4v6_select)
{
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *header;
	struct rtgenmsg *extra;
	int ret;
	unsigned int seq;
	unsigned int portid;

	header = mnl_nlmsg_put_header(buf);
	header->nlmsg_type = RTM_GETADDR;
	header->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	header->nlmsg_seq = seq = time(NULL);
	extra = mnl_nlmsg_put_extra_header(header, sizeof(*extra));

	extra->rtgen_family = AF_INET;
	if (c_ipv4v6_select == '6') {
		extra->rtgen_family = AF_INET6;
	}

	if (mnl_socket_bind(sock_ctx, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		return(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(sock_ctx);

	if (mnl_socket_sendto(sock_ctx, header, header->nlmsg_len) < 0) {
		perror("mnl_socket_sendto");
		return(EXIT_FAILURE);
	}

	ret = mnl_socket_recvfrom(sock_ctx, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, NULL);
		if (ret <= MNL_CB_STOP) {
			break;
		}
		ret = mnl_socket_recvfrom(sock_ctx, buf, sizeof(buf));
	}
	if (ret == -1) {
		perror("mnl_socket_recvfrom / mnl_cb_run");
		return(EXIT_FAILURE);
	}

	return(EXIT_SUCCESS);
}

typedef struct ctx *ctx_ptr_t;
struct ctx {
	int ipv4v6_selector;
	int mnl_socket_is_opened;
	struct mnl_socket *mnl_ptr; // netlink handler ptr
};

static struct ctx UndefinedContext = {.ipv4v6_selector='4', .mnl_socket_is_opened=0, .mnl_ptr=NULL,};
static void * const UNDEFINED_PTR = (void *) (&UndefinedContext);

/** allocate memory for program context structure */
ctx_ptr_t ctx_malloc(void)
{
	void *ret = UNDEFINED_PTR; // 确保本函数返回的指针是有效指针(避免返回空指针), 配合函数ctx_is_valid(ctx_ptr_t ptr)进行返回值判断
	ctx_ptr_t ptr = NULL;

	ptr = malloc(sizeof(*ptr));
	if (!ptr) {
		// 内存不足?(几乎不可能发生)
		return(UNDEFINED_PTR);
	}
	ret = ptr;
	ptr->ipv4v6_selector = '4';
	ptr->mnl_socket_is_opened = 0;
	return(ret);
}

#include <assert.h>

/** check the context structure (successfully allocated or not) */
int ctx_is_valid(struct ctx *ptr)
{
	assert(ptr != NULL);

	if ((ctx_ptr_t)UNDEFINED_PTR == ptr || !ptr) {
		return(0); // 0代表ctx无效
	}
	return(1); // 1代表ctx有效
}

char ctx_get_ipv4v6_selector(struct ctx *ptr)
{
	assert(ctx_is_valid(ptr));
	return((char) (ptr->ipv4v6_selector & 0x7F));
}

void ctx_set_ipv4v6_selector(struct ctx *ptr, char c_ipv4v6_selector)
{
	assert(ctx_is_valid(ptr));
	ptr->ipv4v6_selector = c_ipv4v6_selector;
}

/** run test */
void ctx_run_my_test(struct ctx *ptr)
{
	assert(ctx_is_valid(ptr));

	if (!ptr->mnl_socket_is_opened) {
		ptr->mnl_ptr = mnl_socket_open(NETLINK_ROUTE);
		if (!ptr->mnl_ptr) {
			return; // 发生错误时,返回的ptr->mnl_socket_is_opened等于0;
		}
		ptr->mnl_socket_is_opened = 1;
	}
	run_foobar_test(ptr->mnl_ptr, ptr->ipv4v6_selector);
	mnl_socket_close(ptr->mnl_ptr);
	ptr->mnl_socket_is_opened = 0;
	return;
}

void ctx_free(ctx_ptr_t ptr)
{
	if (!ptr) {
		return;
	}
	if (ptr->mnl_socket_is_opened) {
		/* Note: 关闭当前已打开的socket文件描述符 */
		mnl_socket_close(ptr->mnl_ptr);
		ptr->mnl_socket_is_opened = 0;
	}
	free(ptr);
}

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
