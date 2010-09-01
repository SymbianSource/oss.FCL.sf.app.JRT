/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTBROWSERDIALOGSPROVIDEROBSERVER_H
#define SWTBROWSERDIALOGSPROVIDEROBSERVER_H


#include <e32def.h>


/**
 * MSwtAknBrowserDialogsProviderObserver
 * @lib eswtapifacade.dll
 */
class MSwtBrowserDialogsProviderObserver
{
public:
    enum TDialogType
    {
        ENotifyError,
        ENotifyHttpError,
        EFileSelect,
        ESelectOption,
        EUserAuthentication,
        ENote,
        EAlert,
        EConfirm,
        EPrompt,
        EDownloadObject,
        EDisplayPageImages,
        ECancelAll,
        EUploadProgress,
        EMimeFileSelect
    };
public:
    virtual void ReportDialogEventL(TInt aType, TInt aFlags) = 0;
};

#endif // SWTBROWSERDIALOGSPROVIDEROBSERVER_H
