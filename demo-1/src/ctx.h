// Copyright 2019, Qingdao Xin-Fan-Shi Information Technology Co. Ltd.

#pragma once

struct ctx;
typedef struct ctx *ctx_ptr_t;

/** allocate memory for program context structure */
ctx_ptr_t ctx_malloc(void);
void ctx_free(ctx_ptr_t);

/** check the context structure (successfully allocated or not) */
int ctx_is_valid(ctx_ptr_t);

/** run test */
void ctx_run_my_test(ctx_ptr_t);

/** select IPv4/IPv6 using c_ipv4v6_selector='4' or '6' */
void ctx_set_ipv4v6_selector(ctx_ptr_t, char c_ipv4v6_selector);
char ctx_get_ipv4v6_selector(ctx_ptr_t);

// /**************/
// /* Demo usage */
// /**************/
//
// #include "ctx.h"
//
// int dump_current_ipv4v6_addresses()
// {
//     ctx_ptr_t ctx;
//
//     ctx = ctx_malloc();
//     if (!ctx_is_valid(ctx)) {
//         return(-1);
//     }
//     ctx_set_ipv4v6_selector(ctx, '4');
//     ctx_run_my_test(ctx);
//     ctx_set_ipv4v6_selector(ctx, '6');
//     ctx_run_my_test(ctx);
//     ctx_free(ctx);
//     return(0);
// }
