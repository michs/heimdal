/*
 * Copyright (c) 2006 - 2007 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

/* $Id$ */

#ifndef NTLM_NTLM_H
#define NTLM_NTLM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <gssapi.h>
#include <gssapi_mech.h>

#include <krb5.h>
#include <roken.h>
#include <heim_threads.h>

#include <heimntlm.h>

#include "crypto-headers.h"

typedef OM_uint32
(*ntlm_interface_init)(OM_uint32 *, void **);

typedef OM_uint32
(*ntlm_interface_destroy)(OM_uint32 *, void *);

typedef OM_uint32
(*ntlm_interface_type2)(OM_uint32 *minor_status,
			void *ctx,
			uint32_t flags,
			const char *hostname,
			const char *domain,
			uint32_t *ret_flags,
			struct ntlm_buf *type2);

typedef OM_uint32
(*ntlm_interface_type3)(OM_uint32 *minor_status,
			void *ctx,
			const struct ntlm_type3 *type3,
			struct ntlm_buf *sessionkey);

typedef void
(*ntlm_interface_free_buffer)(struct ntlm_buf *);

struct ntlm_server_interface {
    ntlm_interface_init nsi_init;
    ntlm_interface_destroy nsi_destroy;
    ntlm_interface_type2 nsi_type2;
    ntlm_interface_type3 nsi_type3;
    ntlm_interface_free_buffer nsi_free_buffer;
};


struct ntlmv2_key {
    uint32_t seq;
    RC4_KEY sealkey;
    RC4_KEY *signsealkey;
    unsigned char signkey[16];
};

extern struct ntlm_server_interface ntlmsspi_kdc_digest;

typedef struct {
    struct ntlm_server_interface *server;
    void *ictx;
    struct {
	char *username;
	struct ntlm_buf key;
    } client;
    OM_uint32 gssflags;
    uint32_t flags;
    uint32_t status;
#define STATUS_OPEN 1
#define STATUS_CLIENT 2
#define STATUS_SESSIONKEY 4
    krb5_data sessionkey;

    union {
	struct {
	    struct {
		uint32_t seq;
		RC4_KEY key;
	    } crypto_send, crypto_recv;
	} v1;
	struct {
	    struct ntlmv2_key send, recv;
	} v2;
    } u;
} *ntlm_ctx;

typedef struct {
    int foo;
} *ntlm_cred;

typedef struct {
    char domain[1];
} *ntlm_name;

#include <ntlm/ntlm-private.h>


#endif /* NTLM_NTLM_H */
