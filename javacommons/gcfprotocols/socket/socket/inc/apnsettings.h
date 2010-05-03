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
* Description:  ApnSettings.h
*
*/


#ifndef APNSETTINGS_H
#define APNSETTINGS_H

namespace java
{

class ApnSettings
{

public :
    static int setDefaultApn(int aType, int aApn);
    static void removeDefaultApn();
};



}
#endif // APNSETTINGS_H
