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
* Description:  LinuxFileSystemUtils
 *
*/


#ifndef LINUXFILESYSTEMUTILS_H
#define LINUXFILESYSTEMUTILS_H

#include <string>

#include "jni.h"

namespace java
{
namespace fileutils
{

class LinuxFileSystemUtils
{
public:
    static unsigned long getAvailableSize(const char* aPath);

    static unsigned long getUsedSize(const char* aPath);

    static unsigned long getTotalSize(const char* aPath);

    static bool isHidden(const char* aFile);

    static int setHidden(const char* aFile, const bool aHide);

    static std::string listRoots();
};

}  //end namespace fileutils
}  //end namespace java

#endif // LINUXFILESYSTEMUTILS_H
