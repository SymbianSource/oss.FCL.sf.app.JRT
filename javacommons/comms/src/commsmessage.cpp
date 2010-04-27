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
* Description:  This class provides container for message.
*
*/

#include <string.h>

#include "javacommonutils.h"
#include "exceptionbase.h"

#include "commsmessage.h"
#include "logger.h"
#include "javauid.h"


namespace java
{
namespace comms
{
using namespace std;
using namespace java::util;


OS_EXPORT CommsMessage::CommsMessage()
        : mStream(ios::in | ios::out | ios::binary), mArray(0)
{
    memset(&mHeader, 0, sizeof(mHeader));
}


OS_EXPORT CommsMessage::CommsMessage(const ipcMessage_t* aMessage)
        : mStream(ios::in | ios::out | ios::binary), mArray(0)
{
    mHeader = aMessage->ipcHeader;

    mStream.clear();
    mStream.str(""); //reset
    mStream.write(aMessage->data, aMessage->ipcHeader.length-sizeof(aMessage->ipcHeader));

}


OS_EXPORT CommsMessage::CommsMessage(const CommsMessage& aMessage)
        : mStream(ios::in | ios::out | ios::binary), mArray(0)
{
    *this = aMessage;
}


OS_EXPORT CommsMessage& CommsMessage::operator=(const CommsMessage& aMessage)
{
    if (this != &aMessage)
    {
        mHeader = aMessage.mHeader;

        mStream.clear();
        mStream.str(""); //reset
        mStream << aMessage.toString();
    }
    return *this;
}


OS_EXPORT CommsMessage::~CommsMessage()
{
    delete[] mArray;
}


OS_EXPORT void CommsMessage::setMessageId(int aMessageId)
{
    mHeader.messageId = aMessageId;
}


OS_EXPORT void CommsMessage::setModuleId(int aModuleId)
{
    mHeader.moduleId = aModuleId;
}


OS_EXPORT void CommsMessage::setReceiver(int aReceiver)
{
    mHeader.receiver = aReceiver;
}


OS_EXPORT void CommsMessage::setSender(int aSender)
{
    mHeader.sender = aSender;
}


OS_EXPORT void CommsMessage::setMessageRef(int aMessageRef)
{
    mHeader.messageRef = aMessageRef;
}


OS_EXPORT int CommsMessage::getMessageId() const
{
    return mHeader.messageId;
}


OS_EXPORT int CommsMessage::getModuleId() const
{
    return mHeader.moduleId;
}


OS_EXPORT int CommsMessage::getReceiver() const
{
    return mHeader.receiver;
}


OS_EXPORT int CommsMessage::getSender() const
{
    return mHeader.sender;
}


OS_EXPORT bool CommsMessage::hasPermission(const CommsPermission& aPermission) const
{
    return (mHeader.permissions & aPermission) == aPermission;
}


OS_EXPORT int CommsMessage::getMessageRef() const
{
    return mHeader.messageRef;
}


OS_EXPORT void CommsMessage::replyTo(const CommsMessage& aMessage)
{
    setModuleId(aMessage.getModuleId());
    setMessageId(aMessage.getMessageId());
    setMessageRef(aMessage.getMessageRef());
    setReceiver(aMessage.getSender());
    setSender(aMessage.getReceiver());
}


OS_EXPORT CommsMessage& CommsMessage::operator << (const std::string& aStr)
{
    mStream << aStr.size() << " " << aStr;
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator << (const std::wstring& aStr)
{
    string str;
    try
    {
        char* utf8 = JavaCommonUtils::wstringToUtf8(aStr);
        str.assign(utf8);
        delete[] utf8;
    }
    catch (ExceptionBase& e)
    {
        ELOG2(EJavaComms, "%s: wstringToUtf8 failed: %s", __PRETTY_FUNCTION__, e.toString().c_str());
    }

    (*this) << str;

    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator << (const int& aInt)
{
    (*this) << JavaCommonUtils::intToString(aInt);
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator << (const long long& aLong)
{
    (*this) << JavaCommonUtils::longLongToString(aLong);
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator << (const java::util::Uid& aUid)
{
    (*this) << aUid.toString();
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator >> (std::wstring& aStr)
{
    string str;
    (*this) >> str;

    try
    {
        aStr = JavaCommonUtils::utf8ToWstring(str.c_str());
    }
    catch (ExceptionBase& e)
    {
        ELOG2(EJavaComms, "%s: utf8ToWstring failed: %s", __PRETTY_FUNCTION__, e.toString().c_str());
        aStr = L"";
    }

    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator >> (std::string& aStr)
{
    aStr.clear();
    int length = 0;

    if (mStream >> length)
    {
        //Below is skipped space character.
        if (!(mStream.get()))
        {
            ELOG2(EJavaComms, "ios::badbit - no space after length, invalid stream? - msgId=%d - moduleId=%d",
                  getMessageId(), getModuleId());
            mStream.setstate(ios::badbit);
            return *this;
        }

        char* retPtr = new char[length];
        if (!(mStream.read(retPtr,length)))
        {
            ELOG2(EJavaComms, "ios::badbit - unable read all, invalid stream? - msgId=%d - moduleId=%d",
                  getMessageId(), getModuleId());
            mStream.setstate(ios::badbit);
            delete [] retPtr;
            return *this;
        }
        aStr.append(retPtr, length);
        delete [] retPtr;
    }
    else
    {
        LOG2(EJavaComms, EInfo, "ios::badbit - nothing to read - msgId=%d - moduleId=%d",
             getMessageId(), getModuleId());
        mStream.setstate(ios::badbit);
    }
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator >> (int& aInt)
{
    string str;
    (*this) >> str;
    try
    {
        aInt = JavaCommonUtils::stringToInt(str);
    }
    catch (ExceptionBase& e)
    {
        aInt = 0;
        LOG3(EJavaComms, EInfo, "CommsMessage::operator >> (int& aInt) failed: %s - msgId=%d - moduleId=%d",
             e.toString().c_str(), getMessageId(), getModuleId());
    }
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator >> (long long& aLong)
{
    string str;
    (*this) >> str;
    try
    {
        aLong = JavaCommonUtils::stringToLongLong(str);
    }
    catch (ExceptionBase& e)
    {
        aLong = 0LL;
        LOG3(EJavaComms, EInfo, "CommsMessage::operator >> (long long& aLong) failed: %s - msgId=%d - moduleId=%d",
             e.toString().c_str(), getMessageId(), getModuleId());
    }
    return *this;
}


OS_EXPORT CommsMessage& CommsMessage::operator >> (java::util::Uid& aUid)
{
    std::wstring str;
    (*this) >> str;
    java::util::Uid uid(str);
    aUid = uid;
    return *this;
}


OS_EXPORT bool CommsMessage::operator !() const
{
    if ((mStream.fail()) || (mStream.bad()))
        return true;
    return false;
}


OS_EXPORT CommsMessage::operator void*() const
{
    if ((mStream.fail()) || (mStream.bad()))
        return 0;
    return const_cast<CommsMessage*>(this);
}


OS_EXPORT std::string CommsMessage::toString() const
{
    return mStream.str();
}


OS_EXPORT char* CommsMessage::toByteArray()
{
    // get size
    mStream.seekg(0, ios::end);
    int size = mStream.tellg();
    mStream.seekg(0, ios::beg);

    if (size < 0)
    {
        // S60 STL implementation sets error flag on and return size -1 if stream is empty
        size = 0;
        mStream.clear();
    }

    delete[] mArray;

    mArray = new char[size + sizeof(ipcHeader_t)];
    ipcMessage_t* msg = reinterpret_cast<ipcMessage_t*>(mArray);

    // get content
    mStream.read(msg->data, size);

    msg->ipcHeader = mHeader;
    msg->ipcHeader.length = size + sizeof(ipcHeader_t);

    return mArray;
}


OS_EXPORT void CommsMessage::begin()
{
    mStream.seekg(0, ios::beg);
}


OS_EXPORT void CommsMessage::reset()
{
    CommsMessage empty;
    *this = empty;
}

} // namespace comms
} // namespace java
