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


#include <w32std.h>
#include <apgtask.h>
#include <hbdevicenotificationdialogsymbian.h>
#include <hbsymbianvariant.h>
#include "CSoftNotification.h"

_LIT(KMidletId, "MidletId");
_LIT(KNotificationId, "NotificationId");
_LIT(KIndicatorTypeJavaSoftNote,"com.nokia.javasoftnotification.indicatorplugin/1.0_%u_%i");
const TInt KIndicatorTypeStringLength = 250;
const TInt KMidletUidLength = 50;

// Event id when user accepted notification
const TInt KEventNoteAccepted = 1;

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
    iNotificationId(aNotificationId)
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
    iIndicator = CHbIndicatorSymbian::NewL();
    iIndicator->SetObserver(this);
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
    
    if (iPeer)
        {
        iJniEnv->DeleteWeakGlobalRef((jweak)iPeer);
        }
    
    iPrimaryText = NULL;
    iSecondaryText = NULL;
    iImagePath = NULL;
    iIndicator = NULL;
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
    CHbDeviceNotificationDialogSymbian* dialog =
            CHbDeviceNotificationDialogSymbian::NewL();
    CleanupStack::PushL(dialog);
    
    if(iPrimaryText != NULL)
        {
        dialog->SetTitleL(iPrimaryText->Des());
        }

    if(iSecondaryText != NULL)
        {
        dialog->SetTextL(iSecondaryText->Des());
        }
    
    if (iImagePath != NULL)
        {
        dialog->SetIconNameL(iImagePath->Des());
        }

    dialog->ShowL();
  
    CHbSymbianVariant* varValues = NotificationParamsL();
    CleanupStack::PushL(varValues);
    
    TBuf<KIndicatorTypeStringLength> indicatorType;
    indicatorType.Format(KIndicatorTypeJavaSoftNote, iMidletId, iNotificationId);
    bool indStatus = iIndicator->Activate(indicatorType, varValues);
    
    CleanupStack::PopAndDestroy(varValues);
    CleanupStack::PopAndDestroy(dialog);
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
// CSoftNotification::FillNotificationParams
// -----------------------------------------------------------------------------
//
CHbSymbianVariant* CSoftNotification::NotificationParamsL()
    {
    //only five parameter, so creat the array 
    //for five parameters to fit and append them
    CDesC16ArrayFlat* array = new CDesC16ArrayFlat(5);
    CleanupStack::PushL(array);
    TBuf<KMidletUidLength> midletId;
    _LIT(MidletId,"%u");
    midletId.Format(MidletId,iMidletId);
    array->AppendL(midletId);
    array->AppendL(reinterpret_cast<TDesC16&>(iNotificationId));
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
