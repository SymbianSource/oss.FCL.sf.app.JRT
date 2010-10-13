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
* Description:  JavaCommonUtils
*
*/


#include <iostream>
#include <sstream>
#include <vector>
#include <string.h>
#include <cstdio>

#include "logger.h"
#include "javainifileutils.h"
#include "exceptionbase.h"

using namespace java::util;


OS_EXPORT int JavaIniFileUtils::getProperty(const std::string& property)
{
#ifdef RD_JAVA_INI_FILE_ACCESS_IN_USE
    FileContent fileContent = readFileContent();
    int result = -1;

    const char* position = getPosition(fileContent, property);
    if (position)
    {
        result = *position - 48;
//        ELOG1(EUtils, "Res: %d", result);
    }
    return result;
#else // RD_JAVA_INI_FILE_ACCESS_IN_USE
    int res = 0;
    if (property == PREWARM)
    {
        res = 0;
    }
    else if (property == UI_PREWARM)
    {
        res = 0;
    }

    else if (property == PREWARM_CAPTAIN_CONTROL)
    {
        res = 0;
    }

    else if (property == START_SCREEN_ENABLED)
    {
        res = 1;
    }

    else if (property == SIMPLE_START_SCREEN)
    {
        res = 0;
    }

    else if (property == START_SCREEN_SKIN_ENABLED)
    {
        res = 1;
    }

    else if (property == START_SCREEN_UPDATE_INTERVAL)
    {
        res = 8;
    }
    return res;

#endif // RD_JAVA_INI_FILE_ACCESS_IN_USE

}

#ifdef RD_JAVA_INI_FILE_ACCESS_IN_USE
OS_EXPORT void JavaIniFileUtils::setProperty(const std::string& property,
        int value)
{
    FileContent fileContent = readFileContent();
    char* position = getPosition(fileContent, property);
    if (position)
    {
        *position = (char)(value+48);
//        ELOG1(EUtils, "Res: %s", fileContent.getContent());
        writeFileContent(fileContent.getContent());
    }
    else
    {
        const char* origContent =  fileContent.getContent();
        std::string newContent;
        if (origContent && strlen(origContent) > 0)
        {
            newContent += origContent;
            newContent += (char)10;
        }
        newContent += property;
        newContent += ": ";
        newContent += (char)(value+48);
        writeFileContent(newContent.c_str());
    }
}
#else // RD_JAVA_INI_FILE_ACCESS_IN_USE
OS_EXPORT void JavaIniFileUtils::setProperty(const std::string& /*property*/,
        int /*value*/)
{
}
#endif // RD_JAVA_INI_FILE_ACCESS_IN_USE


const char* JavaIniFileUtils::getPosition(FileContent& content, const std::string& property)
{
    const char* data = content.getContent();
    if (data)
    {
        const char* position = strstr(data, property.c_str());
        if (position)
        {
            position += property.length()+2;

            return position;
        }
    }
    return 0;
}

FileContent JavaIniFileUtils::readFileContent(bool create)
{
    const char* fileName = "c:\\java\\javaini.txt";
    char*   data = 0;
    FILE*   iniFile;
    long    len = 0;
    iniFile = fopen(fileName, "r");
    if (iniFile == 0 && create)
    {
        iniFile = fopen(fileName, "a");
    }
    if (iniFile != 0)
    {
        fseek(iniFile, 0L, SEEK_END);
        len = ftell(iniFile);
        if (len < 0)
        {
            // error, return NULL content
            fclose(iniFile);
            return FileContent(data);
        }
        rewind(iniFile);
        data = new char[len+1];
        data[len] = 0;
        if (data != 0)
        {
            fread(data, sizeof(char), len, iniFile);
        }
        fclose(iniFile);
    }
    return FileContent(data);
}

void JavaIniFileUtils::writeFileContent(const char* content)
{
    FILE*   iniFile;
    iniFile = fopen("c:\\java\\javaini.txt", "w");
    if (iniFile != 0)
    {
        fwrite(content, 1, strlen(content), iniFile);
        fclose(iniFile);
    }
}
