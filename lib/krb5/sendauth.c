/*
 * Copyright (c) 1997, 1998 Kungliga Tekniska H�gskolan
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
 * 3. All advertising materials mentioning features or use of this software 
 *    must display the following acknowledgement: 
 *      This product includes software developed by Kungliga Tekniska 
 *      H�gskolan and its contributors. 
 *
 * 4. Neither the name of the Institute nor the names of its contributors 
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

#include "krb5_locl.h"

RCSID("$Id$");

/*
 * The format seems to be:
 * client -> server
 *
 * 4 bytes - length
 * KRB5_SENDAUTH_V1.0 (including zero)
 * 4 bytes - length
 * protocol string (with terminating zero)
 *
 * server -> client
 * 1 byte - (0 = OK, else some kind of error)
 *
 * client -> server
 * 4 bytes - length
 * AP-REQ
 *
 * server -> client
 * 4 bytes - length (0 = OK, else length of error)
 * (error)
 *
 * if(mutual) {
 * server -> client
 * 4 bytes - length
 * AP-REP
 * }
 */

krb5_error_code
krb5_sendauth(krb5_context context,
	      krb5_auth_context *auth_context,
	      krb5_pointer p_fd,
	      const char *appl_version,
	      krb5_principal client,
	      krb5_principal server,
	      krb5_flags ap_req_options,
	      krb5_data *in_data,
	      krb5_creds *in_creds,
	      krb5_ccache ccache,
	      /*krb5_error*/ void **error,
	      krb5_ap_rep_enc_part **rep_result,
	      krb5_creds **out_creds)
{
    krb5_error_code ret;
    u_int32_t len, net_len;
    const char *version = KRB5_SENDAUTH_VERSION;
    u_char repl;
    krb5_data ap_req, error_data;
    krb5_creds this_cred;
    krb5_principal this_client = NULL;
    krb5_creds *creds;

    len = strlen(version) + 1;
    net_len = htonl(len);
    if (krb5_net_write (context, p_fd, &net_len, 4) != 4
	|| krb5_net_write (context, p_fd, version, len) != len)
	return errno;

    len = strlen(appl_version) + 1;
    net_len = htonl(len);
    if (krb5_net_write (context, p_fd, &net_len, 4) != 4
	|| krb5_net_write (context, p_fd, appl_version, len) != len)
	return errno;

    if (krb5_net_read (context, p_fd, &repl, sizeof(repl)) != sizeof(repl))
	return errno;

    if (repl != 0)
	return KRB5_SENDAUTH_BADRESPONSE; /* XXX */

    if (in_creds == NULL) {
	if (client == NULL) {
	    ret = krb5_cc_get_principal (context, ccache, &this_client);
	    if (ret)
		return ret;
	    client = this_client;
	}
	memset(&this_cred, 0, sizeof(this_cred));
	this_cred.client = client;
	this_cred.server = server;
	this_cred.times.endtime = 0;
	this_cred.ticket.length = 0;
	in_creds = &this_cred;
    }
    if (in_creds->ticket.length == 0) {
	ret = krb5_get_credentials (context, 0, ccache, in_creds, &creds);
	if (ret)
	    return ret;
    } else {
	creds = in_creds;
    }
    ret = krb5_mk_req_extended (context,
				auth_context,
				ap_req_options,
				in_data,
				creds,
				&ap_req);

    if (out_creds)
	*out_creds = creds;
    else
	krb5_free_creds(context, creds);
    if(this_client)
	krb5_free_principal(context, this_client);

    if (ret)
	return ret;

    ret = krb5_write_message (context,
			      p_fd,
			      &ap_req);
    if (ret)
	return ret;

    krb5_data_free (&ap_req);

    ret = krb5_read_message (context, p_fd, &error_data);
    if (ret)
	return ret;

    if (error_data.length != 0) {
	KRB_ERROR error;

	ret = krb5_rd_error (context, &error_data, &error);
	krb5_data_free (&error_data);
	if (ret == 0) {
	    free_KRB_ERROR(&error);
	    return error.error_code;
	} else
	    return ret;
    }

    if (ap_req_options & AP_OPTS_MUTUAL_REQUIRED) {
	krb5_data ap_rep;
	krb5_ap_rep_enc_part *ignore;

	krb5_data_zero (&ap_rep);
	ret = krb5_read_message (context,
				 p_fd,
				 &ap_rep);
	if (ret)
	    return ret;

	ret = krb5_rd_rep (context, *auth_context, &ap_rep,
			   rep_result ? rep_result : &ignore);
	if (ret)
	    return ret;
	if (rep_result == NULL)
	    krb5_free_ap_rep_enc_part (context, ignore);
	krb5_data_free (&ap_rep);
    }
    return 0;
}
