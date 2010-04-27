/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This main entry point of Java processes.
*
*/

#include <string>
#include <javastarter.h>


int main(int argc, const char *argv[])
{
#ifdef __WINS__
    const char* const fileName = "c:\\java\\JvmArgs.txt";
#else
    const char* const fileName = "f:\\java\\JvmArgs.txt";
#endif
    int res = java::start(fileName);
    return res;
}
