/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MREFHTTPCLIENTOBSERVER_H
#define MREFHTTPCLIENTOBSERVER_H

class MRefHttpClientObserver
{
public:
    virtual void SubmitComplete(TInt aStatus) =0 ;
    virtual void DataReadyForRead(TInt aStatus) = 0;
    virtual void DoPostCallBack() = 0;
};

#endif // MREFHTTPCLIENTOBSERVER_H

