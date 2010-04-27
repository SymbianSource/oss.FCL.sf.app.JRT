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


#include "btdataelementlistpopulator.h"
#include "bluetoothconsts.h"

namespace java
{
namespace bluetooth
{

BTDataElementListPopulator* BTDataElementListPopulator::New(JNIEnv* aJni,
        MSdpElementBuilder* aParentBuilder, jobject aParentDataElement,
        bool aCanUnregister)
{
    JELOG2(EJavaBluetooth);
    BTDataElementListPopulator* populator = new BTDataElementListPopulator();
    populator->Construct(aJni, aParentBuilder, aParentDataElement,
                         aCanUnregister);
    return populator;
}

BTDataElementListPopulator::BTDataElementListPopulator()
{
}

void BTDataElementListPopulator::Construct(JNIEnv* aJni,
        MSdpElementBuilder* aParentBuilder, jobject aParentDataElement,
        bool aCanUnregister)
{
    JELOG2(EJavaBluetooth);
    mJni = aJni;
    mParentDataElement = aParentDataElement;
    mParentBuilder = aParentBuilder;
    mDataElementListPopulator = NULL;
    mCanUnregister = aCanUnregister;
    mDataElementClass = mJni->GetObjectClass(aParentDataElement);
    mAddElementMethod = mJni->GetMethodID(mDataElementClass, "addElement",
                                          "(Ljavax/bluetooth/DataElement;)V");
}

BTDataElementListPopulator::~BTDataElementListPopulator()
{
    JELOG2(EJavaBluetooth);
    if (mDataElementListPopulator)
    {
        delete mDataElementListPopulator;
        mDataElementListPopulator = NULL;
    }
    mParentDataElement = NULL;
    mParentBuilder = NULL;
    mCanUnregister = false;
    mDataElementClass = NULL;
    mAddElementMethod = NULL;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildDEAL()
{
    JELOG2(EJavaBluetooth);
    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(I)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_DATALT);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    mDataElementListPopulator = BTDataElementListPopulator::New(mJni, this,
                                dataElement, true);

    // Further 'Build' commands should add elements to the new list
    return mDataElementListPopulator;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildDESL()
{
    JELOG2(EJavaBluetooth);
    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(I)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_DATSEQ);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    mDataElementListPopulator = BTDataElementListPopulator::New(mJni, this,
                                dataElement, true);

    // Further 'Build' commands should add elements to the new list
    return mDataElementListPopulator;
}

MSdpElementBuilder* BTDataElementListPopulator::StartListL()
{
    JELOG2(EJavaBluetooth);
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::EndListL()
{
    JELOG2(EJavaBluetooth);
    MSdpElementBuilder* parentBuilder = mParentBuilder;
    if (mCanUnregister == true)
    {
        // End of the current object. Execution continues with the parent builder
        return ((BTDataElementListPopulator*)
                mParentBuilder)->UnregisterDataElementListPopulator();
    }
    return parentBuilder;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildUintL(const TDesC8& aUint)
{
    JELOG2(EJavaBluetooth);
    CSdpAttrValue* value = CSdpAttrValueUint::NewUintL(aUint);
    TUint dataSize = value->DataSize();
    jobject dataElement;

    if (dataSize <= 4)
    {
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(IJ)V");
        jlong dataValue = (jlong) value->Uint();

        if (dataSize == 1)
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_U_INT_1, dataValue);
        }
        else if (dataSize == 2)
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_U_INT_2, dataValue);
        }
        else // if (dataSize == 4)
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_U_INT_4, dataValue);
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
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_U_INT_8, byteArray);
        }
        else
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_U_INT_16, byteArray);
        }
    }

    delete value;
    value = NULL;

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    LOG(EJavaBluetooth, EInfo, "- BTDataElementListPopulator::BuildUintL");
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildIntL(const TDesC8& aInt)
{
    JELOG2(EJavaBluetooth);
    CSdpAttrValue* value = CSdpAttrValueInt::NewIntL(aInt);
    TUint dataSize = value->DataSize();
    jobject dataElement;

    if (dataSize < 8)
    {
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(IJ)V");
        jlong dataValue = (jlong) value->Int();

        if (dataSize == 1)
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_INT_1, dataValue);
        }
        else if (dataSize == 2)
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_INT_2, dataValue);
        }
        else //if (dataSize == 4)
        {
            dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_INT_4, dataValue);
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
        dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                      DataElement_INT_8, dataValue);
    }
    else
    {
        jmethodID constructorID = mJni->GetMethodID(mDataElementClass,
                                  "<init>", "(ILjava/lang/Object;)V");

        jbyte* bytes = (jbyte*) aInt.Ptr();
        jbyteArray byteArray = mJni->NewByteArray(dataSize);
        mJni->SetByteArrayRegion(byteArray, 0, dataSize, bytes);

        dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                      DataElement_INT_16, byteArray);
    }

    delete value;
    value = NULL;

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    LOG(EJavaBluetooth, EInfo, "- BTDataElementListPopulator::BuildIntL");
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildNilL()
{
    JELOG2(EJavaBluetooth);
    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(I)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_NULL);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildBooleanL(TBool aBool)
{
    JELOG2(EJavaBluetooth);
    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(Z)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          aBool);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    LOG1(EJavaBluetooth, EInfo,
         "- BTDataElementListPopulator::BuildBooleanL %d", (int) aBool);
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildStringL(
    const TDesC8& aString)
{
    JELOG2(EJavaBluetooth);
    HBufC8* bufString = HBufC8::New(aString.Length()+1);
    TPtr8 ptr = bufString->Des();

    ptr.Copy(aString);
    jstring dataValue = mJni->NewStringUTF((char *) ptr.PtrZ());
    delete bufString;

    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(ILjava/lang/Object;)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_STRING, dataValue);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    LOG1(EJavaBluetooth, EInfo,
         "- BTDataElementListPopulator::BuildStringL %S",
         (char *) aString.Ptr());
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildURLL(const TDesC8& aURL)
{
    JELOG2(EJavaBluetooth);
    HBufC8* bufString = HBufC8::New(aURL.Length()+1);
    TPtr8 ptr = bufString->Des();

    ptr.Copy(aURL);
    jstring dataValue = mJni->NewStringUTF((char *) ptr.PtrZ());
    delete bufString;
    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(ILjava/lang/Object;)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_URL, dataValue);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    LOG1(EJavaBluetooth, EInfo, "- BTDataElementListPopulator::BuildURLL %S",
         (char *) aURL.Ptr());
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildUUIDL(const TUUID& aUUID)
{
    JELOG2(EJavaBluetooth);
    jobject dataValue = TUUIDtoJUUID(aUUID);
    jmethodID constructorID = mJni->GetMethodID(mDataElementClass, "<init>",
                              "(ILjava/lang/Object;)V");
    jobject dataElement = mJni->NewObject(mDataElementClass, constructorID,
                                          DataElement_UUID, dataValue);

    (*mJni).CallVoidMethod(mParentDataElement, mAddElementMethod, dataElement);
    return this;
}

MSdpElementBuilder* BTDataElementListPopulator::BuildUnknownL(TUint8 /*aType*/,
        TUint8 /*aSizeDesc*/, const TDesC8& /*aData*/)
{
    JELOG2(EJavaBluetooth);
    // Ignore if any unknown type of data exist !!
    return this;
}

MSdpElementBuilder*
BTDataElementListPopulator::UnregisterDataElementListPopulator()
{
    JELOG2(EJavaBluetooth);
    if (mDataElementListPopulator)
    {
        delete mDataElementListPopulator;
        mDataElementListPopulator = NULL;
    }
    return this;
}

char BTDataElementListPopulator::NibbleToHexChar(TUint8 aByte)
{
    JELOG2(EJavaBluetooth);
    char hexChars[] = "0123456789abcdef";
    return hexChars[aByte];
}

jobject BTDataElementListPopulator::TUUIDtoJUUID(const TUUID& aUUID)
{
    JELOG2(EJavaBluetooth);
    TPtrC8 ptrUUID = aUUID.Des();
    int uuidLen = (int) ptrUUID.Length();
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

    LOG1(EJavaBluetooth, EInfo,
         "- BTDataElementListPopulator::TUUIDtoJUUID %s", strUUIDBuff);
    delete[] strUUIDBuff;
    strUUIDBuff = NULL;
    return mJni->NewObject(uuidClass, constructorID, uuidString, isShortUUID);
}

} //end namespace bluetooth
} //end namespace java
