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
* Description:  FileTestConsts
 *
*/


#ifndef FILETESTCONSTS_H
#define FILETESTCONSTS_H

#include <string>

namespace java
{
namespace filetest
{

using namespace std;

class FileTestConsts
{
public:
    static wstring KDefaultTestPath;
    static bool KIncludeDRMInits;
};

wstring FileTestConsts::KDefaultTestPath(L"/home/jars/fileutilstestfolder/");
bool FileTestConsts::KIncludeDRMInits = false;

}  //end namespace filetest
}  //end namespace java

#endif // FILETESTCONSTS_H
