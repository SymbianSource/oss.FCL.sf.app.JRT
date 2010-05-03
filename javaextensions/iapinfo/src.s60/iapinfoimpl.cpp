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


#include <cmmanager.h>
#include <cmdestination.h>
#include "jni.h"
#include "stringbuffer.h"
#include "com_nokia_mid_iapinfo_IAPInfoImpl.h"
#include "logger.h"

using namespace CommsDat;
using namespace CMManager;
using namespace java::util;

_LIT(KSelector, ";");
_LIT(KIDSelector, ",");

jstring CreateJavaString(JNIEnv* aJNI, const HBufC* aString);
jobjectArray GetDestinationNetworksL(JNIEnv * aJNI);
jstring GetDestinationNetworkByIdL(JNIEnv* aJNI, jint aID);
jstring GetDestinationNetworkByNameL(JNIEnv *aJNI, jstring aJName);
void throwException(JNIEnv* aJNI, TInt aErrorCode);

///////////////////////////////////////////////////////////////////////////////
//  Method:    _getDestinationNetworks
//  Signature: ()I
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT jobjectArray JNICALL Java_com_nokia_mid_iapinfo_IAPInfoImpl__1getDestinationNetworks
(JNIEnv * aJNI, jclass)
{
    jobjectArray Object = NULL;
    TInt err = KErrNone;
    TRAP(err, Object = GetDestinationNetworksL(aJNI));
    if (err != KErrNone)
    {
        throwException(aJNI, err);
        return NULL;
    }
    return Object;
}

///////////////////////////////////////////////////////////////////////////////
//GetDestinationNetworksL
// @param JNIEnv
///////////////////////////////////////////////////////////////////////////////
jobjectArray GetDestinationNetworksL(JNIEnv * aJNI)
{

    RCmManager localCmManagerExt;
    LOG(EJavaIapInfo, EInfo,"using RCMManager");
    localCmManagerExt.OpenL();
    RArray<TUint32> destIdArray;
    localCmManagerExt.AllDestinationsL(destIdArray);

    //allocate the String array
    TInt array_size = destIdArray.Count();
    jobjectArray jarray = aJNI->NewObjectArray(array_size, aJNI->FindClass(
                              "java/lang/String"), aJNI->NewStringUTF(""));
    for (TInt i = 0; i < destIdArray.Count(); i++)
    {
        StringBuffer* Buffer;
        Buffer = StringBuffer::NewL();
        CleanupStack::PushL(Buffer);
        RCmDestination dest = localCmManagerExt.DestinationL(destIdArray[i]);
        CleanupClosePushL(dest);
        TUint32 ID = dest.Id();
        Buffer->Append(ID);
        Buffer->Append(KSelector);
        HBufC* destName = dest.NameLC();
        Buffer->Append(destName);
        Buffer->Append(KSelector);
        for (TInt j = 0; j < dest.ConnectionMethodCount(); j++)
        {
            TUint32 iapId =
                dest.ConnectionMethodL(j).GetIntAttributeL(ECmIapId);
            Buffer->Append(iapId);
            Buffer->Append(KIDSelector);
        }
        aJNI->SetObjectArrayElement(jarray, i, CreateJavaString(aJNI,
                                    Buffer->GetString()));
        CleanupStack::PopAndDestroy(2);//destName, dest
        CleanupStack::PopAndDestroy(1); // destroy the Buffer object
    }//end for
    localCmManagerExt.Close();

    return jarray;
}

///////////////////////////////////////////////////////////////////////////////
// Class:     com_nokia_mid_iapinfo_IAPInfoImpl
// Method:    _getDestinationNetworkById
// Signature: ()I
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT jstring JNICALL Java_com_nokia_mid_iapinfo_IAPInfoImpl__1getDestinationNetworkById
(JNIEnv* aJNI, jclass, jint aID)
{
    jstring Object = NULL;
    TInt err = KErrNone;
    TRAP(err, Object = GetDestinationNetworkByIdL(aJNI, aID));
    if (err != KErrNone)
    {
        throwException(aJNI, err);
        return NULL;
    }
    return Object;
}

///////////////////////////////////////////////////////////////////////////////
// GetDestinationNetworkByIdL
//
// @param JNIEnv
// @param jint aID
//
///////////////////////////////////////////////////////////////////////////////
jstring GetDestinationNetworkByIdL(JNIEnv* aJNI, jint aID)
{
    RCmManager localCmManagerExt;
    LOG(EJavaIapInfo, EInfo,"using RCMManager");
    localCmManagerExt.OpenL();
    //read all destinations
    RArray<TUint32> destIdArray;
    localCmManagerExt.AllDestinationsL(destIdArray);
    StringBuffer* Buffer = StringBuffer::NewL();
    CleanupStack::PushL(Buffer);
    for (TInt i = 0; i < destIdArray.Count(); i++)
    {
        RCmDestination DestinationNetwork = localCmManagerExt.DestinationL(
                                                destIdArray[i]);
        CleanupClosePushL(DestinationNetwork);
        //check the id.
        TUint32 ID = DestinationNetwork.Id();
        if (ID == aID)
        {
            Buffer->Append(ID);
            Buffer->Append(KSelector);
            //Destination network is found, add value to stringbuffer
            HBufC* Name = DestinationNetwork.NameLC();
            Buffer->Append(Name);
            Buffer->Append(KSelector);
            //get the ids of the Access points
            for (TInt j = 0; j < DestinationNetwork.ConnectionMethodCount(); j++)
            {
                TUint32
                iapId =
                    DestinationNetwork.ConnectionMethodL(j).GetIntAttributeL(
                        ECmIapId);
                Buffer->Append(iapId);
                Buffer->Append(KIDSelector);
            }
            CleanupStack::PopAndDestroy(1);//Name
            i = destIdArray.Count(); //to exit
        }
        CleanupStack::PopAndDestroy(1); // DestinationNetwork
    }

    jstring ReturnString = CreateJavaString(aJNI, Buffer->GetString());
    CleanupStack::PopAndDestroy(1);//Buffer
    localCmManagerExt.Close();

    return ReturnString;
}
/*
 * Class:     com_nokia_mid_iapinfo_IAPInfoImpl
 * Method:    _getDestinationNetworkByName
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mid_iapinfo_IAPInfoImpl__1getDestinationNetworkByName
(JNIEnv *aJNI, jclass, jstring aJName)
{
    jstring Object = NULL;
    TInt err = KErrNone;
    TRAP(err, Object = GetDestinationNetworkByNameL(aJNI, aJName));
    if (err != KErrNone)
    {
        throwException(aJNI, err);
        return NULL;
    }
    return Object;
}

///////////////////////////////////////////////////////////////////////////////
// GetDestinationNetworkByNameL
//
// @param JNIEnv
// @param aJName
//
///////////////////////////////////////////////////////////////////////////////
jstring GetDestinationNetworkByNameL(JNIEnv *aJNI, jstring aJName)
{
    RCmManager localCmManagerExt;
    LOG(EJavaIapInfo, EInfo,"using RCMManager");
    localCmManagerExt.OpenL();
    //read all destinations
    RArray<TUint32> destIdArray;
    localCmManagerExt.AllDestinationsL(destIdArray);
    StringBuffer* Buffer = StringBuffer::NewL();
    CleanupStack::PushL(Buffer);
    HBufC* SearchString = Buffer->CreateHBufCFromJavaStringLC(aJNI, aJName);
    for (TInt i = 0; i < destIdArray.Count(); i++)
    {
        RCmDestination DestinationNetwork = localCmManagerExt.DestinationL(
                                                destIdArray[i]);
        CleanupClosePushL(DestinationNetwork);
        //check the id.
        TUint32 ID = DestinationNetwork.Id();
        HBufC* DNName = DestinationNetwork.NameLC();
        if (DNName->Des().Compare(SearchString->Des()) == 0)
        {
            Buffer->Append(ID);
            Buffer->Append(KSelector);
            //Destination network is found, add value to stringbuffer
            Buffer->Append(DNName);
            Buffer->Append(KSelector);
            //get the ids of the Access points
            for (TInt j = 0; j < DestinationNetwork.ConnectionMethodCount(); j++)
            {
                TUint32
                iapId =
                    DestinationNetwork.ConnectionMethodL(j).GetIntAttributeL(
                        ECmIapId);
                Buffer->Append(iapId);
                Buffer->Append(KIDSelector);
            }//end for
            i = destIdArray.Count();//to terminate for
        }//end if
        CleanupStack::PopAndDestroy(1);// DNName
        CleanupStack::PopAndDestroy(1); // DestinationNetwork
    }//end for
    jstring ReturnString = CreateJavaString(aJNI, Buffer->GetString());
    CleanupStack::PopAndDestroy(1);// SearchString
    CleanupStack::PopAndDestroy(1);// Buffer
    localCmManagerExt.Close();

    return ReturnString;
}

///////////////////////////////////////////////////////////////////////////////
// CreateJavaString Creates a java string from HBufC object
//
// @param JNIEnv
// @param HBufC
///////////////////////////////////////////////////////////////////////////////
jstring CreateJavaString(JNIEnv* aJNI, const HBufC* aString)
{
    jstring str = aJNI->NewString(aString->Ptr(), aString->Length());
    return str;
}

///////////////////////////////////////////////////////////////////////////////
// CreateJavaString Method to throw a java exception
// @param JNIEnv* aJNI
// @param TInt aErrorCode The error code
///////////////////////////////////////////////////////////////////////////////
void throwException(JNIEnv* aJNI, TInt aErrorCode)
{
    jclass clsException = aJNI->FindClass(
                              "com/nokia/mid/iapinfo/IAPInfoException");
    TBuf8<40> buf;
    buf.Append(_L("Native error: "));
    buf.AppendNum(aErrorCode);
    const char *temp = (const char *) buf.PtrZ();
    aJNI->ThrowNew(clsException, temp);
}
