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
* Description:  Versit serializer native side.
 *
*/


// INCLUDE FILES
#include "cpimserializer.h"
#include "cpimmanager.h"
#include "cpimversit.h"
#include "cpimitem.h"
#include "cpimeventitem.h"
#include "pimbaserepeatrule.h"
#include "pimjnitools.h"
#include "pimutils.h"
#include "logger.h"

// CONSTANTS
const Versit::TVersitCharSet KCharSetMapping[] =
    { Versit::EUTF8CharSet, Versit::EISO88591CharSet };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMSerializer::getInstance
// Two-phased constructor for JNI
// -----------------------------------------------------------------------------
//
pimbaseserializer* pimbaseserializer::getInstance(pimbasemanager* aManager) // used for getting validators
{
    JELOG2(EPim);
    CPIMManager* manager = static_cast<CPIMManager*>(aManager);
    TInt error = KErrNone;
    pimbaseserializer *serializer = NULL;
    TRAP(error, serializer = CPIMSerializer::NewL(manager));
    if (error != KErrNone)
    {
        throw error;
    }
    return serializer;
}

// -----------------------------------------------------------------------------
// CPIMSerializer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMSerializer* CPIMSerializer::NewL(CPIMManager* aManager)
{
    JELOG2(EPim);
    CPIMSerializer* self = new(ELeave) CPIMSerializer;

    CleanupStack::PushL(self);
    self->ConstructL(aManager);
    CleanupStack::Pop();

    return self;
}

// Destructor
CPIMSerializer::~CPIMSerializer()
{
    JELOG2(EPim);
    delete iVersit;
}

// -----------------------------------------------------------------------------
// CPIMSerializer::FromSerialFormatL
// forwards the request to the versit module
// -----------------------------------------------------------------------------
//
jintArray CPIMSerializer::FromSerialFormatL(JNIEnv* aJniEnv,
        const TDesC8& aBuffer, TEncoding aEncoding)
{
    JELOG2(EPim);
    Versit::TVersitCharSet charSet = MapEncodingL(aEncoding);
    RPointerArray<CPIMItem>* itemArray = iVersit->StringToItemL(aBuffer,
                                         charSet);
    CleanupDeletePushL(itemArray);
    CleanupClosePushL(*itemArray);

    // calculate the length of the needed array
    TInt arrayLength = 0;
    TInt itemCount = itemArray->Count();
    TInt i = 0;
    for (i = 0; i < itemCount; i++)
    {
        switch ((*itemArray)[i]->ItemType())
        {
        case EPIMContactList:
        case EPIMToDoList:
        {
            arrayLength += 2;
            break;
        }
        case EPIMEventList:
        {
            arrayLength += 4;
            break;
        }
        default:
        {
            User::Leave(KErrNoMemory);
        }
        }
    }

    jintArray itemHandles = aJniEnv->NewIntArray(arrayLength);
    if (!itemHandles)
    {
        User::Leave(KErrNoMemory);
    }
    jint* handles = aJniEnv->GetIntArrayElements(itemHandles, 0);
    jint* handlePointer = handles;
    for (i = 0; i < itemCount; i++)
    {
        pimbaseitem* item = (*itemArray)[i];
        TInt itemHandle(reinterpret_cast<int>(item));
        TInt itemType = ((*itemArray)[i])->ItemType();
        *handlePointer++ = itemType;
        *handlePointer++ = itemHandle;
        if (itemType == EPIMEventList)
        {

            int baseRepeatRuleHandle = item->getRepeatHandle();
            if (baseRepeatRuleHandle < KErrNone)
                throw baseRepeatRuleHandle;

            *handlePointer++ = baseRepeatRuleHandle;
            handlePointer++;
        }
    }
    aJniEnv->ReleaseIntArrayElements(itemHandles, handles, 0);

    CleanupStack::PopAndDestroy(2);
    return itemHandles;
}

jintArray CPIMSerializer::fromSerialFormat(JNIEnv* aJniEnv, jbyteArray aBytes,
        int aByteLength, int aEncoding, jintArray aError)
{
    JELOG2(EPim);
    jbyte* bytes = aJniEnv->GetByteArrayElements(aBytes, 0);
    TUint8* temp = reinterpret_cast<TUint8*>(bytes);
    const TPtrC8 versitString(temp, aByteLength);
    TInt error = KErrNone;
    jintArray itemArray = NULL;
    TRAP(error, itemArray = FromSerialFormatL(aJniEnv, versitString,
                            static_cast<TEncoding>(aEncoding)));
    aJniEnv->ReleaseByteArrayElements(aBytes, bytes, JNI_ABORT);
    if (error != KErrNone)
    {
        SetJavaErrorCode(aJniEnv, aError, error);
        return NULL;
    }

    if (!itemArray)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }

    SetJavaErrorCode(aJniEnv, aError, KErrNone);

    return itemArray;
}

// -----------------------------------------------------------------------------
// CPIMSerializer::ToSerialFormatL
// forwards the request to the versit module
// -----------------------------------------------------------------------------
//
HBufC8* CPIMSerializer::ToSerialFormatL(const CPIMItem& aItem,
                                        TEncoding aEncoding)
{
    JELOG2(EPim);
    Versit::TVersitCharSet charSet = MapEncodingL(aEncoding);
    return iVersit->ItemToStringL(aItem, charSet);
}

jbyteArray CPIMSerializer::toSerialFormat(pimbaseitem* aItem, int aEncoding,
        JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    HBufC8* versitObject = NULL;
    CPIMItem* item = static_cast<CPIMItem *>(aItem);

    TInt error = 0;
    TRAP(error, versitObject = ToSerialFormatL(*item,
                               static_cast<TEncoding>(aEncoding)));
    if (error != KErrNone)
    {
        SetJavaErrorCode(aJniEnv, aError, error);
        return NULL;
    }

    if (!versitObject)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }

    TInt versitLength = versitObject->Length();
    jbyteArray retVal = aJniEnv->NewByteArray(versitLength);
    if (!retVal)
    {
        // creating a new byte array failed
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        delete versitObject;
        return NULL;
    }
    jbyte* bytes = aJniEnv->GetByteArrayElements(retVal, 0);
    const TUint8* versitPtr = versitObject->Ptr();
    for (int i = 0; i < versitLength; i++)
    {
        bytes[i] = versitPtr[i];
    }
    aJniEnv->ReleaseByteArrayElements(retVal, bytes, 0);
    SetJavaErrorCode(aJniEnv, aError, KErrNone);
    delete versitObject;
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMSerializer::SupportedSerialFormatsL
// forwards the request to the versit module
// -----------------------------------------------------------------------------
//
const CDesCArray& CPIMSerializer::SupportedSerialFormatsL(
    TPIMListType aPimListType)
{
    JELOG2(EPim);
    return iVersit->SupportedSerialFormatsL(aPimListType);
}

jobjectArray CPIMSerializer::supportedSerialFormats(TPIMListType aPimListType,
        JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    const CDesCArray* desArray = NULL;
    TInt error = KErrNone;
    TRAP(error, desArray = &(SupportedSerialFormatsL(aPimListType)));
    if (error != KErrNone)
    {
        SetJavaErrorCode(aJniEnv, aError, error);
        return NULL;
    }

    if (!desArray)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }

    TInt desCount = desArray->Count();
    jobjectArray retVal = aJniEnv->NewObjectArray(desCount, aJniEnv->FindClass(
                              "java/lang/String"), NULL);
    if (retVal == NULL)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }

    for (TInt i = 0; i < desCount; i++)
    {
        TPtrC epocString = (*desArray)[i];
        jstring javaString = aJniEnv->NewString(epocString.Ptr(),
                                                epocString.Length());
        if (javaString == NULL)
        {
            SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
            return NULL;
        }
        aJniEnv->SetObjectArrayElement(retVal, i, javaString);

        // Avoid exceeding max local references (5)
        aJniEnv->DeleteLocalRef(javaString);
    }
    // note that desArray is owned by the adapter, and not deleted here
    SetJavaErrorCode(aJniEnv, aError, KErrNone);
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMSerializer::CPIMSerializer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMSerializer::CPIMSerializer()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMSerializer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMSerializer::ConstructL(CPIMManager* aManager)
{
    JELOG2(EPim);
    iVersit = CPIMVersit::NewL(aManager);
}

// -----------------------------------------------------------------------------
// CPIMSerializer::MapEncodingL
// Maps the encoding from TEncoding to Versit::TVersitCharSet
// -----------------------------------------------------------------------------
//
Versit::TVersitCharSet CPIMSerializer::MapEncodingL(TEncoding aEncoding)
{
    JELOG2(EPim);
    if ((aEncoding < 0) || (aEncoding > EISO88591))
    {
        User::Leave(KErrArgument);
    }
    return KCharSetMapping[aEncoding];
}

