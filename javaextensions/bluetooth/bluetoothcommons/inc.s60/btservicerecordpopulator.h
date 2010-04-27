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


#ifndef BTSERVICERECORDPOPULATOR_H
#define BTSERVICERECORDPOPULATOR_H

#include <string>
#include <btsdp.h>
#include <bttypes.h>
#include <e32base.h>

#include "logger.h"
#include "javajniutils.h"
#include "btdataelementlistpopulator.h"

namespace java
{
namespace bluetooth
{

class BTServiceRecordPopulator: public MSdpElementBuilder
{
public:
    OS_IMPORT
    static BTServiceRecordPopulator* New(JNIEnv* aJni,
                                         jobject aServiceRecordImpl);

    OS_IMPORT ~BTServiceRecordPopulator();

    OS_IMPORT bool isAttributesFound();

    // From MSdpElementBuilder
    MSdpElementBuilder* BuildDEAL();
    MSdpElementBuilder* BuildDESL();
    MSdpElementBuilder* StartListL();
    MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
    MSdpElementBuilder* EndListL();
    MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
    MSdpElementBuilder* BuildNilL();
    MSdpElementBuilder* BuildBooleanL(TBool aBool);
    MSdpElementBuilder* BuildStringL(const TDesC8& aString);
    MSdpElementBuilder* BuildURLL(const TDesC8& aURL);
    MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
    MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc,
                                      const TDesC8& aData);

    // Calls specific to restore & synchronize the Java service record
    MSdpElementBuilder* SetDeviceServiceClass(TUint32 aDeviceClass);
    MSdpElementBuilder* SetServiceRecordHandle(TUint32 aServRecHandle);

private:
    BTServiceRecordPopulator();
    void Construct(JNIEnv* aJni, jobject aServiceRecordImpl);
    char NibbleToHexChar(TUint8 aByte);
    jobject TUUIDtoJUUID(const TUUID& aUUID);

private:
    enum TPopulationState
    {
        ENotPopulating = 0,
        EReadyForStartList,
        EReadyForAttributeId,
        EReadyForAttributeValue
    };
    TPopulationState mCurrentState;
    bool mAttributeFound;
    JNIEnv* mJni;
    jobject mServiceRecordImpl;
    jclass mServiceRecordImplClass;
    jmethodID mPopulateAttributeValueMethod;
    jclass mDataElementClass;
    jobject mCurrentDataElement;
    TSdpAttributeID mCurrentAttributeId;

    // Used to populatore DES/DEA list
    BTDataElementListPopulator* mDataElementListPopulator;
};

} //end namespace bluetooth
} //end namespace java

#endif // BTSERVICERECORDPOPULATOR_H
