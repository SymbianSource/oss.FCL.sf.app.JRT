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

#ifndef PUSHREGDATA_H
#define PUSHREGDATA_H

#include <string>

namespace java
{
namespace push
{

class PushRegData
{
public:

    explicit PushRegData(std::wstring aUri,std::wstring aFilter)
            :mUri(aUri),mFilter(aFilter) {}
    virtual ~PushRegData() {}
    PushRegData(const PushRegData& x)
    {
        *this = x;
    }
    PushRegData &operator=(const PushRegData& x)
    {
        mUri = x.mUri;
        mFilter = x.mFilter;
        return *this;
    }
    bool operator==(const PushRegData& x) const
    {
        return (mUri == x.mUri);
    }
    std::wstring mUri;
    std::wstring mFilter;
};

}// end namespace push
}// end namespace java

#endif // PUSHREGDATA_H

