/*
 * Copyright (c) 1997 - 2004 Kungliga Tekniska H�gskolan
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

#include "ntlm/ntlm.h"

RCSID("$Id$");

OM_uint32 _gss_ntlm_acquire_cred
           (OM_uint32 * minor_status,
            const gss_name_t desired_name,
            OM_uint32 time_req,
            const gss_OID_set desired_mechs,
            gss_cred_usage_t cred_usage,
            gss_cred_id_t * output_cred_handle,
            gss_OID_set * actual_mechs,
            OM_uint32 * time_rec
           )
{
#if 0
    OM_uint32 major_status;
    krb5_error_code ret;
    ntlm_ctx ctx;
    ntlm_name name = (ntlm_name)desired_name;
#endif

    *minor_status = 0;
    if (output_cred_handle)
	*output_cred_handle = GSS_C_NO_CREDENTIAL;
    if (actual_mechs)
	*actual_mechs = GSS_C_NO_OID_SET;
    if (time_rec)
	*time_rec = GSS_C_INDEFINITE;

#if 0 /* XXX */
    major_status = _gss_ntlm_allocate_ctx(minor_status, &ctx);
    if (major_status != GSS_S_COMPLETE)
	return GSS_S_FAILURE;

    ret = krb5_ntlm_init_request(ctx->context, 
				 ctx->ntlm,
				 NULL,
				 ctx->id,
				 NTLM_NEG_UNICODE|NTLM_NEG_NTLM,
				 name->domain,
				 NULL);
    if (ret) {
	*minor_status = ret;
	return GSS_S_FAILURE;
    }

    {
	gss_ctx_id_t context = (gss_ctx_id_t)ctx;
	_gss_ntlm_delete_sec_context(minor_status, &context, NULL);
	*minor_status = 0;
    }
#endif

    return (GSS_S_COMPLETE);
}
