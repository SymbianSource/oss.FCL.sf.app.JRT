/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef SECURITYUTILS_H
#define SECURITYUTILS_H

#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include "javajniutils.h"
#include <openssl/sha.h>

namespace java
{
namespace security
{

/*
 * The OID of the certificate extension used to carry the IMEI list information
 * in the Developer Certificates
 */
#define DEVCERT_IMEI_LIST_OID "1.3.6.1.4.1.94.1.49.1.2.2.7"

/*
 * id-kp-codeSigning OID
 */
#define X509_CODE_SIGNING_OID "1.3.6.1.5.5.7.3.3"

/*
 * Nokia Java Code Signing Extension OID
 */
#define NOKIA_CODE_SIGNING_OID "1.3.6.1.4.1.94.1.49.1.2.2.3"

/*
 * The policy identifiers for protection domains. These identifiers are searched
 * into the X.509 certificatePolicies extension
 * ({joint-iso-itu-t(2) ds(5) ce(29) certificatePolicies(32)})
 */
#define DEVCERT_MANUFACTURER_DOMAIN_OID "1.3.6.1.4.1.42.2.110.2.2.2.2"
#define DEVCERT_OPERATOR_DOMAIN_OID "1.3.6.1.4.1.42.2.110.2.2.2.1"
#define DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN_OID "1.3.6.1.4.1.42.2.110.2.2.2.3"

/*
 * Internal constants for the protection domains
 */
const int DEVCERT_ANY_DOMAIN = -1;
const int DEVCERT_UNKNOWN_DOMAIN = 0;
const int DEVCERT_MANUFACTURER_DOMAIN = 1;
const int DEVCERT_OPERATOR_DOMAIN = 2;
const int DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN = 3;

/* The length of the SHA-1 digest (160 bits) */
const int SHA_1_DIGEST_LEN = 20;

/* The length of the MD5 digest (32 digit hexadecimal number) */
const int MD5_DIGEST_LEN = 8;

/* The length of the message chunks used to compute the hash */
const int SHA_1_HASH_CHUNK_LEN = 128*1024;

/* Types of supported certificates */
const int PEM = 1;
const int DER = 2;

typedef struct cert_details_st
{
    char * issuer;
    char * subject;
    char * organization;
    char * notBefore; /* format is YYYYMMDDHHMMSS */
    char * notAfter; /* format is YYYYMMDDHHMMSS */
    char * serial_number;
    char * fingerprint;
    int domain_category;
} CERT_DETAILS;

typedef struct auth_credentials_st
{
    char * domain_name;
    char * domain_category;
    char * jar_hash;
    char * root_hash;
    int chain_index;
    int predefined_domain_category;
    CERT_DETAILS* signing_cert;
} AUTH_CREDENTIALS;

typedef struct auth_info_st
{
    int cert_chain_len;
    char ** cert_chain;
    int signature_len;
    char * signature;
} AUTH_INFO;

class SecurityUtils
{
public:
    static bool areAllCriticalExtsKnown(X509 *);
    static X509 * readCert(const char *, int len, int type);
    static char * encodePEM(const char *, int);
    static void getCertDetails(X509, CERT_DETAILS *, bool);
    static char * computeDigest(const char*);
    static void throw_exception(JNIEnv*, const char *);
    static void getAuthInfo(JNIEnv*, jobjectArray, int, AUTH_INFO *);
    static jobject getJNICertDetails(JNIEnv *, const CERT_DETAILS);
    static jobjectArray getJNIAuthCredentials(JNIEnv *, std::vector<AUTH_CREDENTIALS*>);
private:
    static bool checkIMEI(const X509_EXTENSION *, const char *);
    static char * computeDigest1(const char*);
};

} //end namespace security
} //end namespace java

#endif // SECURITYUTILS_H

