/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <apgtask.h>
#include <hbdevicenotificationdialogsymbian.h>
#include <hbsymbianvariant.h>

#include <memory>
#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <JavaCommonUtils.h>
#include <logger.h>

#include "CSoftNotification.h"

_LIT(KMidletId, "MidletId");
_LIT(KNotificationId, "NotificationId");
_LIT(KIndicatorTypeJavaSoftNote,"com.nokia.javasoftnotification.indicatorplugin/1.0_%u_%i");
const TInt KIndicatorTypeStringLength = 250;
const TInt KMidletUidLength = 50;

// Event id when user accepted notification
const TInt KEventNoteAccepted = 1;

using namespace std;
using namespace java::storage;
using namespace java::util;

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSoftNotification::NewLC
// Static constructor
// -----------------------------------------------------------------------------
//
CSoftNotification* CSoftNotification::NewL(TInt aAppId, TInt aNotificationId)
    {
    CSoftNotification* self = new (ELeave) CSoftNotification(aAppId,
            aNotificationId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::CSoftNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSoftNotification::CSoftNotification(TInt aAppId, TInt aNotificationId) :
    iNotificationId(aNotificationId),iIsNoteExist(EFalse)
    {
    iMidletId = TUid::Uid(aAppId);
    }

// -----------------------------------------------------------------------------
// CSoftNotification::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSoftNotification::ConstructL()
    {
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    TInt maxValue = 0;
    try
        {
        js->open(JAVA_DATABASE_NAME);
        JavaStorageEntry attribute;
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t foundEntries;

        std::wstring midletId = JavaCommonUtils::intToWstring(iMidletId.iUid);
        attribute.setEntry(SOFTNOTE_MIDLET_ID, midletId);
        findPattern.insert(attribute);

        js->search(JAVA_SOFTNOTE_TABLE, findPattern, foundEntries);
        js->close();
        findPattern.clear();

        // like to know softnote id.
        attribute.setEntry(SOFTNOTE_ID, L"");
        // Iterate through all rows matched with search patterns.
        JavaStorageApplicationList_t::const_iterator applications;
        for(applications = foundEntries.begin(); applications !=foundEntries.end(); applications++)
            {
            // Get softnote ID from its parameters.
            JavaStorageApplicationEntry_t::const_iterator findIterator = (*applications).find(attribute);
            if (findIterator != (*applications).end())
                {
                TInt id = JavaCommonUtils::wstringToInt((*findIterator).entryValue());
                if(iNotificationId == id)
                    {
                    iIsNoteExist = true;
                    }
                if(maxValue<id)
                    {
                    maxValue = id;
                    }
                }
            }
        foundEntries.clear();
        }
    catch (JavaStorageException& ex)
        {
        LOG1(EJavaStorage, EInfo," JavaSoftNote Exception %S", ex.toString().c_str());
        }
    
    if(!iIsNoteExist && iNotificationId == 0)
        {
        iNotificationId = maxValue + 1;
        }
    if(iIndicator == NULL)
        {
        iIndicator = CHbIndicatorSymbian::NewL();
        iIndicator->SetObserver(this);
        }
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSoftNotification::~CSoftNotification()
    {
    if (iPrimaryText)
        {
        delete iPrimaryText;
        }
    
    if (iSecondaryText)
        {
        delete iSecondaryText;
        }
    
    if (iImagePath)
        {
        delete iImagePath;
        }
    
    if (iIndicator)
        {
        delete iIndicator;
        }
    
    if(iNotificationDialog)
        {
        delete iNotificationDialog;
        }
    
    if (iPeer)
        {
        iJniEnv->DeleteWeakGlobalRef((jweak)iPeer);
        }
    
    iPrimaryText = NULL;
    iSecondaryText = NULL;
    iImagePath = NULL;
    iIndicator = NULL;
    iNotificationDialog = NULL;
    iPeer = NULL;
    iMethodId = NULL;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::ShowSoftNotificationL
// Displays a soft notification
// -----------------------------------------------------------------------------
//
bool CSoftNotification::ShowSoftNotificationL()
    {
    AddorUpdateSoftNotificationToStorage();
    
    if(iNotificationDialog == NULL)
        {
        iNotificationDialog = CHbDeviceNotificationDialogSymbian::NewL(this);
        }
    
    if(iPrimaryText != NULL)
        {
        iNotificationDialog->SetTitleL(iPrimaryText->Des());
        }

    if(iSecondaryText != NULL)
        {
        iNotificationDialog->SetTextL(iSecondaryText->Des());
        }
    
    if (iImagePath != NULL)
        {
        iNotificationDialog->SetIconNameL(iImagePath->Des());
        }
    iNotificationDialog->EnableTouchActivation(ETrue);
    iNotificationDialog->ShowL();
  
    // reading Softnote data
    CHbSymbianVariant* varValues = NotificationParamsL();
    CleanupStack::PushL(varValues);
    
    // creating indicator in status menu
    TBuf<KIndicatorTypeStringLength> indicatorType;
    indicatorType.Format(KIndicatorTypeJavaSoftNote, iMidletId, iNotificationId);
    bool indStatus = iIndicator->Activate(indicatorType, varValues);
    
    CleanupStack::PopAndDestroy(varValues);
    return indStatus;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::RemoveSoftNotificationL
// Cancels and removes the soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::RemoveSoftNotificationL()
    {
    TBuf<KIndicatorTypeStringLength> type;
    type.Format(KIndicatorTypeJavaSoftNote, iMidletId, iNotificationId);
    iIndicator->Deactivate(type);
    }

// -----------------------------------------------------------------------------
// CSoftNotification::SetTextL
// Sets a text for a soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetTextL(const TDesC& aPrimaryText, const TDesC& aSecondaryText)
    {
    HBufC* txt = aPrimaryText.AllocL();
    delete iPrimaryText;
    iPrimaryText = txt;

    txt = aSecondaryText.AllocL();
    delete iSecondaryText;
    iSecondaryText = txt;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::SetImagePathL
// Sets a text for a soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetImagePathL(const TDesC& aImagePath)
    {
    HBufC* txt = aImagePath.AllocL();
    delete iImagePath;
    iImagePath = txt;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::Id
// Notification Id
// -----------------------------------------------------------------------------
//
TInt CSoftNotification::Id()
    {
    return iNotificationId;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::SetPeerObserverL
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetPeerObserver(jobject aPeer, jmethodID aMethodId,
        JNIEnv* aJniEnv)
    {
    iJniEnv = aJniEnv;
    iPeer = aPeer;
    iMethodId = aMethodId;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::IndicatorUserActivated
// Dynamic soft notification was accepted by user.
// -----------------------------------------------------------------------------
//
void CSoftNotification::IndicatorUserActivated(const TDesC& /*aType*/,
        CHbSymbianVariantMap &aData)
    {
    TInt notificationId = *reinterpret_cast<const TInt*>(aData.Get(KNotificationId)->Data());
    TUid midletId(TUid::Uid(*reinterpret_cast<const TUint*>(aData.Get(KMidletId)->Data())));
    if (iPeer && iMidletId == midletId &&
        iNotificationId == notificationId )
        {
        // Can't attempt callback if JVM is processing an exception. Event is
        // possibly lost by Java listener.
        if (iJniEnv->ExceptionCheck() == JNI_TRUE)
            {
            return;
            }
        if (iMethodId)
            {
            iJniEnv->CallVoidMethod(iPeer, iMethodId, KEventNoteAccepted);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSoftNotification::NotificationDialogActivated
// Dynamic soft notification was accepted by user.
// -----------------------------------------------------------------------------
//
void CSoftNotification::NotificationDialogActivated(const CHbDeviceNotificationDialogSymbian* /*aDialog*/)
  {
    RWsSession ws;
    User::LeaveIfError(ws.Connect() == KErrNone);
    TApaTaskList tasklist(ws);
    TApaTask task = tasklist.FindApp(iMidletId);
    if (task.Exists())
        {
        task.BringToForeground();
        ws.Close();
        }
    }

// -----------------------------------------------------------------------------
// CSoftNotification::NotificationDialogClosed
// Dynamic soft notification was closed.
// -----------------------------------------------------------------------------
//
void CSoftNotification::NotificationDialogClosed(const CHbDeviceNotificationDialogSymbian* /*aDialog*/,
        TInt /*aCompletionCode*/)
    {
    
    }

// -----------------------------------------------------------------------------
// CSoftNotification::FillNotificationParams
// -----------------------------------------------------------------------------
//
CHbSymbianVariant* CSoftNotification::NotificationParamsL()
    {
    //only five parameter, so creat the array with size 5.
    CDesC16ArrayFlat* array = new CDesC16ArrayFlat(5);
    CleanupStack::PushL(array);
    TBuf<KMidletUidLength> midletId;
    _LIT(MidletId,"%u");
    midletId.Format(MidletId,iMidletId);
    array->AppendL(midletId);
    
    TBuf<10> noteId;
    _LIT(NoteId,"%d");
    noteId.Format(NoteId,iNotificationId);
    array->AppendL(noteId);

    if(iPrimaryText != NULL)
        {
        array->AppendL(iPrimaryText->Des());
        }
    else
        {
        array->AppendL(KNullDesC);
        }
    
    if(iSecondaryText != NULL)
        {
        array->AppendL(iSecondaryText->Des());
        }
    else
        {
        array->AppendL(KNullDesC);
        }
    
    if (iImagePath != NULL)
        {
        array->AppendL(iImagePath->Des());
        }
    else
        {
        array->AppendL(KNullDesC);
        }
    MDesCArray* marray = static_cast<MDesCArray*>(array);
    CHbSymbianVariant* varValues = CHbSymbianVariant::NewL( marray, CHbSymbianVariant::EDesArray);
    CleanupStack::PopAndDestroy(array);
    return varValues;
    }

// -----------------------------------------------------------------------------
// CSoftNotification::AddSoftNotificationToStorage
// -----------------------------------------------------------------------------
//
void CSoftNotification::AddorUpdateSoftNotificationToStorage()
    {
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    try
        {
        js->open(JAVA_DATABASE_NAME);
        JavaStorageApplicationEntry_t newEntries;
        JavaStorageApplicationEntry_t matchEntries;
        JavaStorageEntry attribute;

        if (!iIsNoteExist)
            {
            // if Softnote is not created, add midletId and notificationId to storage
            std::wstring midletId = JavaCommonUtils::intToWstring(
                    iMidletId.iUid);
            attribute.setEntry(SOFTNOTE_MIDLET_ID, midletId,
                    JavaStorageEntry::STRING);
            newEntries.insert(attribute);

            std::wstring notificationId = JavaCommonUtils::intToWstring(
                    iNotificationId);
            attribute.setEntry(SOFTNOTE_ID, notificationId,
                    JavaStorageEntry::INT);
            newEntries.insert(attribute);
            }
        else
            {
            // if Softnote is already in the storage, just aupdate the data.
            std::wstring midletId = JavaCommonUtils::intToWstring(
                    iMidletId.iUid);
            attribute.setEntry(SOFTNOTE_MIDLET_ID, midletId,
                    JavaStorageEntry::STRING);
            matchEntries.insert(attribute);

            std::wstring notificationId = JavaCommonUtils::intToWstring(
                    iNotificationId);
            attribute.setEntry(SOFTNOTE_ID, notificationId,
                    JavaStorageEntry::INT);
            matchEntries.insert(attribute);
            }

        if (iPrimaryText)
            {
            std::wstring primaryText((wchar_t*) iPrimaryText->Ptr(),
                    iPrimaryText->Length());
            attribute.setEntry(SOFTNOTE_PRIMARY_TEXT, primaryText,
                    JavaStorageEntry::STRING);
            newEntries.insert(attribute);
            }
        if (iSecondaryText)
            {
            std::wstring secondaryText((wchar_t*) iSecondaryText->Ptr(),
                    iSecondaryText->Length());
            attribute.setEntry(SOFTNOTE_SECONDARY_TEXT, secondaryText,
                    JavaStorageEntry::STRING);
            newEntries.insert(attribute);
            }
        if (iImagePath)
            {
            std::wstring imagePath((wchar_t*) iImagePath->Ptr(),
                    iImagePath->Length());
            attribute.setEntry(SOFTNOTE_IMAGE_PATH, imagePath,
                    JavaStorageEntry::STRING);
            newEntries.insert(attribute);
            }

        if (!iIsNoteExist)
            {
            // create new entry in storage if softnote is not created earlier
            js->write(JAVA_SOFTNOTE_TABLE, newEntries);
            }
        else
            {
            // update data for softNote. 
            js->update(JAVA_SOFTNOTE_TABLE, newEntries, matchEntries);
            }
        js->close();
        newEntries.clear();
        matchEntries.clear();
        }
    catch (JavaStorageException& ex)
        {
        LOG1(EJavaStorage, EInfo," JavaSoftNote Exception %S", ex.toString().c_str());
        }
    }
