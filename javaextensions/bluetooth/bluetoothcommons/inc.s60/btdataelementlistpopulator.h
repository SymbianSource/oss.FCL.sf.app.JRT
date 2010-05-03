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


#ifndef BTDATAELEMENTLISTPOPULATOR_H
#define BTDATAELEMENTLISTPOPULATOR_H

#include <string>
#include <btsdp.h>
#include <bttypes.h>
#include <e32base.h>

#include "logger.h"
#include "javajniutils.h"

namespace java
{
namespace bluetooth
{

class BTDataElementListPopulator: public MSdpElementBuilder
{
public:
    static BTDataElementListPopulator* New(JNIEnv* aJni,
                                           MSdpElementBuilder* aParentBuilder, jobject aParentDataElement,
                                           bool aCanUnregister);

    ~BTDataElementListPopulator();

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
    // For clearing the objects
    MSdpElementBuilder* UnregisterDataElementListPopulator();

private:
    BTDataElementListPopulator();
    void Construct(JNIEnv* aJni, MSdpElementBuilder* aParentBuilder,
                   jobject aParentDataElement, bool aCanUnregister);
    char NibbleToHexChar(TUint8 aByte);
    jobject TUUIDtoJUUID(const TUUID& aUUID);

private:
    JNIEnv* mJni;
    jobject mParentDataElement;
    jclass mDataElementClass;
    jmethodID mAddElementMethod;
    bool mCanUnregister;
    MSdpElementBuilder* mParentBuilder;
    MSdpElementBuilder* mDataElementListPopulator;
};

} //end namespace bluetooth
} //end namespace java

#endif // BTDATAELEMENTLISTPOPULATOR_H
