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

#ifndef JAVACERTSTORE_H
#define JAVACERTSTORE_H

#include "commslistener.h"
#include "javaosheaders.h"
#include "extensionplugininterface.h"
#include "javacommonutils.h"
#include <vector>
#include <string>

namespace java
{
namespace comms
{
class CommsMessage;
}

namespace captain
{

class CoreInterface;

#ifdef __SYMBIAN32__
const char KFileSeparator = '\\';
const char KJavaCertsDir[] = "security\\trustroots\\device\\certificates";
const char KJavaCertsStateDir[] = "security\\trustroots\\device\\state";
const char KJavaRootsListDir[] = "security\\trustroots";
#else //__SYMBIAN32__
const char KFileSeparator = '/';
const char KJavaCertsDir[] = "security/trustroots/device/certificates";
const char KJavaCertsStateDir[] = "security/trustroots/device/state";
const char KJavaRootsListDir[] = "security/trustroots";
#endif //__SYMBIAN32__
const char KJavaRootsListName[] = "midprootslist";
const char KMetadataSuffix[] = ".metadata";
const char KStateSuffix[] = ".state";
const char KCertSuffix1[] = ".der";
const char KCertSuffix2[] = ".cer";
const char KOperatorTrustedUsage[] = "1.3.6.1.4.1.42.2.110.2.2.2.1";
const char KSupplementaryOperatorTrustedUsage1[] = "1.3.6.1.4.1.42.2.110.2.2.2.4";
const char KSupplementaryOperatorTrustedUsage2[] = "1.3.6.1.4.1.42.2.110.2.2.2.5";
const char KSupplementaryOperatorTrustedUsage3[] = "1.3.6.1.4.1.42.2.110.2.2.2.6";
const char KOperatorDomainName[] = "Operator";
const char KOperatorDomainCategory[] = "OPD";
const char KIdentifiedThirdPartyTrustedUsage[] = "1.3.6.1.4.1.42.2.110.2.2.2.3";
const char KIdentifiedThirdPartyDomainName[] = "IdentifiedThirdParty";
const char KIdentifiedThirdPartyDomainCategory[] = "ITPD";
const int PEM_FORMAT = 1;
const int DER_FORMAT = 2;

typedef struct cert_metadata_st
{
    std::string full_path;
    std::string hash;
    std::string pkey;
    int state;
    bool disposable;
    bool disablable;
    std::string prot_domain_name;
    std::string prot_domain_category;
    // internal flag highlighting
    // any changes in the metadata
    bool changes;
    // the name of the external
    // file from where the metadata
    // was read
    std::string file_name;
    // the content/data of the certificate
    std::string data;
    // size of the certificate
    long len;
} CERT_METADATA;

OS_NONSHARABLE_CLASS(JavaCertStore) :  public java::comms::CommsListener,
        public ExtensionPluginInterface
{
public:
    JavaCertStore();
    virtual ~JavaCertStore();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // ExtensionPluginInterface methods
    virtual java::comms::CommsListener* getCommsListener();

protected:
    // CommsListener methods
    virtual void processMessage(java::comms::CommsMessage& aMessage);

private:
    void queryCerts(java::comms::CommsMessage& aMessage);
    void loadCertsMetadata();
    void loadCertsMetadata(std::string aCertsLocation);
    void loadCertsMetadata(std::string aCertsLocation, bool primaryPath);
    void unloadCertsMetadata();
    bool readMetadataFromFiles(const std::string& aCertsLocation, const std::string& certFileName, CERT_METADATA *);
    void writeMetadataIntoFile(CERT_METADATA * metadata);
    int encodeState(bool, bool, int);
    void manageCert(const std::string&, int);
    std::string readCert(const std::string& certFileName, long *);
    void readCert(const std::string& aCertsLocation, const std::string& aCertFileName);
    long replyWithContent(java::comms::CommsMessage& aReplyMessage, const CERT_METADATA&, int format = DER_FORMAT);
    void assignProtectionDomain(std::vector<std::string>, CERT_METADATA *);
    void addCertMetadataToCache(CERT_METADATA* metadata, bool overwrite);
    bool mkDirAll(const char* dirPath, const std::string& aBaseDir);

private:
    CoreInterface* iCore;
    std::string iPrimaryCertsPath;
    std::string iSecondaryCertsPath;
    std::string iCertsMetadataPath;
    int no_certs;
    std::vector<CERT_METADATA*> iCertsMetadata;
};

} // namespace captain
} // namespace java

#endif // JAVACERTSTORE_H

