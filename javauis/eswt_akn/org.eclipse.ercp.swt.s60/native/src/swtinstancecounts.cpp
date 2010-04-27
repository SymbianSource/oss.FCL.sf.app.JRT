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


#if defined(__WINS__) && defined(_DEBUG)


#include "swtinstancecounts.h"
#include "swtdisplay.h" // needed for macro definitions


// ======== LOCAL FUNCTIONS ========


static void CheckLeak(TInt& aCount, TInt aCorrectCount=0)
{
    // The reason for stopping here is that eSWT Display is being disposed
    // but not all the eSWT resources have been appropriately freed by
    // calling dispose(). This creates a memory leak. Step up the call stack
    // into CheckForLeaks() and you will see which type of object was leaked.
    // Likely reasons for this error are:
    // - The Java application not disposing its eSWT resources appropriately
    //   (e.g. Images, Fonts, GCs, Colors)
    // - eSWT widget not appropriately disposing another eSWT object it owns
    //   when being disposed itself
#ifdef ESWT_NATIVE_UITHREAD_OWN_HEAP
    ASSERT(aCount == aCorrectCount);
#endif // ESWT_NATIVE_UITHREAD_OWN_HEAP

    if (aCount != aCorrectCount)
    {
        aCount = aCorrectCount;
    }
}


// ======== MEMBER FUNCTIONS ========


void TSwtInstanceCounts::CheckForLeaks()
{
    CheckLeak(iColor);
    CheckLeak(iFont);
    CheckLeak(iGc);
    CheckLeak(iImage);
    CheckLeak(iScrollBar);
    CheckLeak(iMenu);
    CheckLeak(iMenuItem);
    CheckLeak(iCommand);
    CheckLeak(iClipboard);
    CheckLeak(iTableColumn);
    CheckLeak(iTableItem);
    CheckLeak(iTreeItem);

    // Controls deserve a special treatment as we don't know precisely which has been disposed
    const TInt ctrlSum = iButton         + iCanvas            + iCaptionedControl  + iCombo
                         + iComposite      + iConstrainedText   + iDateEditor        + iDecorations
                         + iHyperLink      + iLabel             + iList              + iListBox
                         + iListView       + iMultiPagePager    + iMultiPageTabGroup + iMultiPageDialog
                         + iProgressBar    + iShell             + iSlider            + iSortedList
                         + iText           + iTextExtension     + iTable             + iTree
                         + iBrowser        + iMobileShell       + iDCControl;
    CheckLeak(iDisposedControls, ctrlSum);
}


#endif // defined(__WINS__) && defined(_DEBUG)
