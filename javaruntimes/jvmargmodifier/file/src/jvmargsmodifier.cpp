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
* Description:  A default empty implementation for JvmArgs modifier.
*
*/

#include <algorithm>

#include "jvmargsmodifier.h"
#include "logger.h"

#include "jvmargsfilereader.h"
#include "argsmodifier.h"

#ifdef __SYMBIAN32__
#include <AknGlobalNote.h>
_LIT(KArgsTxt, "Note! Modified VM arguments used!");
#endif

void logArguments(std::wstring aArg)
{
    WLOG1(EJavaRuntime, " '%S'", aArg.c_str());
}

#ifdef __SYMBIAN32__
void showWarningDialogL()
{
    ::CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
    CleanupStack::PushL(globalNote);
    globalNote->ShowNoteL(EAknGlobalInformationNote, KArgsTxt);
    CleanupStack::PopAndDestroy(globalNote);
}
#endif


OS_EXPORT
void java::runtime::modifyJvmArguments(const std::wstring& aIdentifier,
                                       std::list<std::wstring>& aJvmArgs,
                                       std::list<std::wstring>& aApplicationAndArgs)
{
    JELOG2(EJavaRuntime);

#ifdef __SYMBIAN32__
    // Open the extension directory of the J9 VM.
    aJvmArgs.push_back(L"-Dcom.ibm.j9.ext.dirs=c:\\resource\\java\\jvm\\lib\\common;z:\\resource\\java\\jvm\\lib\\common");
#endif

    JvmArgsFileReader reader(aIdentifier);
    JvmArgsModifier modifier;
    std::list<std::wstring> newJvmArgs = reader.getJvmArguments();
    std::list<std::wstring> newAppArgs = reader.getApplicationArguments();

    modifier.combineArguments(aJvmArgs, newJvmArgs);
    modifier.combineArguments(aApplicationAndArgs, newAppArgs);

    if (newJvmArgs.size() != 0 || newAppArgs.size() != 0)
    {
        WLOG(EJavaRuntime, "new JVM arguments");
        std::for_each(aJvmArgs.begin(), aJvmArgs.end(), logArguments);
        WLOG(EJavaRuntime, "new App arguments");
        std::for_each(aApplicationAndArgs.begin(), aApplicationAndArgs.end(), logArguments);
#ifdef __SYMBIAN32__
        TRAP_IGNORE(showWarningDialogL());
#endif
    }
}

