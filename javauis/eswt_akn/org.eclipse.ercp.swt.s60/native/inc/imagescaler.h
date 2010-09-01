/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef IMAGESCALER_H
#define IMAGESCALER_H

class CBitmapScaler;

NONSHARABLE_CLASS(CImageScaler)
        : public CActive
{
public:
    static CImageScaler* NewL();
    ~CImageScaler();
    void Scale(CFbsBitmap& aSrcBitmap, CFbsBitmap& aDestBitmap, TBool aKeepAspectRatio);

protected:
    void RunL();
    void DoCancel();

private:
    CImageScaler();
    void ConstructL();

private:
    CActiveSchedulerWait* iWait;
    CBitmapScaler* iBitmapScaler;
};


#endif // IMAGESCALER_H
