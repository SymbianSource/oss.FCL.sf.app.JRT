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


#ifndef SWTINSTANCECOUNTS_H
#define SWTINSTANCECOUNTS_H


#if defined(__WINS__) && defined(_DEBUG)


#include <e32std.h>


/**
 * Poorman's leak detection tool.
 * A basic structure that keeps track of how many objects of each type have been
 * created and disposed.
 */
struct TSwtInstanceCounts
{
    TInt iColor;
    TInt iFont;
    TInt iGc;
    TInt iImage;
    TInt iScrollBar;
    TInt iMobileDevice;
    TInt iInput;
    TInt iScreen;
    TInt iClipboard;

    // Controls
    TInt iButton;
    TInt iDisposedControls;
    TInt iCanvas;
    TInt iCaptionedControl;
    TInt iCombo;
    TInt iComposite;
    TInt iConstrainedText;
    TInt iDecorations;
    TInt iHyperLink;
    TInt iLabel;
    TInt iLink;
    TInt iList;
    TInt iListBox;
    TInt iListView;
    TInt iShell;
    TInt iText;
    TInt iTextExtension;
    TInt iProgressBar;
    TInt iSlider;
    TInt iTable;
    TInt iTree;
    TInt iBrowser;
    TInt iDCControl;

    // Other widgets
    TInt iCommand;
    TInt iDateEditor;
    TInt iMenu;
    TInt iMenuItem;
    TInt iMobileShell;
    TInt iMultiPagePager;
    TInt iMultiPageTabGroup;
    TInt iMultiPageDialog;
    TInt iSortedList;
    TInt iTableColumn;
    TInt iTableItem;
    TInt iTreeItem;
    TInt iTaskTip;

    void CheckForLeaks();

    inline TSwtInstanceCounts()
    {
        Mem::FillZ(this, sizeof(*this));
    }
};


#define INCREASE_INSTANCE_COUNT_USE_DISPLAY(handle, classname, display) if (handle!=0) ++(display->iInstanceCounts.i##classname)
#define INCREASE_INSTANCE_COUNT(handle, classname) if (handle!=0) ++(CSwtDisplay::Current().iInstanceCounts.i##classname)
#define DECREASE_INSTANCE_COUNT_USE_DISPLAY(classname, display) --(display->iInstanceCounts.i##classname)
#define DECREASE_INSTANCE_COUNT(classname) --(CSwtDisplay::Current().iInstanceCounts.i##classname)


#else // defined(__WINS__) && defined(_DEBUG)


#define INCREASE_INSTANCE_COUNT_USE_DISPLAY(handle, classname, display)
#define INCREASE_INSTANCE_COUNT(pointer, classname)
#define DECREASE_INSTANCE_COUNT_USE_DISPLAY(classname, display)
#define DECREASE_INSTANCE_COUNT(classname)


#endif // defined(__WINS__) && defined(_DEBUG)


#endif // SWTINSTANCECOUNTS_H
