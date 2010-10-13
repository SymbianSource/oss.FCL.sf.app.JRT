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


#ifndef SWTROTATEIMAGE_H
#define SWTROTATEIMAGE_H


#include <e32base.h>


class MSwtControl;
class CBitmapRotator;
class CFbsBitmap;


/**
 * CAORotateImage
 * This class is an active object which make an asynchronous rotation
 * on several images. Used in CSwtSlider and CSwtProgressBar.
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CAORotateImage)
        : public CActive
{
public:
    /**
     * The 1st and 2nd phase constructor wrapper.
     */
    static CAORotateImage* NewL();

private:
    /**
     * Contructor.
     */
    CAORotateImage();

    /**
     * Second phase constructor
     */
    void ConstructL();

public:
    /**
     * Destructor.
     */
    ~CAORotateImage();

    /**
     * Remove all images previously added with the AddImage() or AddImages() methods.
     */
    void RemoveAllImages();

    /**
     * Add a image which will be rotate after a call to the method Start().
     */
    void AddImage(CFbsBitmap* aImage);

    /**
    * Add images which will be rotate after a call to the method Start().
    */
    void AddImages(const RArray<CFbsBitmap*>& aImages);

    /**
    * Indicate if a redraw should be make when the rotation of all images is finih,
    * the redraw works only if a Control is passed to the Start() method.
    * @param aRedrawAfterRotation If equal ETrue so a redraw will be make
    * when the rotation is finish, if EFalse no redraw will be make at the
    * end of the rotation.
    */
    void SetRedrawAfterRotation(TBool aRedrawAfterRotation);

    /**
    * Start the rotation of all images added with the AddImage() or AddImages() methods.
    * @param aControl   The control which should be redraw after the images rotation.
    * See SetRedrawAfterRotation() method.
    */
    void Start(MSwtControl* aControl = NULL);

    /**
    * Indicate if the rotation of all images id finish.
    * @return Returns Etrue if the rotation is started and finish.
    */
    TBool IsFinish() const;

protected:
    void DoCancel();
    void RunL();
    void Queue();
private:

    /**
     * Indicate the step in the rotation process.
     */
    TInt iStep;

    /**
     * Indicate if the rotation process is finish.
     */
    TBool iIsFinish;

    /**
     * Indicate if a redraw should be make after the rotation.
     */
    TBool iRedraw;

    /**
     * The Control will can be redraw after the rotation.
     */
    MSwtControl* iControl;

    /**
     * ASP for the image rotation.
     */
    CBitmapRotator* iBitmapRotator;

    /**
     * Contains the images will should be rotate.
     */
    RArray<CFbsBitmap*> iImages;
};


#endif // SWTROTATEIMAGE_H
