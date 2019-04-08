# libmnl样例代码
1. 使用libmnl向Linux内核查询各网卡接口的IP地址.
demo-1 目录下的样例代码源自:
- https://git.netfilter.org/libmnl/tree/examples/rtnl/rtnl-addr-dump.c

# 引用的内核头文件
- 头文件[`<uapi/linux/if_addr.h>`](https://github.com/torvalds/linux/blob/v5.0/include/uapi/linux/if_addr.h#L8-L41)
其中包含结构体`struct ifaddrmsg`以及`IFA_ADDRESS`/`IFA_MAX`的定义:
```
struct ifaddrmsg {
	__u8		ifa_family;
	__u8		ifa_prefixlen;	/* The prefix length		*/
	__u8		ifa_flags;	/* Flags			*/
	__u8		ifa_scope;	/* Address scope		*/
	__u32		ifa_index;	/* Link index			*/
};
```
```
enum {
	IFA_UNSPEC,
	IFA_ADDRESS,
	IFA_LOCAL,
	IFA_LABEL,
	IFA_BROADCAST,
	IFA_ANYCAST,
	IFA_CACHEINFO,
	IFA_MULTICAST,
	IFA_FLAGS,
	IFA_RT_PRIORITY,  /* u32, priority/metric for prefix route */
	IFA_TARGET_NETNSID,
	__IFA_MAX,
};


#define IFA_MAX (__IFA_MAX - 1)
```
