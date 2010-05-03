/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef JAVAINIFILEUTILS_H
#define JAVAINIFILEUTILS_H

#include <string>
#include "javaosheaders.h"


namespace java
{
namespace util
{

class FileContent
{
public:
    FileContent(char* content) : mContent(content) {}
    ~FileContent()
    {
        delete mContent;
        mContent = 0;
    }
    char* getContent()
    {
        return mContent;
    }

private:
    char* mContent;
};

const char* const PREWARM                      = "Prewarm";
const char* const UI_PREWARM                   = "Uiprewarm";
const char* const PREWARM_CAPTAIN_CONTROL      = "PrewarmCaptainControl";
const char* const START_SCREEN_ENABLED         = "StartScreenEnabled";
const char* const SIMPLE_START_SCREEN          = "SimpleStartScreen";
const char* const START_SCREEN_SKIN_ENABLED    = "StartScreenSkinEnabled";
const char* const START_SCREEN_UPDATE_INTERVAL = "StartScreenUpdateInterval";

class JavaIniFileUtils
{
public:
    OS_IMPORT static int getProperty(const std::string& property);
    OS_IMPORT static void setProperty(const std::string& property, int value);

public:

private:
    static FileContent readFileContent(bool create=false);
    static void writeFileContent(const char* content);
    static char* getPosition(FileContent& content, const std::string& property);

};

} //end namespace util
} //end namespace java

#endif // JAVAINIFILEUTILS_H

