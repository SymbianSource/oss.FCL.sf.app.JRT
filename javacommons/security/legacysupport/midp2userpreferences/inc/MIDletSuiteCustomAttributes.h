/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <MMIDletSuiteCustomAttributes.h>

namespace MIDP
{

class CMIDletSuiteCustomAttributes: public CBase, public MIDP::MMIDletSuiteCustomAttributes
{
public:

    static CMIDletSuiteCustomAttributes* NewL(TUint32);

public:

    virtual ~CMIDletSuiteCustomAttributes();

    virtual HBufC* GetL(const TDesC&, TUint32&);

    virtual void AddL(const TDesC&, const TDesC&, const TUint32);

    virtual void UpdateL(const TDesC&, const TDesC&, const TUint32);

    virtual void RemoveAllL();

private:

    CMIDletSuiteCustomAttributes(TUint32);
    void ConstructL();
    HBufC* mapOnScreenKeyboardToLegacyValueL(const std::wstring& aOmjValue);
    void mapOnScreenKeyboardToOmjValue(const TDesC& aLegacyValue, std::wstring& aOmjValue);

private:

    TUint32       iSuiteId;
};

} // end of namespace MIDP
