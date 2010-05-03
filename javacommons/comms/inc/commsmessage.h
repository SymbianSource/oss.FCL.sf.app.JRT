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


#ifndef COMMSMESSAGE_H
#define COMMSMESSAGE_H

#include <string>
#include <sstream>

#include "javaosheaders.h"
#include "transport.h"
#include "commspermissions.h"

namespace java
{
namespace util
{
class Uid;
}//end util
namespace comms
{

/**
* CommsMessage class provides container for message payload. It provides an
* interface to manipulate payload as if it was input/output stream.
*
* Comms message consists of header fields and message body.
* Header is used to route message to correct endpoint and listener.
* Header consists of following fields:
* - module id, identifies which listener is used to handle message
* - message id, identifies message
* - receiver, identifies receiving endpoint
* - sender, identifies sending endpoint
* - message reference, identifies message
*
* Message payload can be read and write using stream operations (<<, >>).
* Message can contain multiple items (ints, strings etc) that must be
* read in the same order as they were originally written.
*
* There is no limit to message size but some other interprocess communication mechanism
* should be considered if message sizes exceed 60k bytes.
*
* Usage:
* It is important that application fills message headers correctly as
* message routing and dispatching is done based on header information.
* Following headers should be set always: module id, message id, message reference
* and receiver.
* @code
* CommsMessage msg;
*
* // Following headers must be set always
* msg.setModuleId(MY_MODULE_ID);
* msg.setMessageId(MY_MESSAGE_ID);
*
* // Following header should be set if replying
* msg.setMessageRef(aMesssage.getMessageRef());
*
* // If CommsServerEndpoint is used then also receiver header must be filled
* msg.setReceiver(aMessage.getSender());
*
* // or use replyTo() method instead of setting each header field separately
*
* // Message payload can be written using insert operator
* msg << "this is a string" << 1234 << L"this is a wstring";
*
* // Message payload can be read using extract operator
* std::string s;
* std::wstring w;
* int i;
* msg >> s >> i >> w;
* @endcode
*
* @see CommsListener, CommsEndpoint
*/
class CommsMessage
{
public:
    /**
    * Constructs a message.
    * Content is initialized to an empty stream.
    */
    OS_IMPORT CommsMessage();

    /**
    * Constructs a message.
    * Content is initialized to a copy of the IPC message pointer
    * @param[in] aMessage Pointer to IPC message, which content is used to initialize the content of this
    */
    OS_IMPORT CommsMessage(const ipcMessage_t* aMessage);

    /**
    * Constructs a message.
    * Content is initialized to a copy of the CommsMessage object
    * @param[in] aMessage Object whose content is used to initialize the content of this
    */
    OS_IMPORT CommsMessage(const CommsMessage& aMessage);

    /**
    * A destructor.
    */
    OS_IMPORT virtual ~CommsMessage();

    /**
    * Message assignment.
    * Sets a copy of the argument as the new content for this object.
    * The previous content is dropped.
    * @code
    * CommsMessage a, b;
    * a << "hello world";
    * b = a;
    * @endcode
    * @param[in] aMessage Comms message. A copy of the content of this object is used as the new content for the object
    */
    OS_IMPORT CommsMessage& operator=(const CommsMessage& aMessage);

    /**
     * Sets message id, identifies message type
     * @param aMessageId message id
     */
    OS_IMPORT void setMessageId(int aMessageId);

    /**
     * Sets module id, identifies which listener is used to handle message
     * @param aModuleId module id
     */
    OS_IMPORT void setModuleId(int aModuleId);

    /**
     * Sets message reference, identifies message
     * @param aMessageRef message reference
     */
    OS_IMPORT void setMessageRef(int aMessageRef);

    /**
     * Sets receiver, identifies receiving endpoint
     * @param aReceiver receiver
     */
    OS_IMPORT void setReceiver(int aReceiver);

    /**
     * Sets sender, identifies sending endpoint
     * @param aSender sender
     */
    OS_IMPORT void setSender(int aSender);

    /**
     * Returns message id, identifies message type
     * @return message id
     */
    OS_IMPORT int getMessageId() const;

    /**
     * Returns module id, identifies which listener is used to handle message
     * @return module id
     */
    OS_IMPORT int getModuleId() const;

    /**
     * Returns message reference, identifies message
     * @return message reference
     */
    OS_IMPORT int getMessageRef() const;

    /**
     * Returns receiver, identifies receiving endpoint
     * @return receiver
     */
    OS_IMPORT int getReceiver() const;

    /**
     * Returns sender, identifies sending endpoint
     * @return sender
     */
    OS_IMPORT int getSender() const;

    /**
     * Checks what permissions sender of the message has.
     * CommsPermission enumeration defines the set of supported permissions.
     * @code
     * MyListener::processMessage(CommsMessage& aMessage)
     * {
     *     if( aMessage.hasPermission(MANAGE_CERTIFICATES) )
     *     {
     *         // manage certificates
     *     }
     * }
     * @endcode
     * @see CommsPermission
     * @return true, if sender of the message has given permission
     */
    OS_IMPORT bool hasPermission(const CommsPermission& aPermission) const;

    /**
     * Sets headers so that reply is sent to given message.
     * @code
     * CommsMessage msg;
     * msg.replyTo(aMessage);
     * // is equivalent to
     * msg.setModuleId(aMessage.getModuleId());
     * msg.setMessageId(aMessage.getMessageId());
     * msg.setMessageRef(aMessage.getMessageRef());
     * msg.setReceiver(aMessage.getSender());
     * msg.setSender(aMessage.getReceiver());
     * @endcode
     * @param aMessage Address where the reply should be sent
     * @return -
     */
    OS_IMPORT void replyTo(const CommsMessage& aMessage);

    /**
     * Insert data to message.
     * @code
     * CommsMessage msg;
     * // Message payload can be written using insert operator
     * msg << "this is a string" << 1234 << L"this is a wstring";
     * @endcode
     * @param aStr A value to be inserted
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator << (const std::string& aStr);

    /**
     * Insert data to message.
     * @code
     * CommsMessage msg;
     * // Message payload can be written using insert operator
     * msg << "this is a string" << 1234 << L"this is a wstring";
     * @endcode
     * @param aStr A value to be inserted
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator << (const std::wstring& aStr);

    /**
     * Insert data to message.
     * @code
     * CommsMessage msg;
     * // Message payload can be written using insert operator
     * msg << "this is a string" << 1234 << L"this is a wstring";
     * @endcode
     * @param aInt A value to be inserted
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator << (const int& aInt);

    /**
     * Insert data to message.
     * @code
     * CommsMessage msg;
     * // Message payload can be written using insert operator
     * msg << "this is a string" << 1234LL << L"this is a wstring";
     * @endcode
     * @param aLong A value to be inserted
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator << (const long long& aLong);

    /**
     * Insert data to message.
     * @param aUid A value to be inserted
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator << (const java::util::Uid& aUid);

    /**
     * Extract data from message.
     * @code
     * CommsMessage msg;
     * // Message payload can be read using extract operator
     * std::string s;
     * std::wstring w;
     * int i;
     * msg >> s >> i >> w;
     * @endcode
     * @param aStr String that contains extracted data
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator >> (std::wstring& aStr);

    /**
     * Extract data from message.
     * @code
     * CommsMessage msg;
     * // Message payload can be read using extract operator
     * std::string s;
     * std::wstring w;
     * int i;
     * msg >> s >> i >> w;
     * @endcode
     * @param aStr String that contains extracted data
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator >> (std::string& aStr);

    /**
     * Extract data from message.
     * @code
     * CommsMessage msg;
     * // Message payload can be read using extract operator
     * std::string s;
     * std::wstring w;
     * int i;
     * msg >> s >> i >> w;
     * @endcode
     * @param aInt Integer that contains extracted data
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator >> (int& aInt);

    /**
     * Extract data from message.
     * @code
     * CommsMessage msg;
     * // Message payload can be read using extract operator
     * std::string s;
     * std::wstring w;
     * long long i;
     * msg >> s >> i >> w;
     * @endcode
     * @param aLong Integer that contains extracted data
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator >> (long long& aLong);

    /**
     * Extract data from message.
     * @code
     * java::util::Uid uid;
     * msg >> uid;
     * @endcode
     * @param aUid Uid object that contains extracted data
     * @return The object itself (*this).
     */
    OS_IMPORT CommsMessage& operator >> (java::util::Uid& aUid);

    /**
    * Sets the position of the get pointer to the beginning of the stream.
    * The get pointer determines the next location to be read from the source stream
    * @code
    * CommsMessage msg;
    * msg << 1 << 2;
    * int a, b;
    * msg >> a;
    * msg.begin();
    * msg >> b;
    * // a == b == 1
    * @endcode
    * @param -
    * @return -
    */
    OS_IMPORT void begin();

    /**
    * The reset() function resets the header and deletes the contents of the stream.
    * @code
    * CommsMessage msg, empty;
    * msg.setMessageId(MY_MESSAGE_ID);
    * msg << 1 << 2;
    * msg.reset();
    * // msg == empty== true
    * @endcode
    * @param -
    * @return -
    */
    OS_IMPORT void reset();

    /**
    * Converts CommsMessage to byte array.
    *
    * The returned array points to an internal location with the required storage space
    * for this sequence of characters , but the values in this array should not be
    * modified in the program and are only granted to remain unchanged until the next
    * call to a non-constant member function of the CommsMessage object.
    *
    * Returned byte array can be casted to ipcMessage_t* if needed.
    * @code
    * char* arr = aMsg.toByteArray();
    * ipcMessage_t& msg = *(ipcMessage_t*)arr;
    * @endcode
    * @param -
    * @return bytearray
    */
    OS_IMPORT char* toByteArray();

    /**
    * Get the message's current payload as string.
    * @param -
    * @return String whose content is copied from the message's stream buffer
    */
    OS_IMPORT std::string toString() const;

    /**
    * Evaluate message's internal stream object state.
    * Returns true if either one of the error flags (failbit or badbit)
    * is set on the stream. Otherwise it returns false.
    *
    * This behavior is equivalent to std::ios::fail().
    * @code
    * CommsMessage msg;
    * msg << "string1" << "string2" << "string3";
    * string a, temp;
    * msg >> a;
    * while(!(msg>>temp)); // ignore rest
    * @endcode
    * @param -
    * @return true if either failbit or badbit is set.
    * @return false otherwise.
    */
    OS_IMPORT bool operator !() const;

    /**
    * Converts message's internal stream object to pointer.
    * This pointer is a null pointer if either one of the error flags (failbit or badbit) is set,
    * otherwise it is a non-zero pointer.
    * The pointer returned is not intended to be referenced, it just indicates success when
    * none of the error flags are set.
    * @code
    * CommsMessage msg;
    * msg << "string";
    * if ( (void*)msg == 0)
    * {
    *    cerr << "something wrong with the message";
    * }
    * @endcode
    * @param -
    * @return null pointer if either failbit or badbit is set on object internal message stream.
    * @return A non-null pointer otherwise.
    */
    OS_IMPORT operator void*() const;

private:
    std::stringstream  mStream;
    ipcHeader_t        mHeader;
    char*              mArray;
};

} // namespace comms
} // namespace java

#endif // COMMSMESSAGE_H
