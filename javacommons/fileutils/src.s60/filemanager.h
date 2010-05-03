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
* Description:  FileManager - Provides file management facility
 *
*/


#ifndef FILEMANAGER_H
#define FILEMANAGER_H

// OpenC Includes
#include <string>

// Symbian Includes
#include <f32file.h>

#include "javaosheaders.h"

namespace java
{
namespace fileutils
{

OS_NONSHARABLE_CLASS(FileManager)
{
public:
    static bool copy(std::wstring aSource, std::wstring aDest, bool overwrite);
    static bool move(std::wstring aSource, std::wstring aDest, bool overwrite);
    static bool copyAll(std::wstring aSource, std::wstring aDest,
                        bool overwrite);
    static bool deleteAll(std::wstring aSource);
};

} //end namespace fileutils
} //end namespace java

#endif // FILEMANAGER_H
