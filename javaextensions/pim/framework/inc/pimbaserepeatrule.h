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
* Description:  PIM item  base implementation.
 *
*/


#ifndef PIMBASEREPEATRULE_H
#define PIMBASEREPEATRULE_H

#include <jni.h>
#include "pimcommon.h"

class pimbaserepeatrule
{
public:

    virtual pimbaserepeatrule::~pimbaserepeatrule()
    {
    };

    virtual jintArray dates(jlong aStartDate, jlong aSubsetBeginning,
                            jlong aSubsetEnding, JNIEnv* aJniEnv) = 0;

    virtual void addExceptDate(jlong aValue) = 0;

    virtual void removeExceptDate(jlong aValue) = 0;

    virtual jintArray getExceptDates(JNIEnv* aJniEnv) = 0;

    virtual int getRepeatInt(TPIMField aField) = 0;

    virtual void setRepeatInt(TPIMField aField, int aValue) = 0;

    virtual jlong getRepeatDate(TPIMField aField) = 0;

    virtual void setDate(TPIMField aField, jlong aValue) = 0;

    virtual jintArray getFields(JNIEnv* aJniEnv) = 0;

    virtual void clear() = 0;

    static pimbaserepeatrule* getInstance();

};

#endif // PIMBASEREPEATRULE_H
