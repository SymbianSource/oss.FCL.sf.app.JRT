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
#include "trustrootpolicy.h"
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include "javasymbianoslayer.h"

_LIT(KLegacyCertsPath,           "z:\\private\\10203636\\security\\trustroots\\device\\certificates\\");
_LIT(KOmjCertsPath,              "c:\\private\\200211dc\\security\\trustroots\\device\\certificates\\");
_LIT8(KOmjCertsPath8,              "c:\\private\\200211dc\\security\\trustroots\\device\\certificates\\");
_LIT(KOmjCertsTmpPath,              "c:\\private\\200211dc\\security\\trustroots\\device\\certificates\\tmp\\");
_LIT8(KOmjCertsTmpPath8,              "c:\\private\\200211dc\\security\\trustroots\\device\\certificates\\tmp\\");
_LIT(KLegacyTrustRootPolicyPath, "z:\\private\\10203636\\security\\midp2_trp.xml");
_LIT(KLegacyCertsStatesPath,    "c:\\private\\10203636\\security\\trustroots\\device\\state\\");
_LIT(KOmjCertsStatesPath,       "c:\\private\\200211dc\\security\\trustroots\\device\\state\\");

static bool CertsCopyNeeded()
{
    _LIT(KSystemAmsSid, "10203636");
    return (KOmjCertsPath().Find(KSystemAmsSid) == -1);
}

static void MoveFileL(const TDesC& oldPath, const TDesC& newPath, RFs& fsSession)
{
    CFileMan* fileMan = CFileMan::NewL(fsSession);
    CleanupStack::PushL(fileMan);
    fileMan->Move(oldPath, newPath);
    CleanupStack::PopAndDestroy();
}

static void RemoveDirL(const TDesC& dirName, RFs& fsSession)
{
    CFileMan* fileMan = CFileMan::NewL(fsSession);
    CleanupStack::PushL(fileMan);
    fileMan->RmDir(dirName);
    CleanupStack::PopAndDestroy();
}

static string GetDomainName(string domainId)
{
    string domainName = "";
    if (domainId == "*MFD*"
            || domainId == "*MFDU*")
    {
        domainName = "Manufacturer";
    }
    else if (domainId == "*OPD*"
             || domainId == "*OPDU*"
             || domainId == "*ODOPDU*"
             || domainId == "*ODOPD*")
    {
        domainName = "Operator";
    }
    else if (domainId == "*TTPD*")
    {
        domainName = "IdentifiedThirdParty";
    }
    return domainName;
}

static string GetDomainCategory(string domainId)
{
    string domainCategory = "";
    if (domainId == "*MFD*"
            || domainId == "*MFDU*")
    {
        domainCategory = "MFD";
    }
    else if (domainId == "*OPD*"
             || domainId == "*ODOPD*"
             || domainId == "*OPDU*"
             || domainId == "*ODOPDU*")
    {
        domainCategory = "OPD";
    }
    else if (domainId == "*TTPD*")
    {
        domainCategory = "ITPD";
    }
    return domainCategory;
}

static string GetHash(string trustRootPath)
{
    /* The length of the MD5 digest (32 digit hexadecimal number) */
    const int MD5_DIGEST_LEN = 8;
    char trustRootHash[MD5_DIGEST_LEN + 1];
    trustRootHash[0] = '\0';

    FILE* trustRootDataFile = fopen(trustRootPath.c_str(), "r");
    if (trustRootDataFile != NULL)
    {
        if (fseek(trustRootDataFile, 0L, SEEK_END) == 0)
        {
            long  trustRootDataLen = ftell(trustRootDataFile);
            if (trustRootDataLen > 0)
            {
                if (fseek(trustRootDataFile, 0L, SEEK_SET) == 0)
                {
                    char* trustRootData = new char[trustRootDataLen];
                    if (trustRootData != NULL)
                    {
                        fread(trustRootData, sizeof(char), trustRootDataLen, trustRootDataFile);
                        if (!ferror(trustRootDataFile))
                        {
                            BIO*  bio = BIO_new_mem_buf((void *) trustRootData, trustRootDataLen);
                            if ((bio))
                            {
                                BIO_set_close(bio, BIO_CLOSE);
                                X509* trustRootCert = d2i_X509_bio(bio,NULL);
                                if (trustRootCert != NULL)
                                {
                                    sprintf(trustRootHash,"%08lX",X509_issuer_name_hash(trustRootCert));
                                    trustRootHash[MD5_DIGEST_LEN] = '\0';
                                    delete trustRootCert;
                                    trustRootCert = NULL;
                                }
                            }
                            BIO_free(bio);
                        }
                        delete[] trustRootData;
                    }
                }
            }
        }
        fclose(trustRootDataFile);
    }
    return string(trustRootHash);
}

static void CopyFilesL(const TDesC& aSource, const TDesC& aDest, RFs& fsSession)
{
    // Create file management object
    CFileMan* fileMan = CFileMan::NewL(fsSession);
    CleanupStack::PushL(fileMan);

    // Do copy (here synchronously)
    fileMan->Copy(aSource, aDest, CFileMan::EOverWrite|CFileMan::ERecurse);
    // clear the read-only attribute
    _LIT(KWildCards,"*");
    HBufC* newDir = HBufC::NewLC(aDest.Length() + 1);
    TPtr ptr = newDir->Des();
    ptr.Copy(aDest);
    ptr.Append(KWildCards);
    fileMan->Attribs(ptr,KEntryAttNormal, KEntryAttReadOnly, TTime(0),CFileMan::ERecurse);
    CleanupStack::PopAndDestroy(newDir);

    // Clean up
    CleanupStack::PopAndDestroy();
}

static void HandleTrustRootsL(const std::vector<TrustRoot>& aConfiguredTrustRoots, RFs& fsSession)
{
    string path((const char *)KOmjCertsPath8().Ptr(), KOmjCertsPath8().Length());
    string tmpPath;
    if (!CertsCopyNeeded())
    {
        tmpPath = path;
    }
    else
    {
        tmpPath = string((const char *)KOmjCertsTmpPath8().Ptr(), KOmjCertsTmpPath8().Length());
    }
    for (int i=0; i<aConfiguredTrustRoots.size(); i++)
    {
        TrustRoot trustRoot = aConfiguredTrustRoots[i];

        // check if the corresponding certificate exists
        string tmpBaseName = tmpPath + trustRoot.iName;
        string trustRootName = tmpBaseName + ".cer";
        string rootName = trustRoot.iName + ".cer";
        FILE* trustRootFile = fopen(trustRootName.c_str(), "r");
        if (trustRootFile == NULL)
        {
            trustRootName.clear();
            trustRootName = tmpBaseName + ".der";
            trustRootFile = fopen(trustRootName.c_str(), "r");
        }
        if (trustRootFile != NULL)
        {
            fclose(trustRootFile);
            string domainName = GetDomainName(trustRoot.iDomain);
            if (domainName == "")
            {
                continue;
            }
            string domainCategory = GetDomainCategory(trustRoot.iDomain);
            if (domainCategory == "")
            {
                continue;
            }
            string hash = GetHash(trustRootName);
            if (hash == "")
            {
                continue;
            }
            // generate the metadata
            string baseName = path + trustRoot.iName;
            FILE * metadataFile = fopen((baseName + ".metadata").c_str(), "w+");
            if (metadataFile != NULL)
            {
                // copy the cert from tmp
                if (CertsCopyNeeded())
                {
                    HBufC* tmp = stringToDes(trustRootName.c_str());
                    CleanupStack::PushL(tmp);
                    TPtr ptr = tmp->Des();
                    MoveFileL(ptr, KOmjCertsPath, fsSession);
                    CleanupStack::PopAndDestroy(tmp);
                }

                // generate the metadatafile
                fprintf(metadataFile, "name=%s\n", domainName.c_str());
                fprintf(metadataFile, "category=%s\n", domainCategory.c_str());
                fprintf(metadataFile, "removable=%d\n", (trustRoot.iCanDelete == true));
                fprintf(metadataFile, "disablable=%d\n", (trustRoot.iCanDisable == true));
                fprintf(metadataFile, "hash=%s\n", hash.c_str());
                fclose(metadataFile);
            }
        }
    }
}


static void ConvertTrustRootPolicyL(RFs& fsSession)
{
    // read the trust root info
    CTrustRootPolicy* trustRootPolicy = CTrustRootPolicy::NewL();
    CleanupStack::PushL(trustRootPolicy);
    std::vector<TrustRoot> trustRoots;
    trustRootPolicy->ReadFromFileL(KLegacyTrustRootPolicyPath, trustRoots);
    CleanupStack::PopAndDestroy(trustRootPolicy);

    // generate trust roots metadata
    HandleTrustRootsL(trustRoots, fsSession);
}

static void CleanupL(RFs& fsSession)
{
    if (!CertsCopyNeeded())
    {
        return;
    }
    // delete the tmp directory (together with the leftovers = certificates without metadata)
    RemoveDirL(KOmjCertsTmpPath, fsSession);
}

static void CopyCertsL(RFs& fsSession)
{
    if (!CertsCopyNeeded())
    {
        return;
    }

    // copy all the files from KLegacyCertsPath to KOmjCertsPath
    CopyFilesL(KLegacyCertsPath, KOmjCertsTmpPath, fsSession);
    // copy all the files from KLegacyCertsStatesPath to KOmjCertsStatesPath
    CopyFilesL(KLegacyCertsStatesPath, KOmjCertsStatesPath, fsSession);
}

static void DoL()
{
    // open file server session
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());

    // copy the certs and state infos from legacy to omj
    CopyCertsL(fsSession);

    // convert the trust root policy from KLegacyTrustRootPolicyPath to KOmjCertsPath
    ConvertTrustRootPolicyL(fsSession);

    // cleanup
    CleanupL(fsSession);

    // close file server session
    fsSession.Close();
}

TInt E32Main()
{
    //__UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    TRAPD(error, DoL());
    delete cleanupStack;
    //__UHEAP_MARKEND;
    return error;
}
