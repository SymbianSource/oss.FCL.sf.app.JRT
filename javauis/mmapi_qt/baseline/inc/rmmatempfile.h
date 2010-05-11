/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef RMMATEMPFILE_H
#define RMMATEMPFILE_H

// INCLUDES
#include <f32file.h>
#include <e32std.h>

//  CLASS DECLARATION
/**
* This is used to create and delete temporary files.
*/
NONSHARABLE_CLASS(RMMATempFile): public RSessionBase
{
public:
    /**
     * Default constructor.
     */
    RMMATempFile();

    /**
     * Connects to systemams server. This method must be called before
     * other methods may be used.
     */
    void ConnectL();

    /**
     * Creates temp file to server. After this call reference to file can
     * be obtained with File() method.
     */
    void CreateL();

    /**
     * Return reference to created file.
     * @return Reference to created file object.
     */
    RFile& File();

    /**
     * Sets midlet suite. Used to create temp file folder in systemams
     * server.
     * @param aMIDletSuiteID midlet suite id
     */
    void SetMIDletSuiteID(TInt aMIDletSuiteID);

    /**
     * Closes server session and deletes temp file.
     */
    void Close();

private:
    // temp file's path
    TFileName iFileName;

    // file created in CreateL method.
    RFile iFile;

    // Handle to file.
    TInt iFileHandle;

    // midlet suite id
    TInt iMidletSuiteID;
};

#endif // RMMATEMPFILE_H
