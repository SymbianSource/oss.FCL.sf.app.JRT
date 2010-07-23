/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javajniutils.h"
#include "com_nokia_mj_impl_security_midp_authentication_AuthenticationModule.h"
#include "midpauthenticationmodule.h"
#include "midpauthenticationmoduleimpl.h"
#include "storagehandler.h"
#include "securityutils.h"
#include "telutils.h"
#include "javacertstorehandler.h"
#include "securitycommsmessagedefs.h"
#include "javastorage.h"
#include "javastoragenames.h"
#include "logger.h"

#include <memory>
#include <string>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/asn1.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <string.h>
#include <errno.h>

using namespace java::security;
using namespace java::storage;
using namespace java::util;
using namespace std;

/* The DER encoding of the algorithm ID for the SHA-1 hash function */
const char SHA_1_ALG_FOOTPRINT[] =
    {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14};

/* forward declarations of local/private methods */
static int verify_callback(int, X509_STORE_CTX *);
int getErrCode(int);
int verifyCertChain(char **, int, const unsigned char *, int, vector<string> CAs, char *, char *, CERT_DETAILS*);

JNIEXPORT jobjectArray JNICALL Java_com_nokia_mj_impl_security_midp_authentication_AuthenticationModule__1validateChainsAndSignatures
(JNIEnv * env, jobject, jobjectArray authInfos)
{
    // get the roots from JavaCertStore
    vector<string> CAs;
    JavaCertStoreHandler::retrieveRootsContents(CAs);
    if (CAs.size() == 0)
    {
        SecurityUtils::throw_exception(env, "CONNECTION_TO_CAPTAIN_FAILED");
        return NULL;
    }

    // validate each of the chains&signatures and
    jint len = env->GetArrayLength(authInfos);
    vector<AUTH_CREDENTIALS*> all_auth_credentials;
    all_auth_credentials.reserve(len);
    AUTH_CREDENTIALS * auth_credentials = NULL;
    CERT_DETAILS* details = NULL;
    AUTH_INFO* authInfo = NULL;
    char * jar_hash = NULL;
    char * root_hash = NULL;
    int validation_result = KDefault;
    for (int i=0; i<len; i++)
    {
        // validate the chain
        authInfo = new AUTH_INFO();
        SecurityUtils::getAuthInfo(env, authInfos, i, authInfo);
        jar_hash = new char[2*SHA_1_DIGEST_LEN + 1];
        jar_hash[0] = '\0';
        root_hash = new char[MD5_DIGEST_LEN + 1];
        root_hash[0] = '\0';
        details = new CERT_DETAILS();
        int chain_verification_result = verifyCertChain(
                                            authInfo->cert_chain, authInfo->cert_chain_len,
                                            (const unsigned char *)authInfo->signature,
                                            authInfo->signature_len, CAs, jar_hash,
                                            root_hash, details);
        if (chain_verification_result == KCertAndSignatureOk)
        {
            validation_result = KCertAndSignatureOk;
            auth_credentials = new AUTH_CREDENTIALS();
            auth_credentials->jar_hash = new char[2*SHA_1_DIGEST_LEN + 1];
            auth_credentials->root_hash = new char[MD5_DIGEST_LEN + 1];
            memmove(auth_credentials->jar_hash, jar_hash, 2*SHA_1_DIGEST_LEN + 1);
            memmove(auth_credentials->root_hash, root_hash, MD5_DIGEST_LEN + 1);
            auth_credentials->chain_index = i+1;
            auth_credentials->signing_cert = details;
            all_auth_credentials.push_back(auth_credentials);
            auth_credentials->predefined_domain_category = details->domain_category;
            // the domain info is coming shortly
        }
        else
        {
            delete details;
            details = NULL;
            delete[] jar_hash;
            jar_hash = NULL;
            delete[] root_hash;
            root_hash = NULL;
            // just record the failure of the chain validation
            if (chain_verification_result > validation_result)
            {
                validation_result = chain_verification_result;
            }
        }
        // release the elements
        delete[] authInfo->signature;
        authInfo->signature = NULL;
        for (int j=0 ; j<authInfo->cert_chain_len; j++)
        {
            delete[] authInfo->cert_chain[j];
            authInfo->cert_chain[j] = NULL;
        }
        delete[] authInfo->cert_chain;
        authInfo->cert_chain = NULL;

        // free memory
        delete authInfo;
        authInfo = NULL;
    }
    // release the CAs
    CAs.clear();

    // analyze the result of authentication
    switch (validation_result)
    {
    case KDefault:
        // there were no chains to be validated
        return NULL;
    case KCertValidationFailure:
        SecurityUtils::throw_exception(env, "CERT_VERIFICATION_FAILED");
        return NULL;
    case KSignatureVerificationFailure:
        SecurityUtils::throw_exception(env, "SIG_VERIFICATION_FAILED");
        return NULL;
    case KMissingRoot:
        SecurityUtils::throw_exception(env, "MISSING_ROOT");
        return NULL;
    case KCertNotYetValidFailure:
        SecurityUtils::throw_exception(env, "CERT_NOT_YET_VALID");
        return NULL;
    case KCertExpiredFailure:
        SecurityUtils::throw_exception(env, "CERT_EXPIRED");
        return NULL;
    case KSelfSignedCertInChainFailure:
        SecurityUtils::throw_exception(env, "ROOT_CERT_IN_CHAIN");
        return NULL;
    case KUnknownExtendedKeyUsage:
        SecurityUtils::throw_exception(env, "UNKNOWN_EXT_KEY_USAGE");
        return NULL;
    }

    // try to fill in the domain info for each of the existing credentials
    int i=0;
    int domain_mappings = 0;
    while (i < all_auth_credentials.size())
    {
        // init the domain name and category
        all_auth_credentials[i]->domain_name = NULL;
        all_auth_credentials[i]->domain_category = NULL;
        std::string protection_domain_name;
        std::string protection_domain_category;
        JavaCertStoreHandler::retrieveRootProtDomainInfo(
            all_auth_credentials[i]->root_hash,
            protection_domain_name,
            protection_domain_category);
        if (strcmp(protection_domain_name.c_str(),""))
        {
            // DeveloperCertificates: if domain_category is manufacturer and we have predefined_domain_category use the predefined one
            // Get the domain constants from ApplicationInfo&policy
            if ((strcmp(protection_domain_category.c_str(),"MFD") == 0
                    && all_auth_credentials[i]->predefined_domain_category == DEVCERT_UNKNOWN_DOMAIN)
                    || (strcmp(protection_domain_category.c_str(),"OPD") == 0
                        && all_auth_credentials[i]->predefined_domain_category == DEVCERT_UNKNOWN_DOMAIN))
            {
                i++;
                continue;
            }
            if ((strcmp(protection_domain_category.c_str(),"MFD") == 0
                    && (all_auth_credentials[i]->predefined_domain_category == DEVCERT_OPERATOR_DOMAIN
                        || all_auth_credentials[i]->predefined_domain_category == DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN))
                    || (strcmp(protection_domain_category.c_str(),"OPD") == 0
                        && all_auth_credentials[i]->predefined_domain_category == DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN))
            {
                switch (all_auth_credentials[i]->predefined_domain_category)
                {
                case DEVCERT_OPERATOR_DOMAIN:
                    all_auth_credentials[i]->domain_name = new char[strlen("Operator") + 1];
                    strcpy(all_auth_credentials[i]->domain_name,"Operator");
                    all_auth_credentials[i]->domain_category = new char[strlen("OPD") + 1];
                    strcpy(all_auth_credentials[i]->domain_category,"OPD");
                    break;
                case DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN:
                    all_auth_credentials[i]->domain_name = new char[strlen("IdentifiedThirdParty") + 1];
                    strcpy(all_auth_credentials[i]->domain_name,"IdentifiedThirdParty");
                    all_auth_credentials[i]->domain_category = new char[strlen("ITPD") + 1];
                    strcpy(all_auth_credentials[i]->domain_category,"ITPD");
                    break;
                }
            }
            else
            {
                all_auth_credentials[i]->domain_name = new char[protection_domain_name.size() + 1 /* for the \n */];
                strcpy(all_auth_credentials[i]->domain_name,protection_domain_name.c_str());
                all_auth_credentials[i]->domain_category = new char[protection_domain_category.size() + 1 /* for the \n */];
                strcpy(all_auth_credentials[i]->domain_category,protection_domain_category.c_str());
            }
            domain_mappings++;
        }
        i++;
    }

    // if no protection domain was found -> throw corresponding exception
    if (domain_mappings == 0)
    {
        SecurityUtils::throw_exception(env, "MISSING_DOMAIN_MAPPING");
        return NULL;
    }

    // send the response
    return SecurityUtils::getJNIAuthCredentials(env, all_auth_credentials);
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_security_midp_authentication_AuthenticationModule__1computeHash
(JNIEnv * env, jobject, jstring appJARPath)
{
    jboolean isCopy;
    const char* app_jar_path(env->GetStringUTFChars(appJARPath, &isCopy));
    char * jar_hash_value = SecurityUtils::computeDigest(app_jar_path);
    env->ReleaseStringUTFChars(appJARPath, app_jar_path);
    if (jar_hash_value != NULL)
    {
        jstring hash = env->NewStringUTF(jar_hash_value);
        delete[] jar_hash_value;
        jar_hash_value = NULL;
        return hash;
    }
    else
    {
        if (errno == ENOENT)
        {
            SecurityUtils::throw_exception(env, "JAR_NOT_FOUND");
        }
    }
    return NULL;
}

JNIEXPORT jobject JNICALL Java_com_nokia_mj_impl_security_midp_authentication_AuthenticationModule__1parseCertificate
(JNIEnv * env, jobject, jstring rawCertificate)
{
    int len = env->GetStringLength(rawCertificate);
    jboolean isCopy;
    const char* raw_cert(env->GetStringUTFChars(rawCertificate, &isCopy));
    if (len == 0 || raw_cert == NULL)
    {
        return NULL;
    }
    char * encoded_cert = SecurityUtils::encodePEM(raw_cert, len);
    if (encoded_cert == NULL)
    {
        return NULL;
    }
    env->ReleaseStringUTFChars(rawCertificate, raw_cert);
    X509 * x509_cert = SecurityUtils::readCert(encoded_cert, strlen(encoded_cert), PEM);
    delete[] encoded_cert;
    encoded_cert = NULL;
    if (x509_cert != NULL)
    {
        CERT_DETAILS * details = new CERT_DETAILS();
        SecurityUtils::getCertDetails(*x509_cert, details, false /* don't parse domain info */);
        delete x509_cert;
        x509_cert = NULL;
        jobject cert_details = SecurityUtils::getJNICertDetails(env, *details);
        delete[] details->issuer;
        details->issuer = NULL;
        delete[] details->subject;
        details->subject = NULL;
        delete[] details->organization;
        details->organization = NULL;
        delete[] details->notBefore;
        details->notBefore = NULL;
        delete[] details->notAfter;
        details->notAfter = NULL;
        delete[] details->serial_number;
        details->serial_number = NULL;
        delete[] details->fingerprint;
        details->fingerprint = NULL;
        delete details;
        details = NULL;
        return cert_details;
    }
    return NULL;
}

JNIEXPORT jobject JNICALL Java_com_nokia_mj_impl_security_midp_authentication_AuthenticationModule__1getRootCertificate
(JNIEnv * env, jobject, jstring jRootHash)
{
    // get the certificate content from JavaCertStore
    std::wstring rootHash = JniUtils::jstringToWstring(env, jRootHash);
    std::string rootContent;
    long long len;
    JavaCertStoreHandler::retrieveRootContent(rootHash, &len, rootContent);
    X509* root = SecurityUtils::readCert((char *)rootContent.c_str(), len  , DER);
    if (root != NULL)
    {
        CERT_DETAILS * details = new CERT_DETAILS();
        SecurityUtils::getCertDetails(*root, details, false /* don't parse domain info */);
        delete root;
        root = NULL;
        jobject root_details = SecurityUtils::getJNICertDetails(env, *details);
        delete[] details->issuer;
        details->issuer = NULL;
        delete[] details->subject;
        details->subject = NULL;
        delete[] details->organization;
        details->organization = NULL;
        delete[] details->notBefore;
        details->notBefore = NULL;
        delete[] details->notAfter;
        details->notAfter = NULL;
        delete[] details->serial_number;
        details->serial_number = NULL;
        delete[] details->fingerprint;
        details->fingerprint = NULL;
        delete details;
        details = NULL;
        return root_details;
    }

    // return the prased cert to java
    return NULL;
}

OS_EXPORT void MIDPAuthenticationModuleImpl::getCertChains(
    const Uid& aUid,
    std::list<std::string>& aChains)
{
    auto_ptr<StorageHandler> storageHandler(new StorageHandler());
    list<int> indexes;
    storageHandler->readValidCerts(aUid, indexes);

    JavaStorageEntry attr;
    JavaStorageApplicationEntry_t entry;

    attr.setEntry(ID, aUid.toString());
    entry.insert(attr);

    auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    JavaStorageApplicationList_t foundApps;

    try
    {
        js->open();
        js->search(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry, foundApps);
        js->close();
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaStorage, "CertChains: %s", aJse.toString().c_str());
    }

    entry.clear();

    list<int>::const_iterator iter;

    for (iter = indexes.begin(); iter != indexes.end(); iter++)
    {

        string chain("");
        storageHandler->getChainFromIndex(foundApps, (*iter), chain);

        if (chain.size() > 0)
        {
            aChains.push_back(chain);
        }
    }
}

MIDPAuthenticationModuleImpl::MIDPAuthenticationModuleImpl()
{
}

int verifyCertChain(char **cert_chain, int no_certs,
                    const unsigned char * sig, int sig_len,
                    vector<string> CAs, char * jar_hash,
                    char * root_hash, CERT_DETAILS* details)
{
    X509 *end_entity_cert;
    X509_STORE_CTX *x509_ctx = NULL;
    X509_STORE *x509_store = NULL;
    STACK_OF(X509) *validated_certs_st = sk_X509_new_null();
    STACK_OF(X509) *roots_certs_st = sk_X509_new_null();
    RSA * rsakey = NULL;
    unsigned char * plainSig = NULL;
    int ret_code = KCertAndSignatureOk;

    // Calling OpenSSL_add_all_algorithms() links in all algorithms: as a result a statically linked executable can be quite large. Pick up the supported algorithms only
    OpenSSL_add_all_algorithms();

    // get the end entity certificate
    end_entity_cert = SecurityUtils::readCert(cert_chain[0], strlen(cert_chain[0]), PEM);

    while (true)
    {
        // add certs 1.. into the STACK
        for (int i=1; i<no_certs; i++)
        {
            X509 *x = SecurityUtils::readCert(cert_chain[i], strlen(cert_chain[i]), PEM);
            if (!x)
            {
                ret_code = getErrCode(ERR_get_error());
                break;
            }
            sk_X509_push(validated_certs_st,x);
        }

        // create the cerificate storing object
        x509_store = X509_STORE_new();
        if (!(x509_store))
        {
            ret_code = getErrCode(ERR_get_error());
            break;
        }

        // load the CAs
        for (int i=0; i<CAs.size(); i++)
        {
            X509 *x = SecurityUtils::readCert((char *)CAs[i].c_str(), CAs[i].size(), DER);
            sk_X509_push(roots_certs_st,x);
            X509_STORE_add_cert(x509_store, x);
        }

        // create and initialize X509 vertification context
        x509_ctx = X509_STORE_CTX_new();
        if (!(x509_ctx))
        {
            ret_code = getErrCode(ERR_get_error());
            break;
        }

        if (X509_STORE_CTX_init(x509_ctx, x509_store, end_entity_cert, validated_certs_st)
                != 1)
        {
            ret_code = getErrCode(ERR_get_error());
            break;
        }

        X509_STORE_CTX_set_time(x509_ctx, X509_V_FLAG_USE_CHECK_TIME, TelUtils::getSecureTime());
        // set the callback for validation - needed for the critical extension
        // used by developer certificates
        X509_STORE_CTX_set_verify_cb(x509_ctx, verify_callback);

        // verify certificate
        if (X509_verify_cert(x509_ctx) != 1)
        {
            ret_code = getErrCode(X509_STORE_CTX_get_error(x509_ctx));
            // If the secure time of the device has not yet been set
            // (can happen some times during the first device boot),
            // allow installing with not yet valid certificates
            if (KCertNotYetValidFailure == ret_code)
            {
                if (!TelUtils::isSecureTimeSet())
                {
                    ret_code = KCertAndSignatureOk;
                }
            }

            if (KCertAndSignatureOk != ret_code)
            {
                break;
            }
        }
        // verify the extended key usage: it must point to id-kp-codeSigning (RFC3280 code signing)
        // or 1.3.6.1.4.1.94.1.49.1.2.2.3 (Nokia Java Code Signing Extension)
        EXTENDED_KEY_USAGE *extKeyUsage;
        if ((extKeyUsage=(EXTENDED_KEY_USAGE*)X509_get_ext_d2i(end_entity_cert, NID_ext_key_usage, NULL, NULL)) != NULL)
        {
            bool extKeyUsageKnown = false;
            char EXT_KEY_USAGE_OID[80];
            for (int i = 0; i < sk_ASN1_OBJECT_num(extKeyUsage); i++)
            {
                ASN1_OBJECT *usage = sk_ASN1_OBJECT_value(extKeyUsage,i);
                OBJ_obj2txt(EXT_KEY_USAGE_OID,
                            sizeof(EXT_KEY_USAGE_OID),
                            usage,
                            1 /* use numerical form */);
                if (strcmp(EXT_KEY_USAGE_OID, X509_CODE_SIGNING_OID) == 0
                        || strcmp(EXT_KEY_USAGE_OID, NOKIA_CODE_SIGNING_OID) == 0)
                {
                    extKeyUsageKnown = true;
                    break;
                }
            }
            sk_ASN1_OBJECT_pop_free(extKeyUsage, ASN1_OBJECT_free);
            if (!extKeyUsageKnown)
            {
                ret_code = KUnknownExtendedKeyUsage;
                break;
            }
        }

        // compute the root hash value if requested
        sprintf(root_hash,"%08lX",X509_issuer_name_hash(x509_ctx->current_issuer));
        // add the '\0'
        root_hash[MD5_DIGEST_LEN] = '\0';

        // 1. get the public key of the signing cert
        // 2. decode the provided signature using the signing cert's public key
        // 3. parse the digest/decrypted signature
        EVP_PKEY *pkey = X509_get_pubkey(end_entity_cert);
        rsakey = EVP_PKEY_get1_RSA(pkey);
        EVP_PKEY_free(pkey);
        plainSig = new unsigned char[RSA_size(rsakey) * 2];
        int digest_len = RSA_public_decrypt(sig_len, sig, plainSig, rsakey, RSA_PKCS1_PADDING);
        if (digest_len != (sizeof(SHA_1_ALG_FOOTPRINT) + SHA_1_DIGEST_LEN))
        {
            ret_code = KSignatureVerificationFailure;
            break;
        }
        if (memcmp(SHA_1_ALG_FOOTPRINT,
                   (const char *)plainSig,
                   sizeof(SHA_1_ALG_FOOTPRINT)))
        {
            ret_code = KSignatureVerificationFailure;
            break;
        }
        // save the plainSig into jar_hash
        char * tmp_jar_hash = NULL;
        tmp_jar_hash = jar_hash;
        // format it as hex
        for (int i=0; i<SHA_1_DIGEST_LEN; i++)
        {
            sprintf(tmp_jar_hash, "%02X", *(plainSig + sizeof(SHA_1_ALG_FOOTPRINT) + i));
            tmp_jar_hash = tmp_jar_hash + 2;
        }
        // add the '\0'
        jar_hash[2*SHA_1_DIGEST_LEN] = '\0';
        tmp_jar_hash = NULL;
        break;
    }

    // cleanup
    RSA_free(rsakey);
    delete[] plainSig;
    plainSig = NULL;
    EVP_cleanup();
    if (NULL != x509_ctx)
    {
        X509_STORE_CTX_free(x509_ctx);
    }
    X509_STORE_free(x509_store);
    for (;;)
    {
        X509* x = sk_X509_pop(validated_certs_st);
        if (!x) break;
        X509_free(x);
    }
    sk_X509_free(validated_certs_st);
    for (;;)
    {
        X509* x = sk_X509_pop(roots_certs_st);
        if (!x) break;
        X509_free(x);
    }
    sk_X509_free(roots_certs_st);

    // if things are ok, save the cert_details
    if (end_entity_cert != NULL && ret_code == KCertAndSignatureOk)
    {
        SecurityUtils::getCertDetails(*end_entity_cert, details, true /* parse domain info */);
    }
    X509_free(end_entity_cert);

    return ret_code;
}

static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
{
    int    err;
    err = X509_STORE_CTX_get_error(ctx);
    if (!preverify_ok)
    {
        switch (err)
        {
        case X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION:
            if (SecurityUtils::areAllCriticalExtsKnown(X509_STORE_CTX_get_current_cert(ctx)))
            {
                return !preverify_ok;
            }
            break;
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            return !preverify_ok;
        }
    }
    return preverify_ok;
}

int getErrCode(int x509_err_code)
{
    // Needed to handle other error codes?
    switch (x509_err_code)
    {
    case X509_V_ERR_CERT_NOT_YET_VALID:
        return KCertNotYetValidFailure;
    case X509_V_ERR_CERT_HAS_EXPIRED:
        return KCertExpiredFailure;
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
        return KSelfSignedCertInChainFailure;
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        return KMissingRoot;
    default:
        return KCertValidationFailure;
    }
}
