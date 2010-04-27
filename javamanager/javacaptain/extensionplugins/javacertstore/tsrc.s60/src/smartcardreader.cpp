/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/

#include "TestHarness.h"
#include "javasmartcardcertificatesreader.h"
#include <stdio.h>
#include <unistd.h>
#include <openssl/x509.h>
#include <openssl/bio.h>

using namespace java::security;
using namespace std;

void check(vector<TrustedCertificate>&, string);
void check(vector<TrustedCertificate>&, bool);
bool subjectKnown(std::string);

const char CONFIG_FILE_NAME[] = "c:\\java\\javacertstore\\config.txt";
_LIT(KTestDir, "c:\\java\\javacertstore\\");

TEST_GROUP(SmartCardReader)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

/**
 * Retrieve smart card certificates
 */
TEST(SmartCardReader, querycerts)
{
    FILE * config_file;
    vector<TrustedCertificate> trustedCerts;
    // config the cryptotoken plugin to respond with an error when delegated to list token types
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=-1\n");
    fclose(config_file);
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to Leave when delegated to list token types
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with an error when delegated to open token
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=-1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to Leave when delegated to open token
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with an error when delegated to retrieve the store interface
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=-1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to Leave when delegated to retrieve the store interface
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with an error when delegated to list certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=-1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to Leave when delegated to list certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to to respond with an empty list when delegated to list certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=2\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with an error when delegated to check if trusted certificate
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=-1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to Leave when delegated to check if trusted certificate
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with an error when delegated to retrieve certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\config.txt\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=-1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to Leave when delegated to retrieve certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\config.txt\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with an invalid cert when delegated to retrieve certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\config.txt\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    // config the cryptotoken plugin to respond with a valid cert when delegated to retrieve certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, false);
    // config the cryptotoken plugin to respond with a valid, but untrusted cert when delegated to retrieve certificates
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "SmartCardCertUnTrusted=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, true);
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fclose(config_file);
    trustedCerts.clear();
}

/**
 * Retrieve smart card certificates extra info (trusted usage)
 */
TEST(SmartCardReader, querycertsextrainfo)
{
    FILE * config_file;
    vector<TrustedCertificate> trustedCerts;
    string trustedUsage;
    // config the certextras reader to respond with an error when delegated to do the Initialization
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=-1\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fclose(config_file);
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, false);
    // config the certextras reader to Leave when delegated to do the Initialization
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=1\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, false);
    // config the certextras reader to respond with an error when delegated to do the GetExtras
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "UsageCompletionCode=-1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, false);
    // config the certextras reader to Leave when delegated to do the GetExtras
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "UsageCompletionCode=1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    check(trustedCerts, false);
    // config the certextras reader to return TTP usage
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "UsageCompletionCode=0\n");
    fprintf(config_file, "ProtDomainOID=1.3.6.1.4.1.42.2.110.2.2.2.3\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    trustedUsage = "1.3.6.1.4.1.42.2.110.2.2.2.3";
    check(trustedCerts, trustedUsage);
    // config the certextras reader to return OPERATOR usage
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "UsageCompletionCode=0\n");
    fprintf(config_file, "ProtDomainOID=1.3.6.1.4.1.42.2.110.2.2.2.1\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    trustedUsage = "1.3.6.1.4.1.42.2.110.2.2.2.1";
    check(trustedCerts, trustedUsage);
    // config the certextras reader to return OPERATOR SUPPLEMENTARY 1 usage
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "UsageCompletionCode=0\n");
    fprintf(config_file, "ProtDomainOID=1.3.6.1.4.1.42.2.110.2.2.2.4\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    trustedUsage = "1.3.6.1.4.1.42.2.110.2.2.2.4";
    check(trustedCerts, trustedUsage);
    // config the certextras reader to return OPERATOR SUPPLEMENTARY 2 usage
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "UsageCompletionCode=0\n");
    fprintf(config_file, "ProtDomainOID=1.3.6.1.4.1.42.2.110.2.2.2.5\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    trustedUsage = "1.3.6.1.4.1.42.2.110.2.2.2.5";
    check(trustedCerts, trustedUsage);
    // config the certextras reader to return OPERATOR SUPPLEMENTARY 3 usage
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "UsageCompletionCode=0\n");
    fprintf(config_file, "ProtDomainOID=1.3.6.1.4.1.42.2.110.2.2.2.6\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    trustedUsage = "1.3.6.1.4.1.42.2.110.2.2.2.6";
    check(trustedCerts, trustedUsage);
    // config the certextras reader to return unknown usage
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fprintf(config_file, "TokenTypeListCompletionCode=0\n");
    fprintf(config_file, "OpenTokenCompletionCode=0\n");
    fprintf(config_file, "InterfaceRetrievalCompletionCode=0\n");
    fprintf(config_file, "ListCertificatesCompletionCode=0\n");
    fprintf(config_file, "VerifyTrustCompletionCode=0\n");
    fprintf(config_file, "RetrieveCertificateCompletionCode=0\n");
    fprintf(config_file, "CertPath=c:\\java\\javacertstore\\cert.cer\n");
    fprintf(config_file, "ExtraInfoInitializationCompletionCode=0\n");
    fprintf(config_file, "UsageCompletionCode=0\n");
    fprintf(config_file, "ProtDomainOID=1.3.6.1.4.1.42.2.110.2.2.2.7\n");
    fclose(config_file);
    trustedCerts.clear();
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    trustedUsage = "1.3.6.1.4.1.42.2.110.2.2.2.7";
    check(trustedCerts, trustedUsage);
    // cleanup
    trustedCerts.clear();
    config_file = fopen(CONFIG_FILE_NAME, "w+");
    fclose(config_file);
}

void check(vector<TrustedCertificate>& trustedCerts, bool failIfKnownCert)
{
    // check that none of the certs are the testing certificates
    for (int i=0; i<trustedCerts.size(); i++)
    {
        // check that the subject is known
        if (subjectKnown(trustedCerts[i].getData())
                && failIfKnownCert)
        {
            CHECK(false);
        }
    }
    CHECK(true);
}

void check(vector<TrustedCertificate>& trustedCerts, string trustedUsage)
{
    // check that none of the certs are the testing certificates
    for (int i=0; i<trustedCerts.size(); i++)
    {
        // if the subject is known, then the trustedUsage must equal the provided one
        if (subjectKnown(trustedCerts[i].getData()))
        {
            vector<string> trustedUsages = trustedCerts[i].getTrustedUsage();
            CHECK(trustedUsages.size() == 1
                  && trustedUsages[0].compare(trustedUsage) == 0);
        }
    }
    CHECK(true);
}

bool subjectKnown(std::string cert)
{
    X509 *x509 = NULL;
    BIO *bio;
    bool ret = true;
    if ((bio = BIO_new_mem_buf((void *) cert.c_str(), cert.size())))
    {
        BIO_set_close(bio, BIO_CLOSE);
        x509 = d2i_X509_bio(bio,NULL);
        BIO_free(bio);
        if (x509)
        {
            X509_NAME * subject_name = X509_get_subject_name(x509);
            char subject[512];
            X509_NAME_get_text_by_NID(subject_name, NID_commonName, subject, 512);
            const char SUBJECT_NAME[] = "S60_RnD_Certificate_Service_CA";
            ret = (strcmp(SUBJECT_NAME, subject) == 0);
            delete x509;
            x509 = NULL;
        }
    }
    return ret;
}
