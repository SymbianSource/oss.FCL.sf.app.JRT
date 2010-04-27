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
* Description:  This file contains an implementation of Symbian MIME type recognizer
 *                for MIME types application/java-archive (.jar) and
 *                text/vnd.sun.j2me.app-descriptor (.jad)
 *
*/


#ifndef RECJAR_H
#define RECJAR_H

#if !defined(__APAFLREC_H__)
#include <apaflrec.h>
#endif

class CApaJarRecognizer : public CApaDataRecognizerType
{
public:
    // from CApaDataRecognizerType
    CApaJarRecognizer();
    TUint PreferredBufSize();
    TDataType SupportedDataTypeL(TInt aIndex) const;

    static CApaDataRecognizerType* CreateRecognizerL();

private:
    // from CApaDataRecognizerType
    void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
};

#endif // RECJAR_H
