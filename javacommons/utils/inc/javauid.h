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
#ifndef JAVAUID_H
#define JAVAUID_H

#include <string>
#include <iostream>
#include <ostream>

namespace java
{
namespace util
{

/**
 * This class manages application id.
 * Note: THIS CLASS DOES NOT HAVE VIRTUAL DESTRUCTOR SO DO NOT INHERIT YOUR CLASS
 *       FROM THIS CLASS!
 */
class Uid
{
public:

    Uid() {}

    Uid(const std::wstring& aStr):mId(aStr) {}

    ~Uid() {}

    Uid(const Uid& aUid)
    {
        mId = aUid.mId;
    }

    Uid& operator= (const Uid& aUid)
    {
        mId = aUid.mId;
        return *this;
    }

    bool operator== (const Uid& aUid) const
    {
        return (mId == aUid.mId);
    }

    bool operator!= (const Uid& aUid) const
    {
        return !(mId == aUid.mId);
    }

    bool operator< (const Uid& aUid) const
    {
        return (mId < aUid.mId);
    }

    //bool operator() (const Uid& aUid)
    //{return (*this < aUid);}

    const std::wstring& toString() const
    {
        return mId;
    }

    bool isEmpty() const
    {
        return (0 == mId.size());
    }

private:

    std::wstring mId;

};

}//end namespace util
}//end namespace java

inline std::wostream& operator<< (std::wostream& aStream,const java::util::Uid& aUid)
{
    aStream << aUid.toString();
    return aStream;
}

inline std::ostream& operator<< (std::ostream& aStream,const java::util::Uid& aUid)
{
    std::wstring tmpWStr(aUid.toString());
    std::string tmpStr(tmpWStr.begin(),tmpWStr.end());
    aStream << tmpStr;
    return aStream;
}

inline std::wistream& operator>> (std::wistream& aStream,java::util::Uid& aUid)
{
    std::wstring tmpStr;
    aStream >> tmpStr;
    java::util::Uid tmpUid(tmpStr);
    aUid = tmpUid;
    return aStream;
}

inline std::istream& operator>> (std::istream& aStream,java::util::Uid& aUid)
{
    std::string tmpStr;
    aStream >> tmpStr;
    std::wstring tmpWStr(tmpStr.begin(),tmpStr.end());
    java::util::Uid tmpUid(tmpWStr);
    aUid = tmpUid;
    return aStream;
}

#endif // JAVAUID_H

