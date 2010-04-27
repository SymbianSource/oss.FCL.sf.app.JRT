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
* Description:  Common stuff for File Extended
 *
*/


#ifndef FILEEXTENDEDCOMMON_H
#define FILEEXTENDEDCOMMON_H

#include <jni.h>
#include <string>

namespace java
{
namespace fileutility
{

// NOTE!!! To be removed once a compatible convertion utility is available in java commons.
// Reason why it has been defined is present in the cpp file.
class FileUtil
{

public:
    static std::wstring jstringToWstring(JNIEnv* env, const jstring& jStr);

};

} // end namespace fileutility
} // end namespace java

#endif // FILEEXTENDEDCOMMON_H
