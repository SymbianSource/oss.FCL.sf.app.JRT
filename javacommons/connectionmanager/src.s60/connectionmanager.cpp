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
* Description:  Connection Manager
*                Provides Native conection management for all Java Connections:
                 - Network Access selection
*
*/


#include <memory>
#include <agentdialog.h>                    // RGenConAgentDialogServer
#include <commsdattypesv1_1.h>              // TMDBElementId
#include <logger.h>
#include <commdb.h>
#include <cmmanager.h>
#include <cmdefconnvalues.h>
#include <cmmanagerdef.h>
#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <javasymbianoslayer.h>
#include <cmdestination.h>
#include "connectionmanager.h"
#include "javacommonutils.h"
#include "s60commonutils.h"

using namespace java::storage;
using namespace CMManager;

_LIT8(KAskAlwaysId, "A");
_LIT8(KAskOnceId, "O");
_LIT8(KDefaultConn, "F");
_LIT8(KSnapId, "D");
_LIT8(KIapId, "I");


enum JavaNetworkAccessPointType
{
    ENapNone = 0,               ///< Not defined
    ENapAskAlways,              ///< Always ask
    ENapDefault,                ///< Default Conn
    ENapSnap,                   ///< SNAP ID is selected
    ENapIap,                    ///< IAP ID is selected
};


// Byte shift
const TInt KShift8 = 8;

// ---------------------------------------------------------------------------
// ConnectionManager::isIapDefault
// ---------------------------------------------------------------------------
//

EXPORT_C bool ConnectionManager::isIapDefault(TUint32 aMatchIapId, TUint32 aDestId, bool aDefault)
{
	  LOG(ESOCKET,EInfo,"+ConnectionManager::isIapDefault");
	  bool retVal = false;
    TRAPD(err,retVal = checkIapDefaultL(aMatchIapId,aDestId,aDefault));
    if(err!=KErrNone)
    {
        // error occured
        return false;
    }
    return retVal;
}

bool ConnectionManager::checkIapDefaultL(TUint32 aMatchIapId, TUint32 aDestId, bool aDefault)
{
	
    LOG(ESOCKET,EInfo,"+ConnectionManager::checkIapDefaultL");
    TUint32 id = aDestId;
    TUint32 tmpapId;
    TCmDefConnType type;

    RCmManager * mgr = new(ELeave) RCmManager();
    mgr->OpenL();

    if (aDefault)
    {
        TCmDefConnValue obj;
        mgr->ReadDefConnL(obj);
        id = obj.iId;
        type = obj.iType;
        if ((type ==ECmDefConnConnectionMethod) && (id == aMatchIapId))
        {
            return true;
        }
        else if (type != ECmDefConnDestination)
        {
            return false;
        }
    }

    RCmDestination tmpdst =     mgr->DestinationL(id);
    ILOG1(ESOCKET,"Is connected return value = %d ", tmpdst.IsConnectedL());
    if (!(tmpdst.IsConnectedL()))
    {
        return false;   // no access point within this destination are active
    }
    for (TInt j = 0; j < tmpdst.ConnectionMethodCount(); j++)
    {
        tmpapId = tmpdst.ConnectionMethodL(j).GetIntAttributeL(ECmIapId);
        ILOG1(ESOCKET,"tmpai id = %d   ",tmpapId);
        if (tmpapId == aMatchIapId)
        {
            return true;
        }
    }
    return false;	
	
	
}

// ---------------------------------------------------------------------------
// JavaNetworkAccessRepository::GetDeviceDefaultAccessPointL
// ---------------------------------------------------------------------------
//
void ConnectionManager
::GetDeviceDefaultAccessPointL(char * access)
{
    int id;
    TCmDefConnType type;
    LOG(ESOCKET,EInfo,"+GetDeviceDefaultAccessPointL");

    RCmManager * mgr = new(ELeave) RCmManager();
    mgr->OpenLC();
    TCmDefConnValue obj;
    HBufC8 * des;


    mgr->ReadDefConnL(obj);
    id = obj.iId;
    type = obj.iType;
    LOG1(ESOCKET,EInfo,"readDefConnL returned %d",id);
    LOG1(ESOCKET,EInfo,"type is %d",type);
    CleanupStack::PopAndDestroy();


    if (type == ECmDefConnAskOnce || type == ECmDefConnConnectionMethod || type == ECmDefConnDestination)
    {
        des = CreateDescriptorL(type,id);
    }
    else
    {
        des = PromptUserSelectNetworkAccessPointL();
        LOG(ESOCKET,EInfo," after PromptUserSelectNetworkAccessPointL ");
    }
    TInt len = des->Length();
    LOG1(ESOCKET,EInfo,"+des length =  %d",len);
    LOG(ESOCKET,EInfo,"after lenght");
    memcpy(access, des->Des().PtrZ(),len+1);
    LOG(ESOCKET,EInfo,"after memcpy");
    LOG1(ESOCKET,EInfo,"+apnInfo =  %s",access);
    LOG1(ESOCKET,EInfo,"+apnInfo =  %s",access);
    CleanupStack::PopAndDestroy(des);
}

void ConnectionManager
::PromptUserL(char * access)
{
    HBufC8 * des;
    des = PromptUserSelectNetworkAccessPointL();
    TInt len = des->Length();
    memcpy(access, des->Des().PtrZ(),len+1);
    CleanupStack::PopAndDestroy(des);
}

// ---------------------------------------------------------------------------
// JavaNetworkAccessRepository::PromptUserSelectNetworkAccessL
// ---------------------------------------------------------------------------
//
HBufC8 *ConnectionManager
::PromptUserSelectNetworkAccessPointL()
{
    LOG(ESOCKET,EInfo,"+PromptUserSelectNetworkAccessPointL");
    TUint32 id((TUint32)KJavaNetworkAccessNotSpecified);
    TUint32 snapOrIapId(0);

    TConnectionPrefs prefs;
    prefs.iRank = 1;
    prefs.iDirection = ECommDbConnectionDirectionOutgoing;
    prefs.iBearerSet = ECommDbBearerCSD | ECommDbBearerWcdma
                       | ECommDbBearerLAN | ECommDbBearerVirtual
#ifdef RD_JAVA_SUPPORT_CDMA
                       | ECommDbBearerCdma2000
#endif
                       | ECommDbBearerPSD
                       | ECommDbBearerPAN | ECommDbBearerWLAN;

    TUint32 dummy(0);

    RGenConAgentDialogServer dlgSv;
    CleanupClosePushL(dlgSv);
    User::LeaveIfError(dlgSv.Connect());


    TRequestStatus status(KRequestPending);
    LOG(ESOCKET,EInfo,"prompting by regenconagent ");
    dlgSv.AccessPointConnection(dummy, dummy, snapOrIapId, prefs.iBearerSet, status);
    User::WaitForRequest(status);
    CleanupStack::PopAndDestroy(&dlgSv);
    User::LeaveIfError(status.Int());


    TMDBElementId tableId = snapOrIapId & KCDMaskShowRecordType;
    if (tableId == KCDTIdNetworkRecord)
    {
        // User selects SNAP
        LOG(ESOCKET,EInfo,"user selected SNAP");
        id = (snapOrIapId & KCDMaskShowRecordId) >> KShift8;
        return CreateDescriptorL(2, id);
    }
    else if (tableId == KCDTIdIAPRecord)
    {
        // User selects IAP
        LOG(ESOCKET,EInfo,"user selected IAP");
        id = (snapOrIapId & KCDMaskShowRecordId) >> KShift8;
        return CreateDescriptorL(3, id);
    }
    else
    {
        // Uknown selection
    }
    return 0;
}

// ---------------------------------------------------------------------------
// NetworkAccessRepository::DescriptorLC
// ---------------------------------------------------------------------------
//
HBufC8 * ConnectionManager::CreateDescriptorL(const int aType, const int id)
{
    LOG(ESOCKET,EInfo,"+ConnectionManager::CreateDescriptorL");
    HBufC8* des = HBufC8::NewLC(256);
    TPtr8 ptr(des->Des());
    switch (aType)
    {
    case ECmDefConnAlwaysAsk :
        ptr = KAskAlwaysId();
        break;
    case ECmDefConnAskOnce :
        ptr = KAskOnceId();
        break;

    case ECmDefConnDestination :
        if (id == KDefaultConnection)
        {
            // case of defaultconnection set in app settings
            // store 'F' into java storage
            ptr = KDefaultConn();
        }
        else
        {
            ptr = KSnapId();
            ptr.AppendNum(id);
        }
        break;
    case ECmDefConnConnectionMethod :
        LOG(ESOCKET,EInfo,"ECmDefConnConnectionMethod case");
        ptr = KIapId();
        ptr.AppendNum(id);
        break;
    default :
        User::Leave(KErrNotSupported);
        break;
    }
    // CleanupStack::PopAndDestroy();
    LOG(ESOCKET,EInfo,"-ConnectionManager::CreateDescriptorL");
    return des;
}

EXPORT_C unsigned int ConnectionManager::getDestinationNetworkIdL(Uid aAppSuiteUid)
{
    JavaStorageApplicationEntry_t entries;
    std::wstring temp;
    JavaStorageEntry attribute;
    int snapid = KJavaNetworkAccessNotSpecified;
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    js->open(JAVA_DATABASE_NAME);


    // Read all attributes of one application specified by UID.
    js->read(APPLICATION_PACKAGE_TABLE, aAppSuiteUid, entries);
    js->close();

    // set the entry as ACCESS_POINT
    attribute.setEntry(ACCESS_POINT, L"");

    // Get value of the attribute from read attributes.
    while (!entries.empty())
    {
        JavaStorageApplicationEntry_t::const_iterator findIterator = entries.find(attribute);

        if (findIterator != entries.end())
        {
            JavaStorageEntry sourceEntry = (*findIterator);
            temp.append(sourceEntry.entryValue().c_str());
            HBufC * value = S60CommonUtils::wstringToDes(temp.c_str());
            HBufC8 * temp1 = HBufC8::NewL(value->Des().Length());
            temp1->Des().Copy(value->Des());
            TRAP_IGNORE(snapid = ParseNetworkAccessPointL(*temp1));
        }
        else
        {
            LOG(ESOCKET,EInfo,"Name attribute does not exists.");
        }
        entries.clear();
    }
    return snapid;
}

EXPORT_C unsigned int ConnectionManager::getApnIdL(Uid aAppSuiteUid)
{
    int apnid = KJavaNetworkAccessNotSpecified;
    JavaStorageApplicationEntry_t entries;
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open();
        js->read(APPLICATION_PACKAGE_TABLE, aAppSuiteUid, entries);
        js->close();
    }
    catch (JavaStorageException& aJse)
    {
        LOG1(ESOCKET, EInfo,"APN read error: %s", aJse.toString().c_str());
        return apnid;
    }

    JavaStorageEntry attribute;
    attribute.setEntry(ACCESS_POINT, L"");

    JavaStorageApplicationEntry_t::const_iterator findIterator
    = entries.find(attribute);

    if (findIterator != entries.end())
    {
        std::auto_ptr<HBufC>value(S60CommonUtils::wstringToDes(
                                      (*findIterator).entryValue().c_str()));
        std::auto_ptr<HBufC8> temp1(HBufC8::NewL(value->Des().Length()));
        temp1->Des().Copy(value->Des());
        TRAP_IGNORE(apnid = ParseNetworkAccessPointL(*temp1));
    }
    else
    {
        WLOG1WSTR(ESOCKET, "APN ID not found for the Application: %s",
                  aAppSuiteUid.toString());
    }
    return apnid;
}


EXPORT_C void ConnectionManager::setApnIdL(Uid aAppSuiteUid,int aApnId)
{
    // only IAP ids supported
    std::auto_ptr<HBufC8>des(CreateDescriptorL(ECmDefConnConnectionMethod, aApnId));
    CleanupStack::Pop(); // Pop CreateDescriptorL added HBufC from cleanupstack.

    std::auto_ptr<HBufC> temp1(HBufC::NewL(des->Des().Length() + 1));
    temp1->Des().Copy(des->Des());

    TPtr ptr = temp1->Des();
    std::wstring apnString(desToWstring(ptr));

    LOG1(ESOCKET,EInfo,"ConnectionManager::setApnIdL() - len of apnString = %d",
         apnString.size());

    LOG1WSTR(ESOCKET,
             EInfo,
             "COnnectionManager::setApnIdL() - storing %s into javastorage",
             apnString);

    JavaStorageApplicationEntry_t matchEntries;
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open();
        // Check application really exists there. Otherwise orphaned column is
        // written to storage.
        js->read(APPLICATION_PACKAGE_TABLE, aAppSuiteUid, matchEntries);
        if (matchEntries.size() > 0)
        {
            JavaStorageApplicationEntry_t updateEntries;
            JavaStorageEntry attribute;

            attribute.setEntry(ACCESS_POINT, apnString);
            updateEntries.insert(attribute);

            matchEntries.clear();
            attribute.setEntry(ID, aAppSuiteUid.toString());
            matchEntries.insert(attribute);

            js->update(APPLICATION_PACKAGE_TABLE, updateEntries, matchEntries);
        }
    }
    catch (JavaStorageException& aJse)
    {
        WLOG1(ESOCKET, "Connection manager failed to store access point: %s", aJse.toString().c_str());
    }

    js->close();
}
EXPORT_C void ConnectionManager::setDestinationNetworkIdL(Uid aAppSuiteUid,int aSnapId)
{
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    JavaStorageApplicationEntry_t updateEntries;
    JavaStorageApplicationEntry_t matchEntries;
    JavaStorageEntry attribute;
    HBufC8* des = 0;
    try
    {
        js->open(JAVA_DATABASE_NAME);
    }
    catch (JavaStorageException jse)
    {
        LOG(ESOCKET,EInfo,"opening javastorage db failed");
    }

    // only SNAP ids supported
    if (aSnapId == KAlwaysAskId)
    {
        des = CreateDescriptorL(ECmDefConnAlwaysAsk,KAlwaysAskId);
    }
    else
    {
        if (aSnapId == KDefaultConnection)
        {
            des = CreateDescriptorL(ECmDefConnDestination,KDefaultConnection);
        }
        else
        {
            des = CreateDescriptorL(ECmDefConnDestination,aSnapId);
        }
    }

    HBufC* temp1 = HBufC::NewL(des->Des().Length()+1);
    temp1->Des().Copy(des->Des());

    int len1 = des->Length();
    int len2 = temp1->Length();
    LOG2(ESOCKET,EInfo,"ConnectionManager::setApnIdL() - len1= %d, len2 = %d",len1, len2);
    wchar_t* apnString = new wchar_t[len2+1];

    TPtr ptr = temp1->Des();
    TPtr ptr16((TUint16*)apnString,len2+1);

    ptr16.Copy(ptr);
    ptr16.SetLength(len2);
    ptr16.ZeroTerminate();

    LOG1(ESOCKET,EInfo,"ConnectionManager::setApnIdL() - len of apnString = %d", wcslen(apnString));

    std::wstring tmpstring(apnString);
    char *dirName = JavaCommonUtils::wstringToUtf8(tmpstring);
    LOG1(ESOCKET,EInfo,"COnnectionManager::setApnIdL() - storing %s into javastorage",dirName);


    js->startTransaction();

    attribute.setEntry(ACCESS_POINT, apnString);
    updateEntries.insert(attribute);

    attribute.setEntry(ID, aAppSuiteUid.toString());
    matchEntries.insert(attribute);


    LOG(ESOCKET,EInfo,"ConnectionManager::setApnIdL() - before js update");
    try
    {
        js->update(APPLICATION_PACKAGE_TABLE, updateEntries, matchEntries);
    }
    catch (JavaStorageException jse)
    {
        ELOG1(ESOCKET,"Updating the javastorage db failed %S",jse.toString().c_str());
        js->close();
        CleanupStack::PopAndDestroy(des);
        return;
    }

    try
    {
        js->commitTransaction();
    }
    catch (JavaStorageException jse)
    {
        ELOG(ESOCKET,"Commit transaction in javastorage db failed");
        js->close();
        CleanupStack::PopAndDestroy(des);
        return;
    }

    js->close();
    updateEntries.clear();
    matchEntries.clear();
    CleanupStack::PopAndDestroy(des);
}

unsigned int ConnectionManager::ParseNetworkAccessPointL(const TDesC8& aDes)
{
    JavaNetworkAccessPointType type(ENapNone);
    TUint32 id = (TUint32)KJavaNetworkAccessNotSpecified;

    TLex8 charLexer(aDes);
    charLexer.Mark();
    // First character in descriptor MUST indicates Network Access type <(A)|(D|I)ID>
    if (!(charLexer.Peek()).IsAlpha())
    {
        // First character in descriptor IS NOT alpha
        User::Leave(KErrCorrupt);
    }

    // First character in descriptor IS alpha
    charLexer.Inc();
    TPtrC8 parsedText(charLexer.MarkedToken());

    // Parse what Network Access type is indicated
    if (parsedText.Compare(KAskAlwaysId) == 0)
    {
        type = ENapAskAlways;
        id = (TUint32) KAlwaysAskId;
    }
    // In case of default connection, equal to'F' case
    // return KDefaultConnection to app mgr
    else if (parsedText.Compare(KDefaultConn) == 0)
    {
        type = ENapDefault;
        id = (TUint32) KDefaultConnection;
    }
    else if (parsedText.Compare(KSnapId) == 0)
    {
        type = ENapSnap;
    }
    else if (parsedText.Compare(KIapId) == 0)
    {
        type = ENapIap;
    }
    else
    {
        // First character IS NOT pre-defined indicator
        User::Leave(KErrCorrupt);
    }

    if (type == ENapAskAlways || type == ENapDefault)
    {
        // Descriptor MUST contain only indicator
        if (!charLexer.Eos())
        {
            // Descriptor contains more than indicator
            User::Leave(KErrArgument);
        }
        // Success go to end
    }
    else
    {
        // Descriptor MUST contain only indicator and Network Access Point ID
        if (charLexer.Eos())
        {
            // Descriptor contains only indicator
            User::Leave(KErrCorrupt);
        }

        charLexer.Mark();
        charLexer.Inc();
        // Parse Network Access Point ID
        while ((charLexer.Peek()).IsDigit())
        {
            charLexer.Inc();    // move to next char
        }

        // get token
        TPtrC8 parsedNumber = charLexer.MarkedToken();
        TLex8 numLexer(parsedNumber);
        // try to convert token to number
        if (numLexer.Val(id, EDecimal) != KErrNone)
        {
            // Converting fails
            User::Leave(KErrCorrupt);
        }

        // Descriptor MUST contain only indicator and Network Access Point ID
        if (!charLexer.Eos())
        {
            // Descriptor contains more than indicator and Network Access ID
            User::Leave(KErrCorrupt);
        }
        // Success go to end
    }
    return id;
}
