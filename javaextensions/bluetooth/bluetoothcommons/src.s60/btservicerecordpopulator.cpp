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
* Description:
*
*/


#include "btservicerecordpopulator.h"

#include "bluetoothconsts.h"

namespace java
{
namespace bluetooth
{

OS_EXPORT BTServiceRecordPopulator* BTServiceRecordPopulator::New(JNIEnv* aJni,
        jobject aServiceRecordImpl)
{
    JELOG2(EJavaBluetooth);
    BTServiceRecordPopulator* populator = new BTServiceRecordPopulator();
    populator->Construct(aJni, aServiceRecordImpl);
    return populator;
}

BTServiceRecordPopulator::BTServiceRecordPopulator()
{
}

void BTServiceRecordPopulator::Construct(JNIEnv* aJni,
        jobject aServiceRecordImpl)
{
    JELOG2(EJavaBluetooth);
    mCurrentState = ENotPopulating;
    mAttributeFound = false;
    mCurrentAttributeId = 0;
    mDataElementListPopulator = NULL;
    mJni = aJni;
    mServiceRecordImpl = aServiceRecordImpl;
    mServiceRecordImplClass = mJni->GetObjectClass(mServiceRecordImpl);
    mPopulateAttributeValueMethod = mJni->GetMethodID(mServiceRecordImplClass,
                                    "populateAttributeValue", "(ILjavax/bluetooth/DataElement;)V");
    mDataElementClass = mJni->FindClass("javax/bluetooth/DataElement");
}

OS_EXPORT BTServiceRecordPopulator::~BTServiceRecordPopulator()
{
    JELOG2(EJavaBluetooth);
    if (mDataElementListPopulator)
    {
        delete mDataElementListPopulator;
        mDataElementListPopulator = NULL;
    }
    mCurrentState = ENotPopulating;
    mCurrentAttributeId = 0;
    mJni = NULL;
    mServiceRecordImpl = NULL;
    mServiceRecordImplClass = NULL;
    mPopulateAttributeValueMethod = NULL;
    mDataElementClass = NULL;
}

OS_EXPORT bool BTServiceRecordPopulator::isAttributesFound()
{
    return mAttributeFound;
}

MSdpElementBuilder* BTServiceRecordPopulator::SetServiceRecordHandle(
    TUint32 aServRecHandle)
{
    JELOG2(EJavaBluetooth);

    jmethodID populateServiceRecordHandleMethod = mJni->GetMethodID(
                mServiceRecordImplClass, "populateServiceRecordHandle", "(I)V");

    mJni->CallVoidMethod(mServiceRecordImpl, populateServiceRecordHandleMethod,
                         aServRecHandle);

    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::SetDeviceServiceClass(
    TUint32 aDeviceClass)
{
    JELOG2(EJavaBluetooth);

    jmethodID setDeviceServiceClassMethod = mJni->GetMethodID(
                                                mServiceRecordImplClass, "setDeviceServiceClasses", "(I)V");

    mJni->CallVoidMethod(mServiceRecordImpl, setDeviceServiceClassMethod,
                         aDeviceClass);

    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildDESL()
{
    JELOG2(EJavaBluetooth);
    if (ENotPopulating == mCurrentState)
    {
        // Expected BuildDESL must call at the beginning
        mCurrentState = EReadyForStartList;
    }
    else if (EReadyForAttributeValue == mCurrentState)
    {
        LOG(EJavaBluetooth, EInfo,
            "  BTServiceRecordPopulator::BuildDESL Constructing List");
        mCurrentState = EReadyForAttributeId;
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(I)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              DataElement_DATSEQ);
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);

        mDataElementListPopulator = BTDataElementListPopulator::New(mJni, this,
                                    mCurrentDataElement, false);
        return mDataElementListPopulator;
    }

    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildDEAL()
{
    JELOG2(EJavaBluetooth);
    if (EReadyForAttributeValue == mCurrentState)
    {
        LOG(EJavaBluetooth, EInfo,
            "  BTServiceRecordPopulator::BuildDEAL Constructing List");
        mCurrentState = EReadyForAttributeId;
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(I)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              DataElement_DATALT);
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);

        mDataElementListPopulator = BTDataElementListPopulator::New(mJni, this,
                                    mCurrentDataElement, false);
        return mDataElementListPopulator;
    }

    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::StartListL()
{
    JELOG2(EJavaBluetooth);
    if (EReadyForStartList == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;
    }
    else
    {
        LOG(EJavaBluetooth, EInfo,
            "  BTServiceRecordPopulator::StartListL Called without DESL");
    }
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildUintL(const TDesC8& aUint)
{
    JELOG2(EJavaBluetooth);
    CSdpAttrValue* value = CSdpAttrValueUint::NewUintL(aUint);
    TUint dataSize = value->DataSize();

    if (EReadyForAttributeId == mCurrentState)
    {
        LOG1(EJavaBluetooth, EInfo,
             "  BTServiceRecordPopulator::BuildUintL Got Attribute ID %X",
             value->Uint());
        mCurrentState = EReadyForAttributeValue;
        mAttributeFound = true;
        if (mDataElementListPopulator)
        {
            delete mDataElementListPopulator;
            mDataElementListPopulator = NULL;
        }

        LOG1(
            EJavaBluetooth,
            EInfo,
            "  BTServiceRecordPopulator::BuildUintL Setting attribute ID %X",
            value->Uint());
        mCurrentAttributeId = (TSdpAttributeID) value->Uint();
    }
    else
    {
        mCurrentState = EReadyForAttributeId;

        if (dataSize <= 4)
        {
            jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                      "<init>", "(IJ)V");
            jlong dataValue = (jlong) value->Uint();

            if (dataSize == 1)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_U_INT_1, dataValue);
            }
            else if (dataSize == 2)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_U_INT_2, dataValue);
            }
            else //if (dataSize == 4)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_U_INT_4, dataValue);
            }
        }
        else
        {
            jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                      "<init>", "(ILjava/lang/Object;)V");

            jbyte* bytes = (jbyte*) aUint.Ptr();
            jbyteArray byteArray = mJni->NewByteArray(dataSize);
            mJni->SetByteArrayRegion(byteArray, 0, dataSize, bytes);

            if (dataSize == 8)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_U_INT_8, byteArray);
            }
            else
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_U_INT_16, byteArray);
            }
        }

        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    delete value;
    value = NULL;

    LOG(EJavaBluetooth, EInfo, "- BTServiceRecordPopulator::BuildUintL");
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::EndListL()
{
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildIntL(const TDesC8& aInt)
{
    JELOG2(EJavaBluetooth);

    if (EReadyForAttributeValue == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;

        CSdpAttrValue* value = CSdpAttrValueInt::NewIntL(aInt);
        TUint dataSize = value->DataSize();

        if (dataSize < 8)
        {
            jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                      "<init>", "(IJ)V");
            jlong dataValue = (jlong) value->Int();

            if (dataSize == 1)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_INT_1, dataValue);
            }
            else if (dataSize == 2)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_INT_2, dataValue);
            }
            else // (dataSize == 4)
            {
                mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                      constructorID, DataElement_INT_4, dataValue);
            }
        }
        else if (dataSize == 8)
        {
            jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                      "<init>", "(IJ)V");
            const TUint8* ptr = value->Des().Ptr();
            TUint highBytes = BigEndian::Get32(ptr);
            TUint lowBytes = BigEndian::Get32(ptr + sizeof(TUint));
            TInt64 dataValue = MAKE_TINT64(highBytes, lowBytes);
            mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                  constructorID, DataElement_INT_8, (jlong)dataValue);
        }
        else
        {
            jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                      "<init>", "(ILjava/lang/Object;)V");

            jbyte* bytes = (jbyte*) aInt.Ptr();
            jbyteArray byteArray = mJni->NewByteArray(dataSize);
            mJni->SetByteArrayRegion(byteArray, 0, dataSize, bytes);

            mCurrentDataElement = mJni->NewObject(mDataElementClass,
                                                  constructorID, DataElement_INT_16, byteArray);
        }
        delete value;
        value = NULL;
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    LOG(EJavaBluetooth, EInfo, "- BTServiceRecordPopulator::BuildIntL");
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildNilL()
{
    JELOG2(EJavaBluetooth);
    if (EReadyForAttributeValue == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;

        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(I)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              DataElement_NULL);

        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildBooleanL(TBool aBool)
{
    JELOG2(EJavaBluetooth);
    if (EReadyForAttributeValue == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;

        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(Z)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              aBool);
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    LOG1(EJavaBluetooth, EInfo, "- BTServiceRecordPopulator::BuildBooleanL %d",
         (int) aBool);
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildStringL(
    const TDesC8& aString)
{
    JELOG2(EJavaBluetooth);
    if (EReadyForAttributeValue == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;

        HBufC8* bufString = HBufC8::New(aString.Length()+1);
        TPtr8 ptr = bufString->Des();

        ptr.Copy(aString);
        jstring dataValue = mJni->NewStringUTF((char *) ptr.PtrZ());
        delete bufString;
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(ILjava/lang/Object;)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              DataElement_STRING, dataValue);
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    LOG1(EJavaBluetooth, EInfo, "- BTServiceRecordPopulator::BuildStringL %S",
         (char *) aString.Ptr());
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildURLL(const TDesC8& aURL)
{
    JELOG2(EJavaBluetooth);
    if (EReadyForAttributeValue == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;
        HBufC8* bufString = HBufC8::New(aURL.Length()+1);
        TPtr8 ptr = bufString->Des();

        ptr.Copy(aURL);
        jstring dataValue = mJni->NewStringUTF((char *) ptr.PtrZ());
        delete bufString;
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(ILjava/lang/Object;)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              DataElement_URL, dataValue);
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    LOG1(EJavaBluetooth, EInfo, "- BTServiceRecordPopulator::BuildURLL %S",
         (char *) aURL.Ptr());
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildUUIDL(const TUUID& aUUID)
{
    JELOG2(EJavaBluetooth);
    if (EReadyForAttributeValue == mCurrentState)
    {
        mCurrentState = EReadyForAttributeId;

        jobject dataValue = TUUIDtoJUUID(aUUID);
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(ILjava/lang/Object;)V");
        mCurrentDataElement = mJni->NewObject(mDataElementClass, constructorID,
                                              DataElement_UUID, dataValue);
        mJni->CallVoidMethod(mServiceRecordImpl, mPopulateAttributeValueMethod,
                             mCurrentAttributeId, mCurrentDataElement);
    }
    return this;
}

MSdpElementBuilder* BTServiceRecordPopulator::BuildUnknownL(TUint8 /*aType*/,
        TUint8 /*aSizeDesc*/, const TDesC8& /*aData*/)
{
    // Ignore if any unknown type of data exist !!
    JELOG2(EJavaBluetooth);
    return this;
}

char BTServiceRecordPopulator::NibbleToHexChar(TUint8 aByte)
{
    char hexChars[] = "0123456789abcdef";
    return hexChars[aByte];
}

jobject BTServiceRecordPopulator::TUUIDtoJUUID(const TUUID& aUUID)
{
    JELOG2(EJavaBluetooth);
    TPtrC8 ptrUUID = aUUID.Des();
    TInt uuidLen = ptrUUID.Length();
    char *strUUIDBuff = new char[(uuidLen * 2) + 1];
    int strIndex = 0;
    int ptrIndex = 0;

    for (ptrIndex = 0; ptrIndex < uuidLen; ptrIndex++)
    {
        strUUIDBuff[strIndex++] = NibbleToHexChar((ptrUUID[ptrIndex] >> 4)
                                  & 0xf);
        strUUIDBuff[strIndex++] = NibbleToHexChar(ptrUUID[ptrIndex] & 0xf);
    }
    strUUIDBuff[strIndex] = 0;

    jstring uuidString = mJni->NewStringUTF(strUUIDBuff);
    jclass uuidClass = mJni->FindClass("javax/bluetooth/UUID");
    jmethodID constructorID = mJni->GetMethodID(uuidClass, "<init>",
                              "(Ljava/lang/String;Z)V");
    bool isShortUUID = (uuidLen > 4) ? false : true;

    LOG1(EJavaBluetooth, EInfo, "- BTServiceRecordPopulator::TUUIDtoJUUID %s",
         strUUIDBuff);
    delete[] strUUIDBuff;
    strUUIDBuff = NULL;
    return mJni->NewObject(uuidClass, constructorID, uuidString, isShortUUID);
}

} //end namespace bluetooth
} //end namespace java
