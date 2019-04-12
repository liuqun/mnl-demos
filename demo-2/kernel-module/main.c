#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/genetlink.h>

#include "uapi/demo2.h"

/* GENL_ID_GENERATE was deprecated since Linux version >= 4.10.0 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)) || !defined(GENL_ID_GENERATE)
#define GENL_ID_GENERATE 0
#endif

static struct genl_family demo_family = {
    .id = GENL_ID_GENERATE,
    .name = DEMO_GENL_NAME,
    .version = DEMO_GENL_VERSION,
    .maxattr = DEMO_CMD_ATTR_MAX,
};

static const struct nla_policy demo_cmd_policy[DEMO_CMD_ATTR_MAX+1] = {
    [DEMO_CMD_ATTR_MESG] = { .type = NLA_STRING },
    [DEMO_CMD_ATTR_DATA] = { .type = NLA_S32 },
};

static int cmd_attr_echo_message(struct genl_info *info)
{
    struct nlattr *na;
    char *msg;
    size_t nbytes; // 包括额外1字节字符串结束符
    struct sk_buff *rep_skb;
    void *begin;
    size_t size;
    int ret;

    /* 读取用户下发的消息 */
    na = info->attrs[DEMO_CMD_ATTR_MESG];
    if (!na) {
        return -EINVAL;
    }

    msg = (char *) nla_data(na);
    pr_info("demo generic netlink receive echo mesg %s\n", msg);

    /* 回发消息 */
    /* 1. 申请新sk_buff */
    nbytes = strlen(msg) + 1;
    size = nla_total_size(nbytes);
    rep_skb = genlmsg_new(size, GFP_KERNEL);
    if (!rep_skb) {
        return -ENOMEM;
    }
    /* 2. 开始填充genetlink消息具体内容 */
    begin = genlmsg_put_reply(rep_skb, info, &demo_family, 0, DEMO_CMD_REPLY);
    if (!begin) {
        pr_info("Error: genlmsg_put_reply() failed\n");
        ret = -ENOMEM;
        goto err;
    }
    ret = nla_put(rep_skb, DEMO_CMD_ATTR_MESG, nbytes, msg);
    if (ret < 0) {
        goto err;
    }
    /* 3. 内容填充完毕 */
    genlmsg_end(rep_skb, begin);
    /* 4. 发送sk_buff */
    ret = genlmsg_reply(rep_skb, info);
    if (ret < 0) {
        goto err;
    }
    return ret;

err:
    kfree_skb(rep_skb);
    return ret;
}

static int cmd_attr_echo_data(struct genl_info *info)
{
    struct nlattr *na;
    s32 data;
    struct sk_buff *rep_skb;
    void *begin;
    size_t size;
    int ret;

    /* 读取用户下发的数据 */
    na = info->attrs[DEMO_CMD_ATTR_DATA];
    if (!na) {
        return -EINVAL;
    }

    data = nla_get_s32(info->attrs[DEMO_CMD_ATTR_DATA]);
    pr_info("demo generic netlink receive echo data %d\n", data);

    /* 回发数据 */
    /* 1. 申请新sk_buff */
    size = nla_total_size(sizeof(s32));
    rep_skb = genlmsg_new(size, GFP_KERNEL);
    if (!rep_skb) {
        return -ENOMEM;
    }
    /* 2. 开始填充genetlink消息具体内容 */
    begin = genlmsg_put_reply(rep_skb, info, &demo_family, 0, DEMO_CMD_REPLY);
    if (!begin) {
        pr_info("Error: genlmsg_put_reply() failed\n");
        ret = -ENOMEM;
        goto err;
    }
    ret = nla_put_s32(rep_skb, DEMO_CMD_ATTR_DATA, data);
    if (ret < 0) {
        goto err;
    }
    /* 3. 内容填充完毕 */
    genlmsg_end(rep_skb, begin);
    /* 4. 发送sk_buff */
    ret = genlmsg_reply(rep_skb, info);
    if (ret < 0) {
        goto err;
    }
    return ret;

err:
    kfree_skb(rep_skb);
    return ret;
}

static int demo_echo_cmd(struct sk_buff *skb, struct genl_info *info)
{
    if (info->attrs[DEMO_CMD_ATTR_MESG]) {
        return cmd_attr_echo_message(info);
    } else if (info->attrs[DEMO_CMD_ATTR_DATA]) {
        return cmd_attr_echo_data(info);
    } else {
        return -EINVAL;
    }
}

static const struct genl_ops demo_ops[] = {
	{
		.cmd = DEMO_CMD_ECHO,
		.doit = demo_echo_cmd,
		.policy = demo_cmd_policy,
		.flags = GENL_ADMIN_PERM,
	},
};

static int __init demo_genetlink_init(void)
{
    int ret;
    pr_info("demo generic netlink module %d init...\n", DEMO_GENL_VERSION);

    demo_family.ops = demo_ops;
    demo_family.n_ops = 1;
    ret = genl_register_family(&demo_family);
    if (ret != 0) {
        pr_info("failed to init demo generic netlink example module\n");
        return ret;
    }

    pr_info("demo generic netlink module init success\n");

    return 0;
}

static void __exit demo_genetlink_exit(void)
{
    int ret;
    printk("demo generic netlink deinit.\n");

    ret = genl_unregister_family(&demo_family);
    if(ret != 0) {
        printk("faled to unregister family:%i\n", ret);
    }
}

module_init( demo_genetlink_init);
module_exit( demo_genetlink_exit);
MODULE_LICENSE("GPL");
