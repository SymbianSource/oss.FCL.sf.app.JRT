/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MIDPROXYMAP_H
#define MIDPROXYMAP_H

#include <lcdui.h>

NONSHARABLE_CLASS(CMIDProxyMap) : public CBase
{
public:
    CMIDProxyMap();
    ~CMIDProxyMap();

    template<class T>
    inline TBool Lookup(T*& aPointer, TUint32 aHandle);

protected:
    TAny* Lookup(MMIDComponent::TType aType, TUint32 aHandle);

private:
};

template<class T>
inline TBool CMIDProxyMap::Lookup(T*& aPointer, TUint32 aHandle)
{
    TAny* pointer = Lookup(T::EType, aHandle);
    if (pointer)
    {
        aPointer = (T*)pointer;
        return ETrue;
    }
    return EFalse;
}

#endif // MIDPROXYMAP_H
