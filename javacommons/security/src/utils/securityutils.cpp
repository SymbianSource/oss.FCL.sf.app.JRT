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
#include <string.h>
#include "securityutils.h"
#include "telutils.h"
#include "fileutils.h"
#include "com_nokia_mj_impl_security_utils_TelUtils.h"

using namespace java::security;
using namespace std;

JNIEXPORT jobject JNICALL Java_com_nokia_mj_impl_security_utils_TelUtils__1getNetworkCodes
(JNIEnv * env, jclass)
{
    std::string mcc;
    std::string mnc;
    TelUtils * telUtils = TelUtils::createInstance();
    telUtils->getNetworkCodes(mcc, mnc);
    delete telUtils;
    telUtils = NULL;
    jclass network_codes_class = env->FindClass(
                                     "com/nokia/mj/impl/security/utils/TelUtils$NetworkCodes");
    jmethodID network_codes_cid = env->GetMethodID(network_codes_class,"<init>",
                                  "(Ljava/lang/String;Ljava/lang/String;)V");
    jstring jmcc = env->NewStringUTF(mcc.c_str());
    jstring jmnc = env->NewStringUTF(mnc.c_str());
    jobject network_codes = env->NewObject(
                                network_codes_class,
                                network_codes_cid,
                                jmcc,jmnc);
    return network_codes;
}

// Checks if all critical extensions are known. If true and developer
// certificates extensions are found as well, it handles them as following:
//  - get the list of allowed IMEIs from signing certificate extension
//  - get my IMEI
//      - check if my IMEI is among the allowed ones
//      - get the protection domain binding from signing certificate extension
//      - check that protection domain is valid (manufacturer || operator || trusted third party)
//      - return the MIDlet to the protection domain
bool SecurityUtils::areAllCriticalExtsKnown(X509 *cert)
{
    char EXT_OID[80];
    X509_EXTENSION *ext;
    int extCount = X509_get_ext_count(cert);
    for (int i = 1; i<= extCount; i++)
    {
        ext = X509_get_ext(cert, i);
        if (X509_EXTENSION_get_critical(ext)
                && !X509_supported_extension(ext))
        {
            // check if the extension is known
            OBJ_obj2txt(
                EXT_OID,
                sizeof(EXT_OID),
                X509_EXTENSION_get_object(ext),
                1 /* use numerical form */);
            if (!strcmp(EXT_OID, DEVCERT_IMEI_LIST_OID) == 0)
            {
                return false;
            }
            // compare my IMEI against the list of IMEI from the certificate extension
            std::string imei;
            TelUtils * telUtils = TelUtils::createInstance();
            telUtils->getImei(imei);
            delete telUtils;
            telUtils = NULL;
            return checkIMEI(ext, imei.c_str());
        }
    }
    return true;
}

X509 * SecurityUtils::readCert(const char * cert, int len, int type)
{
    X509 *x = NULL;
    BIO *bio;
    bio = BIO_new_mem_buf((void *) cert, len);
    if ((bio))
    {
        BIO_set_close(bio, BIO_CLOSE);
        switch (type)
        {
        case PEM:
            x = PEM_read_bio_X509(bio, 0, 0, 0);
            break;
        case DER:
            x = d2i_X509_bio(bio,NULL);
            break;
        }
        BIO_free(bio);
    }
    return x;
}

char * SecurityUtils::encodePEM(const char *str, int len)
{
    const char PEM_PREFIX[] = "-----BEGIN CERTIFICATE-----\n";
    const char PEM_SUFFIX[] = "\n-----END CERTIFICATE-----\n";

    // wrap the lines at 64 characters and add the header and footer
    // lines (required by OpenSSL)
    // if the length is multiple of 64 don't add the last \n
    int prefixLen = sizeof(PEM_PREFIX);
    int suffixLen = sizeof(PEM_SUFFIX);
    int new_len = len + prefixLen
                  + suffixLen + (len/64) /* wrap lines */
                  + 1 /* the terminating null separator */;
    if (len%64 == 0)
    {
        new_len = new_len - 1;
    }
    char * encStr = new char[ new_len ];
    // add the prefix
    strncpy(encStr, PEM_PREFIX, prefixLen);
    // add the actual string
    int i=0;
    int k = prefixLen - 1;
    while (i<len)
    {
        encStr[k] = str[i];
        k++;
        if ((i + 1)%64 == 0 && i != (len - 1))
        {
            // insert a \n
            encStr[k] = '\n';
            k++;
        }
        i++;
    }
    encStr[k] = '\0';
    // add the suffix
    strcat(encStr, PEM_SUFFIX);
    // add the null terminator
    encStr[new_len - 1] = '\0';
    return encStr;
}

char * SecurityUtils::computeDigest1(const char* jarFileName)
{
    FILE    *jarFile;
    jarFile = fopen(jarFileName, "r");
    if (jarFile == NULL)
    {
        return NULL;
    }
    // figure out the len of the file
    unsigned long len;
    fseek(jarFile, 0L, SEEK_END);
    len = ftell(jarFile);
    fseek(jarFile, 0L, SEEK_SET);
    // if the size of the file is less than the size of the chunks,
    // then do the hash calculation in on go
    unsigned char * computed_digest = new unsigned char[SHA_1_DIGEST_LEN];
    unsigned char * buf = NULL;
    if (len > 0 && len <= SHA_1_HASH_CHUNK_LEN)
    {
        // do the hash calculation in one go
        buf = new unsigned char[len];
        len = fread(buf, sizeof(unsigned char), len, jarFile);
        if (ferror(jarFile))
        {
            // stop right here
            fclose(jarFile);
            delete[] buf;
            buf = NULL;
            delete[] computed_digest;
            computed_digest = NULL;
            return NULL;
        }
        fclose(jarFile);
        SHA1(buf, len, computed_digest);
        delete[] buf;
        buf = NULL;
    }
    else
    {
        // do the hash calculation in chunks
        SHA_CTX c;
        SHA1_Init(&c);
        buf = new unsigned char[SHA_1_HASH_CHUNK_LEN];
        while ((len = fread(buf, sizeof(unsigned char), SHA_1_HASH_CHUNK_LEN, jarFile)) > 0
                && !feof(jarFile) && !ferror(jarFile))
        {
            SHA1_Update(&c, buf, len);
            len = 0;
        }
        if (ferror(jarFile))
        {
            // stop right here
            delete[] buf;
            buf = NULL;
            delete[] computed_digest;
            computed_digest = NULL;
            fclose(jarFile);
            return NULL;
        }
        fclose(jarFile);
        // the end of file was reached
        if (len > 0)
        {
            // write the leftovers
            SHA1_Update(&c, buf, len);
        }
        SHA1_Final(computed_digest, &c);
        delete[] buf;
        buf = NULL;
    }
    // format it as hex
    char * digest = new char[2*SHA_1_DIGEST_LEN + 1];
    char * tmp = digest;
    for (int i=0; i<SHA_1_DIGEST_LEN; i++)
    {
        sprintf(tmp, "%02X", computed_digest[i]);
        tmp = tmp + 2;
    }
    delete[] computed_digest;
    computed_digest = NULL;
    digest[2*SHA_1_DIGEST_LEN] = '\0';
    tmp = NULL;
    return digest;
}

char * SecurityUtils::computeDigest(const char* jarFileName)
{
    char * digest = SecurityUtils::computeDigest1(jarFileName);
    if (digest == NULL)
    {
        // one more try
        digest = FileUtils::computeDigest(jarFileName);
    }
    return digest;
}

jobject SecurityUtils::getJNICertDetails(JNIEnv * env, const CERT_DETAILS details)
{
    jclass cert_class = env->FindClass(
                            "com/nokia/mj/impl/security/common/Certificate");
    jmethodID cert_class_cid = env->GetMethodID(cert_class,"<init>",
                               "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    jstring j_issuer = NULL;
    if (strlen(details.issuer) > 0)
    {
        j_issuer = env->NewStringUTF(details.issuer);
    }
    jstring j_subject = NULL;
    if (strlen(details.subject) > 0)
    {
        j_subject = env->NewStringUTF(details.subject);
    }
    jstring j_organization = NULL;
    if (strlen(details.organization) > 0)
    {
        j_organization = env->NewStringUTF(details.organization);
    }
    jstring j_notAfter =
        env->NewStringUTF(details.notAfter);
    jstring j_notBefore =
        env->NewStringUTF(details.notBefore);
    jstring j_serial_number =
        env->NewStringUTF(details.serial_number);
    jstring j_fingerprint =
        env->NewStringUTF(details.fingerprint);
    jobject cert_details = env->NewObject(
                               cert_class,
                               cert_class_cid,
                               j_issuer,j_subject,j_organization, j_notBefore,j_notAfter,j_serial_number,j_fingerprint);
    return cert_details;
}

void SecurityUtils::getCertDetails(X509 cert, CERT_DETAILS* details, bool parse_domain_info)
{
    X509_NAME * subject_name = X509_get_subject_name(&cert);
    details->issuer = new char[512];
    X509_NAME_oneline(X509_get_issuer_name(&cert),details->issuer,512);
    details->subject = new char[512];
    X509_NAME_oneline(subject_name,details->subject,512);
    details->organization = new char[512];
    int ret = X509_NAME_get_text_by_NID(subject_name, NID_organizationName, details->organization, 512);
    if (ret == -1)
    {
        details->organization[0] = '\0';
    }
    ASN1_GENERALIZEDTIME * gTimeBefore = ASN1_TIME_to_generalizedtime(X509_get_notBefore(&cert), NULL);
    details->notBefore = new char[15];
    strncpy(details->notBefore,(const char*)gTimeBefore->data, 14 /* the format is YYYYMMDDHHMMSSZ and we leave the Z out */);
    details->notBefore[14] = '\0';
    ASN1_TIME_free(gTimeBefore);
    ASN1_GENERALIZEDTIME * gTimeAfter = ASN1_TIME_to_generalizedtime(X509_get_notAfter(&cert), NULL);
    details->notAfter = new char[15];
    strncpy(details->notAfter,(const char*)gTimeAfter->data, 14 /* the format is YYMMDDHHMMSSZ and we and we leave the Z out */);
    details->notAfter[14] = '\0';
    ASN1_TIME_free(gTimeAfter);
    // serial number
    ASN1_INTEGER* serial_number = X509_get_serialNumber(&cert);
    details->serial_number = new char[10];
    unsigned char * int_serial_number = new unsigned char[20];
    unsigned char * tmp_serial_number = int_serial_number;
    int len = i2c_ASN1_INTEGER(serial_number, &tmp_serial_number);
    if (len > 0)
    {
        // format it as hex
        sprintf(details->serial_number,"%08lX",int_serial_number);
        details->serial_number[8] = '\0';
    }
    delete[] int_serial_number;
    int_serial_number = NULL;
    tmp_serial_number = NULL;
    // fingerprint
    const EVP_MD * SHA1=EVP_sha1();
    unsigned int n;
    unsigned char * digest = new unsigned char[SHA_1_DIGEST_LEN];
    if (X509_digest(&cert,SHA1,digest,&n))
    {
        details->fingerprint = new char[50];
        char * tmp_fingerprint = NULL;
        tmp_fingerprint = details->fingerprint;
        // format it as hex
        for (int i=0; i<n; i++)
        {
            sprintf(tmp_fingerprint, "%02X", digest[i]);
            tmp_fingerprint = tmp_fingerprint + 2;
        }
        details->fingerprint[2*n] = '\0';
        tmp_fingerprint = NULL;
    }
    delete[] digest;
    digest = NULL;
    // domain info
    // initialize the domain info to unknown value
    details->domain_category = DEVCERT_ANY_DOMAIN;
    if (parse_domain_info)
    {
        ASN1_OBJECT *imei_list_ext_obj = OBJ_txt2obj(DEVCERT_IMEI_LIST_OID, 1);
        if (X509_get_ext_by_OBJ(&cert, imei_list_ext_obj, -1) != -1)
        {
            details->domain_category = DEVCERT_UNKNOWN_DOMAIN;
        }
        // the extension carrying the domain info
        STACK_OF(POLICYINFO) *policiesInfo;
        POLICYINFO *policyInfo;
        char POLICY_OID[80];
        if ((policiesInfo = (STACK_OF(POLICYINFO)*)X509_get_ext_d2i(&cert, NID_certificate_policies, NULL, NULL)) != NULL)
        {
            for (int i=0; i<sk_POLICYINFO_num(policiesInfo); i++)
            {
                policyInfo = sk_POLICYINFO_value(policiesInfo, i);
                OBJ_obj2txt(POLICY_OID, sizeof(POLICY_OID),
                            policyInfo->policyid,
                            1 /* use numerical form */);
                if (strcmp(POLICY_OID, DEVCERT_MANUFACTURER_DOMAIN_OID) == 0)
                {
                    details->domain_category = DEVCERT_MANUFACTURER_DOMAIN;
                    break;
                }
                else if (strcmp(POLICY_OID, DEVCERT_OPERATOR_DOMAIN_OID) == 0)
                {
                    details->domain_category = DEVCERT_OPERATOR_DOMAIN;
                    break;
                }
                else if (strcmp(POLICY_OID, DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN_OID) == 0)
                {
                    details->domain_category = DEVCERT_IDENTIFIEDTHIRDPARTY_DOMAIN;
                    break;
                }
            }
            // cleanup
            for (;;)
            {
                POLICYINFO* policyInfo = sk_POLICYINFO_pop(policiesInfo);
                if (!policyInfo) break;
                POLICYINFO_free(policyInfo);
            }
            sk_POLICYINFO_free(policiesInfo);
        }
        ASN1_OBJECT_free(imei_list_ext_obj);
    }
}

jobjectArray SecurityUtils::getJNIAuthCredentials(JNIEnv * env, vector<AUTH_CREDENTIALS*> all_auth_credentials)
{
    jclass auth_credentials_class = env->FindClass(
                                        "com/nokia/mj/impl/security/midp/authentication/Credentials");
    jmethodID cid = env->GetMethodID(auth_credentials_class,"<init>",
                                     "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILcom/nokia/mj/impl/security/common/Certificate;)V");

    jobjectArray result = env->NewObjectArray(all_auth_credentials.size(), auth_credentials_class, NULL);
    if (result != NULL)
    {
        for (int i=0; i<all_auth_credentials.size(); i++)
        {
            jobject signing_cert = SecurityUtils::getJNICertDetails(env, *(all_auth_credentials[i]->signing_cert));
            delete[] all_auth_credentials[i]->signing_cert->issuer;
            all_auth_credentials[i]->signing_cert->issuer = NULL;
            delete[] all_auth_credentials[i]->signing_cert->subject;
            all_auth_credentials[i]->signing_cert->subject = NULL;
            delete[] all_auth_credentials[i]->signing_cert->organization;
            all_auth_credentials[i]->signing_cert->organization = NULL;
            delete[] all_auth_credentials[i]->signing_cert->notBefore;
            all_auth_credentials[i]->signing_cert->notBefore = NULL;
            delete[] all_auth_credentials[i]->signing_cert->notAfter;
            all_auth_credentials[i]->signing_cert->notAfter = NULL;
            delete[] all_auth_credentials[i]->signing_cert->serial_number;
            all_auth_credentials[i]->signing_cert->serial_number = NULL;
            delete[] all_auth_credentials[i]->signing_cert->fingerprint;
            all_auth_credentials[i]->signing_cert->fingerprint = NULL;
            delete all_auth_credentials[i]->signing_cert;
            all_auth_credentials[i]->signing_cert = NULL;
            jstring j_jar_hash_value =
                env->NewStringUTF(all_auth_credentials[i]->jar_hash);
            jstring j_root_hash_value;
            if (all_auth_credentials[i]->root_id != NULL)
            {
                j_root_hash_value =
                    env->NewStringUTF(all_auth_credentials[i]->root_id);
            }
            else
            {
                j_root_hash_value =
                    env->NewStringUTF(all_auth_credentials[i]->root_hash);
            }
            jstring j_protection_domain_name = NULL;
            if (all_auth_credentials[i]->domain_name != NULL)
            {
                j_protection_domain_name =
                    env->NewStringUTF(all_auth_credentials[i]->domain_name);

            }
            jstring j_protection_domain_category = NULL;
            if (all_auth_credentials[i]->domain_category != NULL)
            {
                j_protection_domain_category =
                    env->NewStringUTF(all_auth_credentials[i]->domain_category);
            }
            jobject auth_credentials = env->NewObject(
                                           auth_credentials_class,
                                           cid,
                                           j_protection_domain_name,
                                           j_protection_domain_category,
                                           j_jar_hash_value,
                                           j_root_hash_value,
                                           all_auth_credentials[i]->chain_index,
                                           signing_cert);
            delete[] all_auth_credentials[i]->domain_name;
            all_auth_credentials[i]->domain_name = NULL;
            delete[] all_auth_credentials[i]->domain_category;
            all_auth_credentials[i]->domain_category = NULL;
            delete[] all_auth_credentials[i]->jar_hash;
            all_auth_credentials[i]->jar_hash = NULL;
            delete[] all_auth_credentials[i]->root_hash;
            all_auth_credentials[i]->root_hash = NULL;
            if (all_auth_credentials[i]->root_id != NULL)
            {
                delete[] all_auth_credentials[i]->root_id;
                all_auth_credentials[i]->root_id = NULL;
            }
            delete all_auth_credentials[i];
            all_auth_credentials[i] = NULL;
            env->SetObjectArrayElement(result, i, auth_credentials);
        }
    }
    all_auth_credentials.clear();
    return result;
}

void SecurityUtils::computePublicKeyHash(X509 * x, char * pkey_hash)
{
    unsigned char tmp[SHA_1_DIGEST_LEN];
    EVP_Digest(x->cert_info->key->public_key->data,
        x->cert_info->key->public_key->length, tmp, NULL, EVP_sha1(), NULL);
    char * tmp_pkey_hash = NULL;
    tmp_pkey_hash = pkey_hash;
    for (int i=0; i<SHA_1_DIGEST_LEN; i++)
    {
        sprintf(tmp_pkey_hash, "%02X", tmp[i]);
        tmp_pkey_hash = tmp_pkey_hash + 2;
    }
    pkey_hash[2*SHA_1_DIGEST_LEN] = '\0';
    tmp_pkey_hash = NULL;
}

void SecurityUtils::getAuthInfo(JNIEnv* env, jobjectArray authInfos, int authInfoIndex, AUTH_INFO * authInfo)
{
    jboolean isCopy;
    jobject jAuthInfo = env->GetObjectArrayElement(authInfos, authInfoIndex);
    jclass authInfoClass = env->GetObjectClass(jAuthInfo);
    jmethodID certChainMethod = env->GetMethodID(
                                    authInfoClass,"getCertChain", "()[Ljava/lang/String;");
    jobjectArray certChain = (jobjectArray)env->CallObjectMethod(
                                 jAuthInfo, certChainMethod);
    jmethodID signatureMethod = env->GetMethodID(
                                    authInfoClass,"getSignature", "()Ljava/lang/String;");
    jstring signature = (jstring)env->CallObjectMethod(
                            jAuthInfo, signatureMethod);
    authInfo->cert_chain_len = env->GetArrayLength(certChain);
    authInfo->cert_chain = new char* [authInfo->cert_chain_len];
    const char * sig(env->GetStringUTFChars(signature, &isCopy));
    int sig_len = env->GetStringLength(signature);
    jstring *jcert = new jstring[authInfo->cert_chain_len];
    for (int i=0; i<authInfo->cert_chain_len; i++)
    {
        jcert[i] = (jstring)env->GetObjectArrayElement(certChain, i);
        const char *str(env->GetStringUTFChars(jcert[i], &isCopy));
        int len = env->GetStringLength(jcert[i]);
        authInfo->cert_chain[i] = SecurityUtils::encodePEM(str, len);
        env->ReleaseStringUTFChars(jcert[i],str);
    }
    // do the base64 decoding
    authInfo->signature = new char[sig_len];
    BIO * b64 = BIO_new(BIO_f_base64());
    BIO * mem = BIO_new_mem_buf((char *)sig, sig_len);
    if (b64 && mem)
    {    
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO_set_close(b64, BIO_CLOSE);
        BIO_set_close(mem, BIO_CLOSE);
        mem = BIO_push(b64, mem);
        authInfo->signature_len = BIO_read(mem, authInfo->signature, sig_len);
        BIO_free_all(mem);
    }
    env->ReleaseStringUTFChars(signature,sig);
    delete[] jcert;
    jcert = NULL;
}


void SecurityUtils::throw_exception(JNIEnv* env, const char * name)
{
    jclass excClass = env->FindClass("com/nokia/mj/impl/security/midp/authentication/AuthenticationException");
    jmethodID excCid = env->GetMethodID(excClass,"<init>", "(I)V");
    jfieldID errCodeID = env->GetStaticFieldID(excClass, name, "I");
    jthrowable exc = (jthrowable)env->NewObject(excClass, excCid, env->GetStaticIntField(excClass, errCodeID));
    env->Throw(exc);
    env->DeleteLocalRef(excClass);
}


// The list of IMEIs is encoded according to Distinguished Encoding Rule (DER)
// of Abstract Syntax Notation One (ASN.1) syntax encapsulating a sequence of
// UTF-8 strings. The exact syntax is:
// sequence_tag  length_tag  total_imei_length  list_of_imeis
// where:
//       sequence_tag = 0x30
//       length_tag = 0x82 (saying that the length is represented on 2 bytes)
//       total_imei_length = hexavalue of (number of imeis multiplied by 17) represented on 2 bytes
//       list_of_imei = zero or more if these elements (utf8string_tag  imei_length imei_hexvalue)
//       utf8string_tag = 0x0C
//       imei_length = hexavalue of length of imei represented on 1 byte (0x0F)
//       imei_hexavalue = array of UTF-8 representation of each of the IMEI digit
bool SecurityUtils::checkIMEI(const X509_EXTENSION * ext, const char * IMEI)
{
    int SEQUENCE_TAG = 0x30;
    int LENGTH_TAG = 0x82;
    int UTF8STRING_TAG = 0x0C;
    int IMEI_LENGTH = 0x0F;
    if (ext == NULL
            || ext->value == NULL
            || ext->value->data == NULL
            || ext->value->length <= 4 /* at least sequence_tag length_tag totam_imei_length */)
    {
        return false;
    }
    unsigned char *imei_list = ext->value->data;
    int len = ext->value->length;
    if (imei_list[0] == SEQUENCE_TAG
            && imei_list[1] == LENGTH_TAG)
    {
        long total_imei_length = (((long)imei_list[2] << 8) | ((long)imei_list[3]));
        // sanity check
        if (total_imei_length == len - 4)
        {
            int imei_index = 4;
            while (imei_index + 17 <= len
                    && imei_list[imei_index] == UTF8STRING_TAG
                    && imei_list[++imei_index] == IMEI_LENGTH)
            {
                imei_index++;
                bool found = false;
                for (int i=0; i<strlen(IMEI); i++)
                {
                    if (imei_list[imei_index] == '*'
                            || imei_list[imei_index] == IMEI[i])
                    {
                        found = true;
                    }
                    else
                    {
                        // go to the next IMEI
                        found = false;
                        imei_index = imei_index + strlen(IMEI) - i;
                        break;
                    }
                    imei_index++;
                }
                if (found)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
