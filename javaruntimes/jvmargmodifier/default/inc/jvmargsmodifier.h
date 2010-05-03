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
* Description:  A method for modifying the JVM args.
*
*/


#ifndef JVMARGSMODIFIER_H
#define JVMARGSMODIFIER_H

#include "javaoslayer.h"

#include <string>
#include <list>

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

/**
 * Provides a way for modifying the JVM arguments from some external
 * source. In the rom there will be an empty implementation of this method
 * which can be overridden by eclipsing javajvmargsmodifier.dll having this
 * method in ordinal 1. The modifier has full control of each argument passed
 * to the JVM. The arguments are divided into two categories - the arguments
 * for the JVM and the arguments for the application. Both are modifyable by
 * the external source modifier.
 * @param identifier An idetifier that can be used to distinguish different
 *                   runtimes.
 * @param jvmArgs    A list of Jvm args. This list contains all the arguments
 *                   before the main class.
 * @param applicationAndArgs A list that contains the main class as a first
 *                           item in the list. The leading item(s) in the
 *                           list are the arguments for the main method of
 *                           the main class.
 */
OS_IMPORT void modifyJvmArguments(const std::wstring& identifier,
                                  std::list<std::wstring>& jvmArgs,
                                  std::list<std::wstring>& applicationAndArgs);
} // end namespace runtime
} // end namespace java


#endif // JVMARGSMODIFIER_H
