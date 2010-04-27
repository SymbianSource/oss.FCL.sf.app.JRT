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


#include "fileutils.h"
#include "securityutils.h"
#include <f32file.h>
#include <openssl/sha.h>
#include "javajniutils.h"
#include "com_nokia_mj_impl_security_midp_authentication_AuthenticationModule.h"
#include <caf/caf.h>
#include <caf/content.h>
#include <caf/data.h>
#include "javajniutils.h"

using namespace java::security;

char * FileUtils::computeDigest(const char* aFileName)
{
    return computeDigest(aFileName, false);
}

char * FileUtils::computeDigest(const char* aFileName, bool drmContent)
{
    RFs fs;
    TInt err = fs.Connect();
    char * digest = NULL;
    RFile file;
    ContentAccess::CContent* cafContent = NULL;
    ContentAccess::CData* cafData = NULL;
    if (err == KErrNone)
    {
        int len = strlen(aFileName);
        HBufC * fileName = HBufC::New(len);
        TPtr fileNamePtr = fileName->Des();
        TPtr8 ptr8((TUint8 *)aFileName, len);
        ptr8.SetLength(len);
        fileNamePtr.Copy(ptr8);
        if (drmContent)
        {
            TRAP(err,
                 cafContent = ContentAccess::CContent::NewL(fileNamePtr);
                 cafData = cafContent->OpenContentL(ContentAccess::EPeek););
        }
        else
        {
            err = file.Open(fs, fileNamePtr, EFileShareReadersOrWriters);
        }
        delete fileName;
        fileName = NULL;
        if (err == KErrNone)
        {
            // figure out the size of the file
            TInt size;
            if (drmContent)
            {
                TRAP(err, cafData->DataSizeL(size));
            }
            else
            {
                err = file.Size(size);
            }
            if (err == KErrNone)
            {
                // if the size of the file is less than the size of the chunks,
                // then do the hash calculation in on go
                unsigned char * computed_digest = NULL;
                HBufC8* buf = NULL;
                if (size > 0 && size <= SHA_1_HASH_CHUNK_LEN)
                {
                    // do the hash calculation in one go
                    TRAPD(err, buf = HBufC8::NewL(size););
                    if (err == KErrNone)
                    {
                        TPtr8 ptr = buf->Des();
                        if (drmContent)
                        {
                            cafData->Read(ptr, size);
                        }
                        else
                        {
                            err = file.Read(ptr, size);
                        }
                        if (err == KErrNone)
                        {
                            computed_digest = new unsigned char[SHA_1_DIGEST_LEN];
                            SHA1((const unsigned char *)buf->Ptr(), size, computed_digest);
                        }
                        delete buf;
                        buf = NULL;
                    }

                }
                else
                {
                    // do the hash calculation in chunks
                    SHA_CTX c;
                    SHA1_Init(&c);
                    TRAPD(err, buf = HBufC8::NewL(SHA_1_HASH_CHUNK_LEN););
                    if (err == KErrNone)
                    {
                        TPtr8 ptr = buf->Des();
                        if (drmContent)
                        {
                            cafData->Read(ptr, SHA_1_HASH_CHUNK_LEN);
                        }
                        else
                        {
                            err = file.Read(ptr, SHA_1_HASH_CHUNK_LEN);
                        }
                        while (err == KErrNone
                                && ptr.Length() > 0)
                        {
                            SHA1_Update(&c, (const unsigned char *)buf->Ptr(), ptr.Length());
                            if (drmContent)
                            {
                                cafData->Read(ptr, SHA_1_HASH_CHUNK_LEN);
                            }
                            else
                            {
                                err = file.Read(ptr, SHA_1_HASH_CHUNK_LEN);
                            }
                        }
                        delete buf;
                        buf = NULL;
                        if (err == KErrNone)
                        {
                            // put it all together
                            computed_digest = new unsigned char[SHA_1_DIGEST_LEN];
                            SHA1_Final(computed_digest, &c);
                        }
                    }
                }

                // format it as hex
                if (computed_digest != NULL)
                {
                    digest = new char[2*SHA_1_DIGEST_LEN + 1];
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
                }
            }
            if (drmContent)
            {
                if (cafData)
                {
                    delete cafData;
                    cafData = NULL;
                }
                if (cafContent)
                {
                    delete cafContent;
                    cafContent = NULL;
                }
            }
            else
            {
                file.Close();
            }
        }
        fs.Close();
    }
    return digest;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_security_midp_authentication_AuthenticationModule__1drmDecryptAndComputeHash
(JNIEnv * env, jobject, jstring appJARPath)
{
    jboolean isCopy;
    const char* jarPath(env->GetStringUTFChars(appJARPath, &isCopy));
    char * jarHashValue = FileUtils::computeDigest(jarPath, true);
    env->ReleaseStringUTFChars(appJARPath, jarPath);
    if (jarHashValue != NULL)
    {
        jstring hash = env->NewStringUTF(jarHashValue);
        delete[] jarHashValue;
        jarHashValue = NULL;
        return hash;
    }
    return NULL;
}
