/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javasmartcardcertificatesreader.h"
#include "smartcardcryptotokenreader.h"
#include "smartcardcertextrasreader.h"
#include <openssl/x509.h>
#include <openssl/bio.h>

using namespace std;

namespace java
{
namespace security
{

const int SHA_1_DIGEST_LEN = 20;
const int MD5_DIGEST_LEN = 8;

/* forward declarations of local/private methods */
unsigned char * computePublicKeyHash(const HBufC8*);
void computeCertHash(const char *, int, string&);

void JavaSmartCardCertificatesReader::retrieveTrustedCertificates(vector<TrustedCertificate>& aTrustedCerts)
{
    // retrieve the smart card certificates
    SmartCardCryptoTokenReader* smartCardCertsReader = SmartCardCryptoTokenReader::NewL();
    RArray<HBufC8*> smartCardCerts;
    smartCardCertsReader->RetrieveCerts(smartCardCerts);
    delete smartCardCertsReader;
    smartCardCertsReader = NULL;

    // retrieve the smart card certificates trusted usages
    SmartCardCertExtrasReader* smartCardCertsExtrasReader = SmartCardCertExtrasReader::NewL();
    RCPointerArray<HBufC> smartCardCertTrustedUsages;
    for (int i=0; i<smartCardCerts.Count(); i++)
    {
        // compute the SHA1 hash of the public key
        unsigned char * publicKeyHash = computePublicKeyHash(smartCardCerts[i]);
        if (publicKeyHash)
        {
            // retrieve the certificate's extra info
            TPtrC8 ptrPublicKeyHash((TUint8 *)publicKeyHash, SHA_1_DIGEST_LEN);
            smartCardCertsExtrasReader->GetCertTrustedUsages(ptrPublicKeyHash, smartCardCertTrustedUsages);
            // filter the retrieved trusted usages
            vector<string> trustedUsage;
            for (int j=0; j<smartCardCertTrustedUsages.Count(); j++)
            {
                HBufC8 *buf8 = HBufC8::NewMaxL(smartCardCertTrustedUsages[j]->Length());
                buf8->Des().Copy(smartCardCertTrustedUsages[j]->Des());
                string usage((const char *)buf8->Ptr(), buf8->Length());
                delete buf8;
                buf8 = NULL;
                trustedUsage.push_back(usage);
            }
            // format the public key hash as HEX
            string hash = "";
            computeCertHash((const char*)smartCardCerts[i]->Ptr(), smartCardCerts[i]->Length(), hash);
            if (hash.size() > 0)
            {
                // save the certificate together with it's trusted usage
                aTrustedCerts.push_back(TrustedCertificate(string(
                                            (const char*)smartCardCerts[i]->Ptr(),
                                            smartCardCerts[i]->Length()),
                                        hash,
                                        trustedUsage));
            }

            // cleanup
            delete[] publicKeyHash;
            publicKeyHash = NULL;
        }
        delete smartCardCerts[i];
    }
    smartCardCerts.Close();
    smartCardCertTrustedUsages.Close();
    delete smartCardCertsExtrasReader;
    smartCardCertsExtrasReader = NULL;
}

unsigned char * computePublicKeyHash(const HBufC8* smartCardCert)
{
    unsigned char * publicKeyHash = NULL;
    X509 *x509 = NULL;
    BIO *bio;
    bio = BIO_new_mem_buf((void *) smartCardCert->Ptr(), smartCardCert->Length());
    if ((bio))
    {
        BIO_set_close(bio, BIO_CLOSE);
        x509 = d2i_X509_bio(bio,NULL);
        BIO_free(bio);
        if (x509)
        {
            ASN1_BIT_STRING* publicKey = X509_get_X509_PUBKEY(x509)->public_key;
            publicKeyHash = new unsigned char[SHA_1_DIGEST_LEN + 1];
            SHA1(publicKey->data,publicKey->length,publicKeyHash);
            publicKeyHash[SHA_1_DIGEST_LEN] = '\0';
            delete x509;
            x509 = NULL;
        }
    }
    return publicKeyHash;
}


void computeCertHash(const char * cert, int certLen, string& hash)
{
    char computed_hash[MD5_DIGEST_LEN + 1];
    computed_hash[0] = '\0';
    X509 *x509 = NULL;
    BIO *bio;
    bio = BIO_new_mem_buf((void *) cert, certLen);
    if ((bio))
    {
        BIO_set_close(bio, BIO_CLOSE);
        x509 = d2i_X509_bio(bio,NULL);
        BIO_free(bio);
        if (x509)
        {
            sprintf(computed_hash,"%08lX",X509_issuer_name_hash(x509));
            // add the '\0'
            computed_hash[MD5_DIGEST_LEN] = '\0';
            delete x509;
            x509 = NULL;
        }
    }
    hash += string(computed_hash, MD5_DIGEST_LEN);
}

} //end namespace security
} //end namespace java
