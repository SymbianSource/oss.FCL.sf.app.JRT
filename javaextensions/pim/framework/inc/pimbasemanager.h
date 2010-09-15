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
* Description:  PIM manager singleton native side, a starting point to PIM API.
 *
*/


#ifndef PIMBASEMANAGER_H
#define PIMBASEMANAGER_H

#include "pimcommon.h"

class MPIMLocalizationManager;
class pimbaselist;

class pimbasemanager
{
public:

    virtual pimbasemanager::~pimbasemanager()
    {
    };

    virtual MPIMLocalizationManager* localizationManager() const = 0;

    virtual pimbaselist* openPimList(const TPIMListType& aPimListType,
                                     jstring aPimListName, jstring aCalName,
                                     JNIEnv* aJniEnv) = 0;

    virtual jobjectArray listPimLists(const TPIMListType& aPimListType,
                                      jintArray aError, JNIEnv* aJniEnv) = 0;
    virtual jobjectArray listCalendars(jintArray aError,
                                       JNIEnv* aJniEnv) = 0;
    virtual jobjectArray listCalendarNames(jintArray aError,
                                           JNIEnv* aJniEnv) = 0;

    virtual void dispose()= 0;

    virtual void createCalendar(jstring aCalName, jstring aDisplayName, JNIEnv* aJniEnv) = 0;

    virtual void deleteCalendar(jstring aCalName, JNIEnv* aJniEnv) = 0;

    static pimbasemanager* getInstance();
};

#endif // PIMBASEMANAGER_H
