/*
 * scapi.c
 *
 * ASSUMES KMT.
 *
 * Keys are stored in the KMT cache using names derived from the key
 * material itself.  This assumes that if two entities configure the same
 * key, there is no state that carries over in the cache for that key
 * from use to use.  Partial hashes/crypts are not allowed, and IVs must
 * be reset each time.
 *
 * Functions specific to KMT, per se, are listed at the end of the file.
 *
 *
 * XXX	Decide whether to return SNMPERR_* codes, or whether to pass through
 *	KMT_ERR_* codes.  Must be all of one or the other...
 */

#include <config.h>

#include <sys/types.h>
#if HAVE_WINSOCK_H
#include <winsock.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef USE_INTERNAL_MD5
#include "md5.h"
#endif
#include "asn1.h"
#include "tools.h"
#include "snmp_api.h"
#include "snmpusm.h"
#include "keytools.h"
#include "snmp_debug.h"
#include "scapi.h"
#include "callback.h"
#include "snmp_impl.h"
#include "system.h"

#include "transform_oids.h"


#ifdef QUITFUN
#undef QUITFUN
#define QUITFUN(e, l)					\
	if (e != SNMPERR_SUCCESS) {			\
		rval = SNMPERR_SC_GENERAL_FAILURE;	\
		goto l ;				\
	}
#endif


/*
  sc_get_properlength(oid *hashtype, u_int hashtype_len):

  Given a hashing type ("hashtype" and its length hashtype_len), return
  the length of the hash result.

  Returns either the length or SNMPERR_GENERR for an unknown hashing type.
*/
int
sc_get_properlength(oid *hashtype, u_int hashtype_len)
{
  /*
   * Determine transform type hash length.
   */
  if ( ISTRANSFORM(hashtype, HMACMD5Auth)) {
    return BYTESIZE(SNMP_TRANS_AUTHLEN_HMACMD5);
  }
#ifdef HAVE_LIBKMT 
  else if ( ISTRANSFORM(hashtype, HMACSHA1Auth) ) {
    return BYTESIZE(SNMP_TRANS_AUTHLEN_HMACSHA1);
  }
#endif
  return SNMPERR_GENERR;
}

/*
  sc_get_transform_type(oid *hashtype, u_int hashtype_len):

  Given a hashing type ("hashtype" and its length hashtype_len), return
  the transform type (hash library dependent).

  If KMT is being used, it sets up the appropriate hashing function pointer
  as well.

  Returns either the transform type or SNMERR_GENERR for an unknown hashing
  type.
*/
int
sc_get_transform_type(oid *hashtype, u_int hashtype_len,
                      int (**hash_fn)(
                        const int	  mode,		void  	 **context,
                        const u_int8_t	 *data,		const int  data_len,
                        u_int8_t	**digest,	size_t  *digest_len))
{
  /*
   * Determine transform type.
   */
#ifdef HAVE_LIBKMT
  if ( ISTRANSFORM(hashtype, HMACMD5Auth)) {
    if (hash_fn)
      *hash_fn  = kmt_s_md5;
    return KMT_ALG_HMAC_MD5;	

  } else if ( ISTRANSFORM(hashtype, HMACSHA1Auth) ) {
    if (hash_fn)
      *hash_fn  = kmt_s_sha1;
    return  KMT_ALG_HMAC_SHA1;
  }
#else
  if ( ISTRANSFORM(hashtype, HMACMD5Auth)) {
    return INTERNAL_MD5;
  }
#endif                

  if (hash_fn)
    *hash_fn = NULL;
  return SNMPERR_GENERR;
}


/*******************************************************************-o-******
 * sc_init
 *
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_SC_NOT_CONFIGURED	If KMT is not available.
 */
int
sc_init(void)
{
	int		rval = SNMPERR_SUCCESS;

#ifdef HAVE_LIBKMT
	kmt_init();
#else
#ifdef USE_INTERNAL_MD5
        struct timeval tv;

        gettimeofday(&tv,(struct timezone *)0);

        srandom(tv.tv_sec ^ tv.tv_usec);
#else
	rval = SNMPERR_SC_NOT_CONFIGURED;
#endif
#endif

#if		!defined(USE_INTERNAL_MD5) 
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_SHUTDOWN,
                               sc_shutdown, NULL);
#endif		/* !USE_INTERNAL_MD5 */

	return rval;

}  /* end sc_init() */




/*******************************************************************-o-******
 * sc_shutdown
 *
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_SC_NOT_CONFIGURED	If KMT is not available.
 *
 * 
 * kmt_close() is most critical because it deletes (and zeros) all key
 * bits stored in the KMT cache.
 */
int
sc_shutdown(int majorID, int minorID, void *serverarg, void *clientarg) {
	int		rval = SNMPERR_SUCCESS;

#ifdef HAVE_LIBKMT
	kmt_close();
#else

#	ifdef USE_INTERNAL_MD5
		/* EMPTY */
#	else
		rval = SNMPERR_SC_NOT_CONFIGURED;
#	endif
#endif

	return rval;

}  /* end sc_shutdown() */




/*******************************************************************-o-******
 * sc_random
 *
 * Parameters:
 *	*buf		Pre-allocated buffer.
 *	*buflen 	Size of buffer.
 *      
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_SC_GENERAL_FAILURE	Any KMT error.
 */
int
sc_random(u_char *buf, size_t *buflen)
#if defined(HAVE_LIBKMT) || defined(USE_INTERNAL_MD5)
{
	int		rval = SNMPERR_SUCCESS;

#ifdef USE_INTERNAL_MD5
        int i;
        int rndval;
        u_char *ucp = buf;
#endif


#ifdef	HAVE_LIBKMT
	rval = kmt_random(buf, *buflen);
	if (rval < 0) {
		rval = SNMPERR_SC_GENERAL_FAILURE;
	} else {
		*buflen = rval;
		rval = SNMPERR_SUCCESS;
	}

#else	/* USE_INTERNAL_MD5 */
        /* fill the buffer with random integers.  Note that random()
           is defined in config.h and may not be truly the random()
           system call if something better existed */
		rval = *buflen - *buflen%sizeof(rndval);
        for(i = 0; i < rval; i += sizeof(rndval)) {
          rndval = random();
          memcpy(ucp, &rndval, sizeof(rndval));
          ucp += sizeof(rndval);
        }

        rndval = random();
        memcpy(ucp, &rndval, *buflen%sizeof(rndval));

        rval = SNMPERR_SUCCESS;
#endif	/* !HAVE_LIBKMT == USE_INTERNAL_MD5 */

	return rval;

}  /* end sc_random() */

#else
_SCAPI_NOT_CONFIGURED
#endif							/* HAVE_LIBKMT */



/*******************************************************************-o-******
 * sc_generate_keyed_hash
 *
 * Parameters:
 *	 authtype	Type of authentication transform.
 *	 authtypelen
 *	*key		Pointer to key (Kul) to use in keyed hash.
 *	 keylen		Length of key in bytes.
 *	*message	Pointer to the message to hash.
 *	 msglen		Length of the message.
 *	*MAC		Will be returned with allocated bytes containg hash.
 *	*maclen		Length of the hash buffer in bytes; also indicates
 *				whether the MAC should be truncated.
 *      
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_GENERR			All errs, including KMT errors.
 *
 *
 * A hash of the first msglen bytes of message using a keyed hash defined
 * by authtype is created and stored in MAC.  MAC is ASSUMED to be a buffer
 * of at least maclen bytes.  If the length of the hash is greater than
 * maclen, it is truncated to fit the buffer.  If the length of the hash is
 * less than maclen, maclen set to the number of hash bytes generated.
 *
 * ASSUMED that the number of hash bits is a multiple of 8.
 */
int
sc_generate_keyed_hash(	oid	*authtype,	size_t authtypelen,
			u_char	*key,		u_int  keylen,
			u_char	*message,	u_int  msglen,
			u_char	*MAC,		size_t *maclen)
#if defined(HAVE_LIBKMT) || defined(USE_INTERNAL_MD5)
{
	int		 rval	 = SNMPERR_SUCCESS;
	int		 transform,
			 properlength;

	u_int8_t	 buf[SNMP_MAXBUF_SMALL];
#ifdef HAVE_LIBKMT
	int		 buf_len = SNMP_MAXBUF_SMALL;
	u_int8_t	*bufp = buf;
	KMT_KEY_LIST	*kmtkeylist	= NULL;
#endif



#ifdef SNMP_TESTING_CODE
{
	int i;
	DEBUGMSG(("sc_generate_keyed_hash", "sc_generate_keyed_hash(): key=0x"));
        for(i=0; i< keylen; i++)
          DEBUGMSG(("sc_generate_keyed_hash", "%02x", key[i] & 0xff));
	DEBUGMSG(("sc_generate_keyed_hash"," (%d)\n", keylen));
}
#endif /* SNMP_TESTING_CODE */

	/*
	 * Sanity check.
	 */
	if ( !authtype || !key || !message || !MAC || !maclen
		|| (keylen<=0) || (msglen<=0) || (*maclen<=0)
		|| (authtypelen != USM_LENGTH_OID_TRANSFORM) )
	{
		QUITFUN(SNMPERR_GENERR, sc_generate_keyed_hash_quit);
	}


	/*
	 * Determine transform type.
	 */
        transform = sc_get_transform_type(authtype, authtypelen, &kmt_hash);
        if (transform == SNMPERR_GENERR)
          return transform;

        properlength = sc_get_properlength(authtype, authtypelen);
        if (properlength == SNMPERR_GENERR)
          return properlength;

	if ( ((int)keylen < properlength) ) {
		QUITFUN(SNMPERR_GENERR, sc_generate_keyed_hash_quit);
	}


#ifdef HAVE_LIBKMT
	/*
	 * Lookup key in KMT.
	 * Perform the keyed hash over message.
	 * Store the result in MAC, possibly truncating the result.
	 */
	rval = sc_internal_kmtlookup(	transform,
					key, keylen, properlength,
					&kmtkeylist, TRUE);
	QUITFUN(rval, sc_generate_keyed_hash_quit);
		
	rval = kmt_sign_data(	KMT_CRYPT_MODE_ALL,
				kmt_keylist_key(kmtkeylist), NULL,
				message, msglen,
				&bufp, &buf_len);
	QUITFUN(rval, sc_generate_keyed_hash_quit);

	if (*maclen > buf_len) {
		*maclen = buf_len;
	}
	memcpy(MAC, buf, *maclen);

#else /* ! HAVE_LIBKMT */

        if ((int)*maclen > properlength)
          *maclen = properlength;
        if (MDsign(message, msglen, MAC, *maclen, key, keylen)) {
            rval = SNMPERR_GENERR;
            goto sc_generate_keyed_hash_quit;
        }

#endif /* ! HAVE_LIBKMT */

#ifdef SNMP_TESTING_CODE
{
	char    *s;
	int      len = binary_to_hex(MAC, *maclen, &s);

	DEBUGMSGTL(("scapi","Full v3 message hash: %s\n", s));
	SNMP_ZERO(s, len);
	SNMP_FREE(s);
}
#endif
        
sc_generate_keyed_hash_quit:
#ifdef HAVE_LIBKMT
	kmt_release_keylist(&kmtkeylist);
#endif
	SNMP_ZERO(buf, SNMP_MAXBUF_SMALL);
	return rval;

}  /* end sc_generate_keyed_hash() */

#else
_SCAPI_NOT_CONFIGURED
#endif							/* HAVE_LIBKMT */


/* sc_hash(): a generic wrapper around whatever hashing package we are using.

   IN:
     hashtype    - oid pointer to a hash type
     hashtypelen - length of oid pointer
     buf         - u_char buffer to be hashed
     buf_len     - integer length of buf data
     MAC_len     - length of the passed MAC buffer size.
    
   OUT:    
     MAC         - pre-malloced space to store hash output.
     MAC_len     - length of MAC output to the MAC buffer.

   Returns:
     SNMPERR_SUCCESS		Success.
     SNMP_SC_GENERAL_FAILURE	Any error.
*/

#if defined(HAVE_LIBKMT) || defined(USE_INTERNAL_MD5)
int
sc_hash(oid *hashtype, size_t hashtypelen, u_char *buf, size_t buf_len,
        u_char *MAC, size_t *MAC_len)
{


#ifdef HAVE_LIBKMT
  int   rval       = SNMPERR_SUCCESS;
  void *context    = NULL;
#endif

  /*
   * Determine transform type.
   */

  int transform = sc_get_transform_type(hashtype, hashtypelen, &kmt_hash);
  if (transform == SNMPERR_GENERR)
    return transform;

#if defined(HAVE_LIBKMT)

  rval = kmt_hash(KMT_CRYPT_MODE_ALL, &context, buf, buf_len, &MAC, MAC_len);

  if (rval != SNMPERR_SUCCESS) {
    return SNMPERR_SC_GENERAL_FAILURE;
  }

  return rval;

#else /* USE_INTERNAL_MD5 */

  if (MDchecksum(buf, buf_len, MAC, *MAC_len)) {
    return SNMPERR_GENERR;
  }
  if (*MAC_len > 16)
    *MAC_len = 16;
  return SNMPERR_SUCCESS;

#endif
}
#else /* !defined(HAVE_LIBKMT) && !defined(USE_INTERNAL_MD5) */
_SCAPI_NOT_CONFIGURED
#endif /* !defined(HAVE_LIBKMT) && !defined(USE_INTERNAL_MD5) */



/*******************************************************************-o-******
 * sc_check_keyed_hash
 *
 * Parameters:
 *	 authtype	Transform type of authentication hash.
 *	*key		Key bits in a string of bytes.
 *	 keylen		Length of key in bytes.
 *	*message	Message for which to check the hash.
 *	 msglen		Length of message.
 *	*MAC		Given hash.
 *	 maclen		Length of given hash; indicates truncation if it is
 *				shorter than the normal size of output for
 *				given hash transform.
 * Returns:
 *	SNMPERR_SUCCESS		Success.
 *	SNMP_SC_GENERAL_FAILURE	Any error, including KMT errs.
 *
 *
 * Check the hash given in MAC against the hash of message.  If the length
 * of MAC is less than the length of the transform hash output, only maclen
 * bytes are compared.  The length of MAC cannot be greater than the
 * length of the hash transform output.
 */
int
sc_check_keyed_hash(	oid	*authtype,	size_t authtypelen,
			u_char	*key,		u_int keylen,
			u_char	*message,	u_int msglen,
			u_char	*MAC,		u_int maclen)
#if defined(USE_INTERNAL_MD5) || defined(HAVE_LIBKMT)
{
	int		 rval	 = SNMPERR_SUCCESS;
	size_t		 buf_len = SNMP_MAXBUF_SMALL;

	u_int8_t	 buf[SNMP_MAXBUF_SMALL];



#ifdef SNMP_TESTING_CODE
{
 int i;
 DEBUGMSG(("scapi", "sc_check_keyed_hash():    key=0x"));
 for(i=0; i< keylen; i++)
   DEBUGMSG(("scapi", "%02x", key[i] & 0xff));
 DEBUGMSG(("scapi"," (%d)\n", keylen));
}
#endif /* SNMP_TESTING_CODE */

	/*
	 * Sanity check.
	 */
	if ( !authtype || !key || !message || !MAC 
		|| (keylen<=0) || (msglen<=0) || (maclen<=0)
		|| (authtypelen != USM_LENGTH_OID_TRANSFORM) )
	{
		QUITFUN(SNMPERR_GENERR, sc_check_keyed_hash_quit);
	}


	/* 
	 * Generate a full hash of the message, then compare
	 * the result with the given MAC which may shorter than
	 * the full hash length.
	 */
	rval = sc_generate_keyed_hash(	authtype, authtypelen,
					key, keylen,
					message, msglen,
					buf, &buf_len);
	QUITFUN(rval, sc_check_keyed_hash_quit);

	if (maclen > msglen) {
		QUITFUN(SNMPERR_GENERR, sc_check_keyed_hash_quit);

	} else if ( memcmp(buf, MAC, maclen) != 0 ) {
		QUITFUN(SNMPERR_GENERR, sc_check_keyed_hash_quit);
	}


sc_check_keyed_hash_quit:
	SNMP_ZERO(buf, SNMP_MAXBUF_SMALL);

	return rval;

}  /* end sc_check_keyed_hash() */

#else
_SCAPI_NOT_CONFIGURED
#endif	/* HAVE_LIBKMT || USE_INTERNAL_MD5 */



/*******************************************************************-o-******
 * sc_encrypt
 *
 * Parameters:
 *	 privtype	Type of privacy cryptographic transform.
 *	*key		Key bits for crypting.
 *	 keylen		Length of key (buffer) in bytes.
 *	*iv		IV bits for crypting.
 *	 ivlen		Length of iv (buffer) in bytes.
 *	*plaintext	Plaintext to crypt.
 *	 ptlen		Length of plaintext.
 *	*ciphertext	Ciphertext to crypt.
 *	*ctlen		Length of ciphertext.
 *      
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_SC_NOT_CONFIGURED	Encryption is not supported.
 *	SNMPERR_SC_GENERAL_FAILURE	Any other error, including KMT errs.
 *
 *
 * Encrypt plaintext into ciphertext using key and iv.
 *
 * ctlen contains actual number of crypted bytes in ciphertext upon
 * successful return.
 */
int
sc_encrypt(	oid    *privtype,	size_t privtypelen,
		u_char *key,		u_int  keylen,
		u_char *iv,		u_int  ivlen,
		u_char *plaintext,	u_int  ptlen,
		u_char *ciphertext,	size_t *ctlen)
#ifdef								HAVE_LIBKMT
{
	int		rval	= SNMPERR_SUCCESS;
	u_int		transform,
			properlength,
			properlength_iv;

	KMT_KEY_LIST	*kmtkeylist = NULL;



	/*
	 * Sanity check.
	 */
#if	!defined(SCAPI_AUTHPRIV)
		return SNMPERR_SC_NOT_CONFIGURED;
#endif

	if ( !privtype || !key || !iv || !plaintext || !ciphertext || !ctlen
		|| (keylen<=0) || (ivlen<=0) || (ptlen<=0) || (*ctlen<=0)
		|| (privtypelen != USM_LENGTH_OID_TRANSFORM) )
	{
		QUITFUN(SNMPERR_GENERR, sc_encrypt_quit);
	}


#ifdef SNMP_TESTING_CODE
{
        char buf[SNMP_MAXBUF];

	sprint_hexstring(buf, iv, ivlen);
        DEBUGMSGTL(("scapi", "encrypt: IV: %s/ ", buf));
	sprint_hexstring(buf, key, keylen);
        DEBUGMSG(("scapi","%s\n", buf));

	sprint_hexstring(buf, plaintext, 16);
        DEBUGMSGTL(("scapi","encrypt: string: %s\n", buf));
}
#endif /* SNMP_TESTING_CODE */


	/*
	 * Determine privacy transform.
	 */
	if ( ISTRANSFORM(privtype, DESPriv) ) {
		transform	= KMT_ALG_DES;	
		properlength	= BYTESIZE(SNMP_TRANS_PRIVLEN_1DES);
		properlength_iv	= BYTESIZE(SNMP_TRANS_PRIVLEN_1DES_IV);

	} else {
		QUITFUN(SNMPERR_GENERR, sc_encrypt_quit);
	}

	if ( (keylen<properlength) || (ivlen<properlength_iv) ) {
		QUITFUN(SNMPERR_GENERR, sc_encrypt_quit);
	}


	/*
	 * Lookup key in KMT.
	 * Encrypt plaintext into ciphertext.
	 */
	rval = sc_internal_kmtlookup(	transform,
					key, keylen, properlength,
					&kmtkeylist, TRUE);
	QUITFUN(rval, sc_encrypt_quit);

	rval = kmt_encrypt_data(KMT_CRYPT_MODE_ALL,
				kmt_keylist_key(kmtkeylist), NULL,
				plaintext, ptlen,
				&ciphertext, ctlen,
                                iv);
	QUITFUN(rval, sc_encrypt_quit);


sc_encrypt_quit:
	kmt_release_keylist(&kmtkeylist);
	return rval;

}  /* end sc_encrypt() */

#else
#	if USE_INTERNAL_MD5
	{
		DEBUGMSGTL(("scapi","Encrypt function not defined.\n"));
		return SNMPERR_SC_GENERAL_FAILURE;
	}

#	else
		_SCAPI_NOT_CONFIGURED

#	endif /* USE_INTERNAL_MD5 */
#endif							/* HAVE_LIBKMT */



/*******************************************************************-o-******
 * sc_decrypt
 *
 * Parameters:
 *	 privtype
 *	*key
 *	 keylen
 *	*iv
 *	 ivlen
 *	*ciphertext
 *	 ctlen
 *	*plaintext
 *	*ptlen
 *      
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_SC_NOT_CONFIGURED	Encryption is not supported.
 *      SNMPERR_SC_GENERAL_FAILURE      Any other error, including KMT errs.
 *
 *
 * Decrypt ciphertext into plaintext using key and iv.
 *
 * ptlen contains actual number of plaintext bytes in plaintext upon
 * successful return.
 */
int
sc_decrypt(	oid    *privtype,	size_t privtypelen,
		u_char *key,		u_int  keylen,
		u_char *iv,		u_int  ivlen,
		u_char *ciphertext,	u_int  ctlen,
		u_char *plaintext,	size_t *ptlen)
#ifdef								HAVE_LIBKMT
{
	int		rval	= SNMPERR_SUCCESS;
	u_int		transform,
			properlength,
			properlength_iv;

	KMT_KEY_LIST	*kmtkeylist = NULL;



	/*
	 * Sanity check.
	 */
#if	!defined(SCAPI_AUTHPRIV)
		return SNMPERR_SC_NOT_CONFIGURED;
#endif

	if ( !privtype || !key || !iv || !plaintext || !ciphertext || !ptlen
		|| (ctlen<=0) || (*ptlen<=0)
		|| (privtypelen != USM_LENGTH_OID_TRANSFORM) )
	{
		QUITFUN(SNMPERR_GENERR, sc_decrypt_quit);
	}


#ifdef SNMP_TESTING_CODE
{
        char buf[SNMP_MAXBUF];

	sprint_hexstring(buf, iv, ivlen);
        DEBUGMSGTL(("scapi", "decrypt: IV: %s/ ", buf));
	sprint_hexstring(buf, key, keylen);
        DEBUGMSG(("scapi","%s\n", buf));
}
#endif /* SNMP_TESTING_CODE */


	/*
	 * Determine privacy transform.
	 */
	if ( ISTRANSFORM(privtype, DESPriv) ) {
		transform	= KMT_ALG_DES;	
		properlength	= BYTESIZE(SNMP_TRANS_PRIVLEN_1DES);
		properlength_iv	= BYTESIZE(SNMP_TRANS_PRIVLEN_1DES_IV);

	} else {
		QUITFUN(SNMPERR_GENERR, sc_decrypt_quit);
	}

	if ( (keylen<properlength) || (ivlen<properlength_iv) ) {
		QUITFUN(SNMPERR_GENERR, sc_decrypt_quit);
	}


	/*
	 * Lookup key in KMT.
	 * Decrypt ciphertext into plaintext.
	 */
	rval = sc_internal_kmtlookup(	transform,
					key, keylen, properlength,
					&kmtkeylist, TRUE);
	QUITFUN(rval, sc_decrypt_quit);

	rval = kmt_decrypt_data(KMT_CRYPT_MODE_ALL,
				kmt_keylist_key(kmtkeylist), NULL,
				ciphertext, ctlen,
				&plaintext, ptlen,
				iv);

	QUITFUN(rval, sc_decrypt_quit);


sc_decrypt_quit:
	kmt_release_keylist(&kmtkeylist);
	return rval;

}  /* end sc_decrypt() */

#else
#	if USE_INTERNAL_MD5
	{
		DEBUGMSGTL(("scapi","Decryption function not defined.\n"));
		return SNMPERR_SC_GENERAL_FAILURE;
	}

#	else
		_SCAPI_NOT_CONFIGURED

#	endif /* USE_INTERNAL_MD5 */
#endif							/* HAVE_LIBKMT */

	

#ifdef								HAVE_LIBKMT

/*******************************************************************-o-******
 * sc_internal_kmtlookup
 *
 * Parameters:
 *	  transform		Cryptographic algorithm associated with key.
 *	 *key			Key bits in a string of bytes.
 *	  keylen		Number of bytes in key.
 *	  properlen		Number of bytes in a properly formed key.
 *	**kmtkeylist		Hook to return KMT_KEY_LIST with key entry.
 *	  dospecify		TRUE if a new key should be added to KMT cache.
 *      
 * Returns:
 *	SNMPERR_SUCCESS			Success.
 *	SNMPERR_SC_GENERAL_FAILURE	Any err, including KMT errors.
 *
 * Lookup the key in the KMT cache.
 * Add a new key if the requested key is not found.
 * Always returns one or 0 keys.
 *
 * NOTE  Default "mode" is proper for all currently known SNMPv3 algorithms.
 * NOTE  Identical keys may be instantiated with different transforms.
 *
 * ASSUME	No transform has more than one mode of use in the SNMP context.
 */
int
sc_internal_kmtlookup(	u_int 	 transform, 
			u_char	*key,		u_int		  keylen,
			u_int	 properlength,	KMT_KEY_LIST	**kmtkeylist,
			int	 dospecify)
{
	int	 rval		 = SNMPERR_SUCCESS,
		 tmode		 = 0,			/* KMT default. */
		 keyname_len;
	char	*keyname	 = NULL;

	KMT_ATTRIBUTE	kmt_attr;
        kmt_attr.type = KMT_ATTR_ALG;
        kmt_attr.value = (int) transform;



	/*
	 * Sanity check.
	 */
	if ( !key || !kmtkeylist || (keylen <= 0) || (properlength <= 0) ) {
		return SNMPERR_SC_GENERAL_FAILURE;
	}


	*kmtkeylist = NULL;
	keyname_len = binary_to_hex(key, keylen, &keyname);

	rval = kmt_get_keylist_from_cache(
				kmtkeylist, keyname, &kmt_attr, 1);

	if (dospecify && rval != KMT_ERR_SUCCESS) {
		rval = kmt_specify_key(	keyname, properlength * 8,
					transform, tmode,
					NULL, key, keylen,
					kmtkeylist);

		if (rval != KMT_ERR_SUCCESS) {
			rval = SNMPERR_SC_GENERAL_FAILURE;

		} else {
			kmt_set_expiry(kmt_keylist_key(*kmtkeylist), 0, 0);
		}
	}

        free_zero(keyname, keyname_len);

	return rval;

}  /* end sc_internal_kmtlookup() */

#endif							/* HAVE_LIBKMT */

