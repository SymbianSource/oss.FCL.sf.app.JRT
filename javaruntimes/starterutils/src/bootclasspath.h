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
* Description:  This class provides container for message.
*
*/


#ifndef BOOTCLASSPATH_H
#define BOOTCLASSPATH_H

#include <string>

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

/**
 * Finds the add-on JSRs from platform dependent repository.
 * @param bootClassPath a refernce to wstring where the method can store
 *                      the boot classpath. If the there are no add-on JSRs,
 *                      this will be empty wstring.
 */
void getExtendedBootClassPath(std::wstring& bootClassPath);

} // namespace runtime
} // end namespace java

#endif // BOOTCLASSPATH_H
