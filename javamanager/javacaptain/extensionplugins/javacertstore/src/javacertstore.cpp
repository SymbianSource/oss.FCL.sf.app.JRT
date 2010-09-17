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
* Description:  JavaCertStore
*
*/

#include "commsendpoint.h"
#include "comms.h"
#include "commsmessage.h"
#include "logger.h"
#include "javaoslayer.h"
#include "coreinterface.h"
#include "javacertstore.h"
#include "securitycommsmessagedefs.h"
#include "javacommonutils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "trustedcertificate.h"
#include "javasmartcardcertificatesreader.h"
#include "metadatafilehandler.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::captain::JavaCertStore();
}

using namespace std;
using namespace java::security;
using namespace java::util;
using namespace java::comms;

namespace java
{
namespace captain
{


JavaCertStore::JavaCertStore() : iCore(0)
{
    JELOG2(EJavaCaptain);
}

JavaCertStore::~JavaCertStore()
{
    JELOG2(EJavaCaptain);
}

void JavaCertStore::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaCaptain);
    iCore = core;
    iCore->getComms()->registerListener(java::comms::PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C, this);
    loadCertsMetadata();
}

void JavaCertStore::stopPlugin()
{
    JELOG2(EJavaCaptain);
    iCore->getComms()->unregisterListener(java::comms::PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C, this);
    iCore = 0;
    unloadCertsMetadata();
}

java::comms::CommsListener* JavaCertStore::getCommsListener()
{
    JELOG2(EJavaCaptain);
    return this;
}

void JavaCertStore::processMessage(java::comms::CommsMessage& aMessage)
{
    JELOG2(EJavaCaptain);

    switch (aMessage.getMessageId())
    {
    case JAVA_CERT_STORE_MSG_ID_REQUEST:
    {
        int operation = 0;
        aMessage >> operation;
        switch (operation)
        {
        case JAVA_CERT_STORE_OPERATION_QUERY_CERTS:
            queryCerts(aMessage);
            break;
        case JAVA_CERT_STORE_OPERATION_REFRESH_CERTS:
            /*if (!aMessage.hasPermission(MANAGE_CERTIFICATES)) {
              break;
            }*/
            unloadCertsMetadata();
            loadCertsMetadata();
            break;
        case JAVA_CERT_STORE_OPERATION_DELETE_CERT:
        case JAVA_CERT_STORE_OPERATION_DISABLE_CERT:
        case JAVA_CERT_STORE_OPERATION_ENABLE_CERT:
            // check that the client is trusted
            if (!aMessage.hasPermission(MANAGE_CERTIFICATES))
            {
                break;
            }
            std::string id;
            aMessage >> id;
            int state = STATE_UNDEFINED;
            switch (operation)
            {
            case JAVA_CERT_STORE_OPERATION_DELETE_CERT:
                state = STATE_DELETED;
                break;
            case JAVA_CERT_STORE_OPERATION_DISABLE_CERT:
                state = STATE_DISABLED;
                break;
            case JAVA_CERT_STORE_OPERATION_ENABLE_CERT:
                state = STATE_ENABLED;
                break;
            }
            manageCert(id, state);
            break;
        }
    }
    break;
    }
}

void JavaCertStore::queryCerts(java::comms::CommsMessage& aMessage)
{
    JELOG2(EJavaCaptain);
    int id = 0;
    std::string hash;
    int state = 0;
    aMessage >> id;
    int query = 0;
    // process the filter
    while (id != 0)
    {
        switch (id)
        {
        case JAVA_CERT_STORE_FILTER_ID_HASH:
            aMessage >> hash;
            break;
        case JAVA_CERT_STORE_FILTER_ID_STATE:
            int tmp;
            aMessage >> tmp;
            state = state | tmp;
            break;
        case JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN:
            query = JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
            break;
        case JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS:
            query = JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
            break;
        case JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT:
            query = JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT;
            break;
        case JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT_PEM:
            query = JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT_PEM;
            break;
        case JAVA_CERT_STORE_QUERY_ID_STATE:
            query = JAVA_CERT_STORE_QUERY_ID_STATE;
            break;
        }
        aMessage >> id;
    }
    java::comms::CommsMessage replyMsg;
    replyMsg.setReceiver(aMessage.getSender());
    replyMsg.setMessageRef(aMessage.getMessageRef());
    bool queryEnabled = (state & JAVA_CERT_STORE_STATE_ENABLED);
    bool queryDisabled = (state & JAVA_CERT_STORE_STATE_DISABLED);
    bool queryDeleted = (state & JAVA_CERT_STORE_STATE_DELETED);
    bool queryState = (query == JAVA_CERT_STORE_QUERY_ID_STATE);
    bool queryResponseSet = false;
    transform(hash.begin(), hash.end(), hash.begin(), (int(*)(int)) tolower);
    for (int i=0; i<no_certs; i++)
    {
        if ((state == 0 ||
                (queryEnabled && iCertsMetadata[i]->state == STATE_ENABLED)
                || (queryDisabled && iCertsMetadata[i]->state == STATE_DISABLED)
                || (queryDeleted && iCertsMetadata[i]->state == STATE_DELETED))
                && (hash.compare("") == 0
                    || iCertsMetadata[i]->hash.compare(hash) == 0 
                    || iCertsMetadata[i]->pkey.find(hash) == 0))
        {
            // what we return back depends on what was queried
            switch (query)
            {
            case JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN:
                replyMsg << iCertsMetadata[i]->prot_domain_name;
                replyMsg << iCertsMetadata[i]->prot_domain_category;
                break;
            case JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS:
                // return content, ID (which is the actual hash)
                // and the encoded info about the state
                if (replyWithContent(replyMsg, *iCertsMetadata[i]))
                {
                    // add also the ID and the state
                    if (iCertsMetadata[i]->pkey.size() > 0)
                    {
                        replyMsg << iCertsMetadata[i]->pkey;
                    }
                    else
                    {
                        replyMsg << iCertsMetadata[i]->hash;
                    }
                    replyMsg << encodeState(iCertsMetadata[i]->disposable,
                                            iCertsMetadata[i]->disablable,
                                            iCertsMetadata[i]->state);
                }
                break;
            case JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT:
                // return the length and the content of the certificate
                replyWithContent(replyMsg, *iCertsMetadata[i]);
                break;
            case JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT_PEM:
                // return the length and the content of the certificate in PEM format
                replyWithContent(replyMsg, *iCertsMetadata[i], PEM_FORMAT);
                break;
            case JAVA_CERT_STORE_QUERY_ID_STATE:
                if (iCertsMetadata[i]->state == STATE_ENABLED)
                {
                    replyMsg << JAVA_CERT_STORE_STATE_ENABLED;
                }
                else if (iCertsMetadata[i]->state == STATE_DISABLED)
                {
                    replyMsg << JAVA_CERT_STORE_STATE_DISABLED;
                }
                else if (iCertsMetadata[i]->state == STATE_DELETED)
                {
                    replyMsg << JAVA_CERT_STORE_STATE_DELETED;
                }
                else
                {
                    replyMsg << JAVA_CERT_STORE_STATE_UNKNOWN;
                }
                break;
            default:
                // if nothing specifically was queried,
                // return the full_path
                replyMsg << iCertsMetadata[i]->full_path;
            }
            queryResponseSet = true;
        }
    }
    if (queryState && !queryResponseSet)
    {
        replyMsg << JAVA_CERT_STORE_STATE_NOT_PRESENT;
    }
    int r = iCore->getComms()->send(replyMsg);
    if (r != 0)
    {
        ELOG1(EJavaCaptain,
              "[JavaCertStore] - error replying to JavaCaptain %s",
              java::util::JavaCommonUtils::intToString(r).c_str());
    }
}

void JavaCertStore::loadCertsMetadata()
{
    // loads the metadata of the certificates stored on device
    iPrimaryCertsPath = "Z";
    java::util::JavaOsLayer::getJavaCaptainRoot(iPrimaryCertsPath, true);
    iPrimaryCertsPath.append(KJavaCertsDir);
    iPrimaryCertsPath.append(1, KFileSeparator);
    iSecondaryCertsPath = "C";
    java::util::JavaOsLayer::getJavaCaptainRoot(iSecondaryCertsPath, true);
    iSecondaryCertsPath.append(KJavaCertsDir);
    iSecondaryCertsPath.append(1, KFileSeparator);
    iCertsMetadataPath = "C";
    java::util::JavaOsLayer::getJavaCaptainRoot(iCertsMetadataPath, true);
    iCertsMetadataPath.append(KJavaCertsStateDir);
    iCertsMetadataPath.append(1, KFileSeparator);
    no_certs = 0;
    loadCertsMetadata(iPrimaryCertsPath, true);
    loadCertsMetadata(iSecondaryCertsPath, false);

    // loads the metadata of the smart card certificates
    vector<TrustedCertificate> trustedCerts;
    JavaSmartCardCertificatesReader::retrieveTrustedCertificates(trustedCerts);
    for (int i=0; i<trustedCerts.size(); i++)
    {
        CERT_METADATA * metadata = new CERT_METADATA();
        metadata->disposable = false;
        metadata->disablable = false;
        metadata->state = STATE_ENABLED;
        metadata->changes = false;
        metadata->hash = trustedCerts[i].getId();
        transform(metadata->hash.begin(), metadata->hash.end(), metadata->hash.begin(), (int(*)(int)) tolower);
        metadata->full_path = "Smart Card";
        metadata->len = trustedCerts[i].getData().size();
        metadata->data = trustedCerts[i].getData();
        assignProtectionDomain(trustedCerts[i].getTrustedUsage(), metadata);
        addCertMetadataToCache(metadata, true /* overwrite */);
    }
    trustedCerts.clear();
}

void JavaCertStore::unloadCertsMetadata()
{
    for (int i=0; i<no_certs; i++)
    {
        // free the memory
        CERT_METADATA * metadata = iCertsMetadata[i];
        writeMetadataIntoFile(metadata);
        delete metadata;
    }
    // clear the cache
    iCertsMetadata.clear();
    no_certs = 0;
}

void JavaCertStore::writeMetadataIntoFile(CERT_METADATA * metadata)
{
    if (!metadata->changes)
    {
        // no changes
        return;
    }

    if (JavaCertStoreMetadataFileHandler::writeState(
                metadata->file_name, metadata->state))
    {
        // reset the changes flag
        metadata->changes = false;
    }
}

bool JavaCertStore::readMetadataFromFiles(const std::string& aCertsLocation, const std::string& cert_file_name, CERT_METADATA * metadata)
{
    // there are two types of metadata:
    // 1) read-only metadata (this metadata resides into file with the same name than
    //    certificate file, but with KMetadataSuffix as extension)
    // 2) read-write metadata (this metadata resides into file with the same name than
    //    certificate file, but with KStateSuffix as extension)
    bool readingSucceeded = false;
    size_t ext = cert_file_name.rfind('.');
    if (ext != string::npos)
    {
        std::string file_name_without_extension = string(cert_file_name, 0, ext);
        // read-only metadata
        std::string read_only_metadata_file_name = aCertsLocation
                + file_name_without_extension
                + KMetadataSuffix;
        std::string read_write_metadata_file_name = iCertsMetadataPath
                + file_name_without_extension
                + KStateSuffix;
        // read the read-only metadata file */
        FILE * read_only_metadata_file = fopen(read_only_metadata_file_name.c_str(),"r");
        if (read_only_metadata_file != NULL)
        {
            // save the name of the metadata_file for later use
            metadata->file_name = read_write_metadata_file_name;

            // identifier for the metadata value being read
            const int READ_DOMAIN_NAME = 2;
            const int READ_DOMAIN_CATEGORY = 3;
            const int READ_HASH = 4;
            const int READ_REMOVABLE = 5;
            const int READ_DISABLABLE = 6;
            const int READ_PKEY = 7;

            // domain_name_info
            int domain_name_index = 0;
            char domain_name[50];

            // domain_category info
            int domain_category_index = 0;
            char domain_category[50];

            // hash info
            int hash_index = 0;
            char hash[50];

            // removable&disablable
            bool removable = false;
            bool disablable = false;

            // pkey info
            int pkey_index = 0;
            char pkey[50];

            // start&end separators for the metadata's keys
            bool key_ss = true;
            bool key_es = false;
            int retval;

            // identifier for the operation being performed
            int op = 0;
            while ((int)(retval = getc(read_only_metadata_file))!= EOF)
            {
                if (key_ss)
                {
                    // start of reading something new
                    switch ((char)retval)
                    {
                    case 'n':
                        op = READ_DOMAIN_NAME;
                        key_ss = false;
                        break;
                    case 'c':
                        op = READ_DOMAIN_CATEGORY;
                        key_ss = false;
                        break;
                    case 'h':
                        op = READ_HASH;
                        key_ss = false;
                        break;
                    case 'r':
                        op = READ_REMOVABLE;
                        key_ss = false;
                        break;
                    case 'd':
                        op = READ_DISABLABLE;
                        key_ss = false;
                        break;
                    case 'p':
                        op = READ_PKEY;
                        key_ss = false;
                        break;
                    }
                }
                else
                {
                    if ((char)retval == '=')
                    {
                        // end separator
                        key_es = true;
                    }
                    else if (retval == 10 || retval == 13 /* CR or LF */)
                    {
                        key_ss = true;
                        key_es = false;
                    }
                    else if (key_es)
                    {
                        switch (op)
                        {
                        case READ_DOMAIN_NAME:
                            domain_name[domain_name_index] = (char)retval;
                            domain_name_index++;
                            domain_name[domain_name_index] = '\0';
                            break;
                        case READ_DOMAIN_CATEGORY:
                            domain_category[domain_category_index] = (char)retval;
                            domain_category_index++;
                            domain_category[domain_category_index] = '\0';
                            break;
                        case READ_HASH:
                            hash[hash_index] = (char)retval;
                            hash_index++;
                            hash[hash_index] = '\0';
                            break;
                        case READ_REMOVABLE:
                            if ((char)retval == '1')
                            {
                                removable = true;
                            }
                            break;
                        case READ_DISABLABLE:
                            if ((char)retval == '1')
                            {
                                disablable = true;
                            }
                            break;
                        case READ_PKEY:
                            pkey[pkey_index] = (char)retval;
                            pkey_index++;
                            pkey[pkey_index] = '\0';
                            break;
                        }
                    }
                }
            }
            if (domain_name_index > 0
                    && domain_category_index > 0
                    && hash_index > 0)
            {
                if (pkey_index > 0)
                {
                    metadata->pkey = string(pkey, pkey_index);
                    transform(metadata->pkey.begin(), metadata->pkey.end(), metadata->pkey.begin(), (int(*)(int)) tolower);
                }
                metadata->prot_domain_name = string(domain_name, domain_name_index);
                metadata->prot_domain_category = string(domain_category, domain_category_index);
                metadata->hash = string(hash, hash_index);
                transform(metadata->hash.begin(), metadata->hash.end(), metadata->hash.begin(), (int(*)(int)) tolower);
                metadata->disposable = removable;
                metadata->disablable = disablable;
                fclose(read_only_metadata_file);
                // read the read-write metadata file
                int state = JavaCertStoreMetadataFileHandler::readState(
                                read_write_metadata_file_name);
                if (state == STATE_UNDEFINED)
                {
                    // create the file and initialize it with
                    metadata->state = STATE_ENABLED;
                    // create the directory (if it doesn't exist)
                    std::string certsStateBaseDir = "C";
                    java::util::JavaOsLayer::getJavaCaptainRoot(certsStateBaseDir, true);
                    if (mkDirAll(KJavaCertsStateDir, certsStateBaseDir))
                    {
                        // force the writing
                        metadata->changes = true;
                        writeMetadataIntoFile(metadata);
                    }
                }
                else
                {
                    switch (state)
                    {
                    case STATE_ENABLED:
                    case STATE_DISABLED:
                    case STATE_DELETED:
                        // it's ok
                        break;
                    default:
                        // any other state will be translated
                        // into enabled
                        state = STATE_ENABLED;
                    }
                    metadata->state = state;
                }
                // the data has just been read, so no changes
                metadata->changes = false;
                readingSucceeded = true;
            }
        }
    }
    return readingSucceeded;
}

// The state of a certificate is encoded on 3 bits:
// XXX, the most significant bit represents a boolean indicating if the certificate can be deleted,
// the second one represents a boolean indicating if the certificate can be disabled and the third
// one represents a boolean indicating if the certificate is disabled or not can_be_disabled and
// the third one is_disabled
int JavaCertStore::encodeState(bool disposable, bool disablable, int state)
{
    const int DISPOSABLE_STATE_MASK = 4;
    const int DISABLABLE_STATE_MASK = 2;
    const int IS_DISABLED_MASK = 1;

    int encoded_state = 0;

    if (disposable == true)
    {
        encoded_state = encoded_state | DISPOSABLE_STATE_MASK;
    }
    if (disablable == true)
    {
        encoded_state = encoded_state | DISABLABLE_STATE_MASK;
    }
    if (state == STATE_DISABLED)
    {
        encoded_state = encoded_state | IS_DISABLED_MASK;
    }
    return encoded_state;
}

void JavaCertStore::manageCert(const std::string& cert_id, int state)
{
    // go through the certidicates and find the right one
    for (int i=0; i<no_certs; i++)
    {
        if (cert_id.compare(iCertsMetadata[i]->hash) == 0 || 
            iCertsMetadata[i]->pkey.find(cert_id) == 0)
        {
            switch (state)
            {
            case STATE_DELETED:
                // check if it can be deleted, before deleting it
                if (iCertsMetadata[i]->disposable)
                {
                    // mark down if there are any changes
                    iCertsMetadata[i]->changes =
                        (iCertsMetadata[i]->state != STATE_DELETED);
                    // do the actual state change
                    iCertsMetadata[i]->state = STATE_DELETED;
                }
                break;
            case STATE_DISABLED:
                // check if it can be disabled before disabling it
                if (iCertsMetadata[i]->disablable)
                {
                    // mark down if there are any changes
                    iCertsMetadata[i]->changes =
                        (iCertsMetadata[i]->state != STATE_DISABLED);
                    // do the actual state change
                    iCertsMetadata[i]->state = STATE_DISABLED;
                }
                break;
            case STATE_ENABLED:
                // mark down if there are any changes
                iCertsMetadata[i]->changes =
                    (iCertsMetadata[i]->state != STATE_ENABLED);
                // do the actual state change
                iCertsMetadata[i]->state = STATE_ENABLED;
                break;
            }
            // Do we need to do this write so often, or can we rely on
            // doing it only when this plugin is unloaded? this depends on
            // how reliable the stopPlugin() method is
            // -> to be checked with JavaCaptain
            writeMetadataIntoFile(iCertsMetadata[i]);
            break;
        }
    }
}

std::string JavaCertStore::readCert(const std::string& certFileName, long * length)
{
    char* data = NULL;
    FILE    *certFile;
    long    len = 0;
    certFile = fopen(certFileName.c_str(), "rb");
    if (certFile != NULL)
    {
        fseek(certFile, 0L, SEEK_END);
        len = ftell(certFile);
        if (len < 0)
        {
            fclose(certFile);
            return "";
        }
        fseek(certFile, 0L, SEEK_SET);
        data = (char*)calloc(len, sizeof(char));
        if (data != NULL)
        {
            fread(data, sizeof(char), len, certFile);
        }
        fclose(certFile);
    }
    *length = len;
    if (data != NULL)
    {
        return string(data, len);
    }
    else
    {
        return "";
    }
}

long JavaCertStore::replyWithContent(java::comms::CommsMessage& aReplyMsg, const CERT_METADATA& metadata, int format)
{
    long len = metadata.len;
    if (len != 0)
    {
        if (format == DER_FORMAT)
        {
            long long lLen = len;
            aReplyMsg << lLen;
            aReplyMsg << metadata.data;
        }
        else
        {
            std::string encCert = JavaCommonUtils::base64encode(metadata.data);
            long long lLen = encCert.size();
            aReplyMsg << lLen;
            aReplyMsg << encCert;
        }
    }
    else
    {
        // signal that cert content not available
        aReplyMsg << 0;
    }
    return len;
}

void JavaCertStore::assignProtectionDomain(vector<string> aTrustedUsage, CERT_METADATA * metadata)
{
    // by default assign it to trustedthird party
    metadata->prot_domain_name = string(KIdentifiedThirdPartyDomainName);
    metadata->prot_domain_category = string(KIdentifiedThirdPartyDomainCategory);
    // overwrite the identified third party with an operator if present
    for (int i=0; i<aTrustedUsage.size(); i++)
    {
        if (strcmp(aTrustedUsage[i].c_str(), KOperatorTrustedUsage) == 0
                || strcmp(aTrustedUsage[i].c_str(), KSupplementaryOperatorTrustedUsage1) == 0
                || strcmp(aTrustedUsage[i].c_str(), KSupplementaryOperatorTrustedUsage2) == 0
                || strcmp(aTrustedUsage[i].c_str(), KSupplementaryOperatorTrustedUsage3) == 0)
        {
            metadata->prot_domain_name = string(KOperatorDomainName);
            metadata->prot_domain_category = string(KOperatorDomainCategory);
            return;
        }
    }
}

void JavaCertStore::loadCertsMetadata(std::string aCertsLocation)
{
    const char * certs_dir = aCertsLocation.c_str();
    DIR * dirp = opendir(certs_dir);
    if (dirp != NULL)
    {
        struct dirent * dirent;
        while ((dirent = readdir(dirp)) != NULL)
        {
            char * tmp = dirent->d_name;
            bool cert_file = (strlen(dirent->d_name) > strlen(KCertSuffix1)
                              && strcmp(tmp + (strlen(dirent->d_name) - strlen(KCertSuffix1)),KCertSuffix1) == 0)
                             || (strlen(dirent->d_name) > strlen(KCertSuffix2)
                                 && strcmp(tmp + (strlen(dirent->d_name) - strlen(KCertSuffix2)),KCertSuffix2) == 0);
            if (strcmp(dirent->d_name,".")
                    && strcmp(dirent->d_name,"..")
                    && cert_file)
            {
                readCert(aCertsLocation, dirent->d_name);
            }
        }
        closedir(dirp);
    }
}

void JavaCertStore::loadCertsMetadata(std::string aCertsLocation, bool primaryPath)
{
    if (primaryPath || no_certs == 0)
    {
        loadCertsMetadata(aCertsLocation);
    }
    else
    {
        // secondary path: read only the roots listed into the rootslist
        std::string rootsListDir = "";
        std::string rootsListName = "";
        rootsListDir = "C";
        java::util::JavaOsLayer::getResRoot(rootsListDir, true);
        rootsListDir.append(KJavaRootsListDir);
        rootsListDir.append(1, KFileSeparator);
        rootsListName.append(KJavaRootsListName);
        FILE * rootslist = fopen((rootsListDir + rootsListName).c_str(),"r");
        if (rootslist != NULL)
        {
            // root_file_name
            int root_file_name_index = 0;
            char root_file_name[50];
            int retval;
            while ((int)(retval = getc(rootslist))!= EOF)
            {
                if (retval == 10 || retval == 13 /* CR or LF */)
                {
                    if (root_file_name_index > 0)
                    {
                        string rootFileName = string(root_file_name, root_file_name_index);
                        // reset the root file name for the next iteration
                        root_file_name_index  = 0;
                        root_file_name[root_file_name_index] = '\0';
                        readCert(rootsListDir, rootFileName);                
                    }
                }
                else
                {
                    root_file_name[root_file_name_index] = (char)retval;
                    root_file_name_index++;
                    root_file_name[root_file_name_index] = '\0';
                }
            }
            if (root_file_name_index > 0)
            {
                string rootFileName = string(root_file_name, root_file_name_index);
                // read the root
                readCert(rootsListDir, rootFileName);                
            }
            fclose(rootslist);
        }
    }
}

void JavaCertStore::readCert(const std::string& aCertsLocation, const std::string& aCertFileName)
{
    CERT_METADATA * metadata = new CERT_METADATA();
    if (readMetadataFromFiles(aCertsLocation, aCertFileName, metadata))
    {
        int len = aCertsLocation.size() + aCertFileName.size() + 1;
        metadata->full_path = aCertsLocation + aCertFileName;
        metadata->data = readCert(metadata->full_path,
                                &(metadata->len));
        // if the cert already exists, overwrite it: since the primary location is Z and
        // the secondary is C, this is a way to update certificates
        addCertMetadataToCache(metadata, true /* overwrite*/);
    }
    else
    {
        delete metadata;
        metadata = NULL;
    }
}


void JavaCertStore::addCertMetadataToCache(CERT_METADATA* metadata, bool overwrite)
{
    vector<CERT_METADATA *>::iterator startIterator;
    startIterator = iCertsMetadata.begin();
    bool found = false;
    for (int i=0; i<no_certs; i++)
    {
        if (iCertsMetadata[i]->hash.compare(metadata->hash) == 0 
            && iCertsMetadata[i]->pkey.find(metadata->pkey) == 0)
        {
            if (overwrite)
            {
                delete iCertsMetadata[i];
                iCertsMetadata.erase(startIterator + i);
                iCertsMetadata.push_back(metadata);
            }
            else
            {
                // just append it
                iCertsMetadata.push_back(metadata);
                no_certs++;
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        // simply add it
        iCertsMetadata.push_back(metadata);
        no_certs++;
    }
}

bool JavaCertStore::mkDirAll(const char* aDirPath, const std::string& aBaseDir)
{
    // split the path into single directories
    // (separated by file separator) and create
    // each of the directories
    std::string dirPath = string(aDirPath);
    std::string currentDirPath = "";
    int startPos = 0;
    int endPos = dirPath.find(KFileSeparator);
    while (endPos > startPos)
    {
        currentDirPath += dirPath.substr(startPos, endPos - startPos) + KFileSeparator;
        int mkdir_result = mkdir((aBaseDir + currentDirPath).c_str(), 0666);
        if (mkdir_result != 0 && errno != EEXIST)
        {
            return false;
        }
        startPos = endPos + 1;
        endPos = dirPath.find(KFileSeparator, startPos);
    }
    // the last round
    currentDirPath += dirPath.substr(startPos, dirPath.size() - startPos);
    int mkdir_result = mkdir((aBaseDir + currentDirPath).c_str(), 0666);
    if (mkdir_result != 0 && errno != EEXIST)
    {
        return false;
    }
    return true;
}

} // namespace captain
} // namespace java

