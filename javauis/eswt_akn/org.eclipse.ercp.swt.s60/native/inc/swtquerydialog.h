/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTQUERYDIALOG_H
#define SWTQUERYDIALOG_H


#include <eikseced.h>
#include <AknQueryDialog.h>
#include "eswtmobileextensions.h"


/**
 * CSwtQueryDialog
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtQueryDialog)
        : public CBase
{
public:
    enum TSwtQueryDialogStyle
    {
        ESwtQueryDialogStyleStandard,
        ESwtQueryDialogStylePassword,
        ESwtQueryDialogStyleNumeric,
        ESwtQueryDialogStyleTime,
        ESwtQueryDialogStyleDate
    };

public:
    static HBufC* RunDlgL(const TDesC& aTitle, TInt aStyle, TInt aQueryType,
                          const TDesC& aPromptText, const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum);
    static HBufC* ConvertDefaultValueL(const TDesC& aString, TInt aQueryType);
    static void ConvertJavaTime(TDes& aTime, const TDesC& aJavaTime);
    static void ConvertJavaDateL(TDes& aDate, const TDesC& aJavaDate);
};


/**
 * CSwtQueryDialogText
 * Used for both SWT.STANDARD and SWT.PASSWORD style
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtQueryDialogText)
        : public CAknTextQueryDialog
{
private:
    CSwtQueryDialogText(TInt aMinimum, TDes& aResult);

public:
    enum TTextDialogStyle
    {
        EStandardStyle = 0,
        EPasswordStyle
    };

    static TBool RunDlgL(const TDesC& aTitle, const TDesC& aPromptText,
                         const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum,
                         HBufC*& aResult, TTextDialogStyle aStyle);

// From CAknQueryDialog
protected:
    void UpdateLeftSoftKeyL();
    TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);

private:
    TInt iMinimum;
};


/**
 * CSwtQueryDialogNumeric
 * Used for SWT.NUMERIC style
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtQueryDialogNumeric)
        : public CAknFloatingPointQueryDialog
{
private:
    CSwtQueryDialogNumeric(TReal& aResult,
    const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum);

public:
    static TBool RunDlgL(const TDesC& aTitle, const TDesC& aPromptText,
                         const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum, HBufC*& aResult);

protected:
    void PostLayoutDynInitL();
    void UpdateLeftSoftKeyL();
    TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);

private:
    const TDesC& iDefaultValue;
    TInt iTextMinLength;
    TInt iTextMaxLength;
};


/**
 * CSwtQueryDialogDateTime
 * Used for SWT.TIME and SWT.DATE styles
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtQueryDialogDateTime)
        : public CAknTimeQueryDialog
{
private:
    CSwtQueryDialogDateTime(TTime& aTime);

public:
    enum TDateTimeDialogStyle
    {
        EDateStyle = 0,
        ETimeStyle
    };

    static TBool RunDlgL(const TDesC& aTitle, const TDesC& aPromptText,
                         const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum,
                         HBufC*& aResult, TDateTimeDialogStyle aDialogStyle);
};

#endif // SWTQUERYDIALOG_H
