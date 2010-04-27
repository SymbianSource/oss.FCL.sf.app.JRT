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
* Description:  ?Description
 *
*/


#include <jni.h>

/**
 * CJavaMmsRetriever is an abstract method that defines
 * the interface methods that are to be defined by the
 * OS Specific Class.
 */
namespace java
{
namespace wma
{

class MmsPropertyRetriever
{

public:

    /**
     * Gets the MMSC of the Device.
     * @param aJni - jni interface pointer
     */
    static jstring retrieveMsgCenter(JNIEnv& aJni);
    /*
     * CJavaMmsRetriever::isValidEncoding
     * Checks if the provided parameter points to a valid encoding which
     * could be used to encode a certain part/attachment of a multimedia
     * message.
     * @param aJni - jni interface pointer
     * @param encoding - encoding type
     */
    static bool isValidEncoding(JNIEnv& aJni, jstring encoding);
    /*
     * CJavaMmsRetriever::retrieveMaxMMSSize
     * Retrieves the maximum size of a multimedia message which can be
     * sent/received .
     */
    static int retrieveMaxMMSSize();
};

} //namespace wma
} //namespace java
