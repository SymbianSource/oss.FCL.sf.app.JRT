/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.internal.qt.OS;

public final class Style {

    /* Pixel metric ids, the same values as in QStyle::PixelMetric */
    public static final int QSTYLE_PM_BUTTONMARGIN = 0;
    public static final int QSTYLE_PM_DOCKWIDGETTITLEBARBUTTONMARGIN = OS.QStyle_swt_pm_dockWidgetTitleBarButtonMargin();
    public static final int QSTYLE_PM_BUTTONDEFAULTINDICATOR = 1;
    public static final int QSTYLE_PM_MENUBUTTONINDICATOR = 2;
    public static final int QSTYLE_PM_BUTTONSHIFTHORIZONTAL = 3;
    public static final int QSTYLE_PM_BUTTONSHIFTVERTICAL = 4;
    public static final int QSTYLE_PM_DEFAULTFRAMEWIDTH = 5;
    public static final int QSTYLE_PM_SPINBOXFRAMEWIDTH = 6;
    public static final int QSTYLE_PM_COMBOBOXFRAMEWIDTH = 7;
    public static final int QSTYLE_PM_MDISUBWINDOWFRAMEWIDTH = 46;
    public static final int QSTYLE_PM_MDISUBWINDOWMINIMIZEDWIDTH = OS.QStyle_swt_pm_mdiSubWindowMinimizedWidth();
    public static final int QSTYLE_PM_LAYOUTLEFTMARGIN = OS.QStyle_swt_pm_layoutLeftMargin();
    public static final int QSTYLE_PM_LAYOUTTOPMARGIN = OS.QStyle_swt_pm_layoutTopMargin();
    public static final int QSTYLE_PM_LAYOUTRIGHTMARGIN = OS.QStyle_swt_pm_layoutRightMargin();
    public static final int QSTYLE_PM_LAYOUTBOTTOMMARGIN = OS.QStyle_swt_pm_layoutBottomMargin();
    public static final int QSTYLE_PM_LAYOUTHORIZONTALSPACING = OS.QStyle_swt_pm_layoutHorizontalSpacing();
    public static final int QSTYLE_PM_LAYOUTVERTICALSPACING = OS.QStyle_swt_pm_layoutVerticalSpacing();
    public static final int QSTYLE_PM_MAXIMUMDRAGDISTANCE = 8;
    public static final int QSTYLE_PM_SCROLLBAREXTENT = 9;
    public static final int QSTYLE_PM_SCROLLBARSLIDERMIN = 10;
    public static final int QSTYLE_PM_SLIDERTHICKNESS = 11;
    public static final int QSTYLE_PM_SLIDERCONTROLTHICKNESS = 12;
    public static final int QSTYLE_PM_SLIDERLENGTH = 13;
    public static final int QSTYLE_PM_SLIDERTICKMARKOFFSET = 14;
    public static final int QSTYLE_PM_SLIDERSPACEAVAILABLE = 15;
    public static final int QSTYLE_PM_DOCKWIDGETSEPARATOREXTENT = 16;
    public static final int QSTYLE_PM_DOCKWIDGETHANDLEEXTENT = 17;
    public static final int QSTYLE_PM_DOCKWIDGETFRAMEWIDTH = 18;
    public static final int QSTYLE_PM_DOCKWIDGETTITLEMARGIN = OS.QStyle_swt_pm_dockWidgetTitleMargin();
    public static final int QSTYLE_PM_MENUBARPANELWIDTH = 33;
    public static final int QSTYLE_PM_MENUBARITEMSPACING = 34;
    public static final int QSTYLE_PM_MENUBARHMARGIN = 36;
    public static final int QSTYLE_PM_MENUBARVMARGIN = 35;
    public static final int QSTYLE_PM_TOOLBARFRAMEWIDTH = OS.QStyle_swt_pm_toolBarFrameWidth();
    public static final int QSTYLE_PM_TOOLBARHANDLEEXTENT = OS.QStyle_swt_pm_toolBarHandleExtent();
    public static final int QSTYLE_PM_TOOLBARITEMMARGIN = OS.QStyle_swt_pm_toolBarItemMargin();
    public static final int QSTYLE_PM_TOOLBARITEMSPACING = OS.QStyle_swt_pm_toolBarItemSpacing();
    public static final int QSTYLE_PM_TOOLBARSEPARATOREXTENT = OS.QStyle_swt_pm_toolBarSeparatorExtent();
    public static final int QSTYLE_PM_TOOLBAREXTENSIONEXTENT = OS.QStyle_swt_pm_toolBarExtensionExtent();
    public static final int QSTYLE_PM_TABBARTABOVERLAP = 19;
    public static final int QSTYLE_PM_TABBARTABHSPACE = 20;
    public static final int QSTYLE_PM_TABBARTABVSPACE = 21;
    public static final int QSTYLE_PM_TABBARBASEHEIGHT = 22;
    public static final int QSTYLE_PM_TABBARBASEOVERLAP = 23;
    public static final int QSTYLE_PM_TABBARSCROLLBUTTONWIDTH = OS.QStyle_swt_pm_tabBarScrollButtonWidth();
    public static final int QSTYLE_PM_TABBARTABSHIFTHORIZONTAL = OS.QStyle_swt_pm_tabBarTabShiftHorizontal();
    public static final int QSTYLE_PM_TABBARTABSHIFTVERTICAL = OS.QStyle_swt_pm_tabBarTabShiftVertical();
    public static final int QSTYLE_PM_PROGRESSBARCHUNKWIDTH = 24;
    public static final int QSTYLE_PM_SPLITTERWIDTH = 25;
    public static final int QSTYLE_PM_TITLEBARHEIGHT = 26;
    public static final int QSTYLE_PM_INDICATORWIDTH = 37;
    public static final int QSTYLE_PM_INDICATORHEIGHT = 38;
    public static final int QSTYLE_PM_EXCLUSIVEINDICATORWIDTH = 39;
    public static final int QSTYLE_PM_EXCLUSIVEINDICATORHEIGHT = 40;
    public static final int QSTYLE_PM_MENUPANELWIDTH = 30;
    public static final int QSTYLE_PM_MENUHMARGIN = 28;
    public static final int QSTYLE_PM_MENUVMARGIN = 29;
    public static final int QSTYLE_PM_MENUSCROLLERHEIGHT = 27;
    public static final int QSTYLE_PM_MENUTEAROFFHEIGHT = 31;
    public static final int QSTYLE_PM_MENUDESKTOPFRAMEWIDTH = 32;
    public static final int QSTYLE_PM_CHECKLISTBUTTONSIZE = 41;
    public static final int QSTYLE_PM_CHECKLISTCONTROLLERSIZE = 42;
    public static final int QSTYLE_PM_HEADERMARKSIZE = OS.QStyle_swt_pm_headerMarkSize();
    public static final int QSTYLE_PM_HEADERGRIPMARGIN = OS.QStyle_swt_pm_headerGripMargin();
    public static final int QSTYLE_PM_HEADERMARGIN = OS.QStyle_swt_pm_headerMargin();
    public static final int QSTYLE_PM_SPINBOXSLIDERHEIGHT = OS.QStyle_swt_pm_SpinBoxSliderHeight();
    public static final int QSTYLE_PM_TOOLBARICONSIZE = OS.QStyle_swt_pm_toolBarIconSize();
    public static final int QSTYLE_PM_SMALLICONSIZE = OS.QStyle_swt_pm_smallIconSize();
    public static final int QSTYLE_PM_LARGEICONSIZE = OS.QStyle_swt_pm_largeIconSize();
    public static final int QSTYLE_PM_FOCUSFRAMEHMARGIN = OS.QStyle_swt_pm_focusFrameHMargin();
    public static final int QSTYLE_PM_FOCUSFRAMEVMARGIN = OS.QStyle_swt_pm_focusFrameVMargin();
    public static final int QSTYLE_PM_ICONVIEWICONSIZE = OS.QStyle_swt_pm_iconViewIconSize();
    public static final int QSTYLE_PM_LISTVIEWICONSIZE = OS.QStyle_swt_pm_listViewIconSize();
    public static final int QSTYLE_PM_TOOLTIPLABELFRAMEWIDTH = OS.QStyle_swt_pm_toolTipLabelFrameWidth();
    public static final int QSTYLE_PM_CHECKBOXLABELSPACING = OS.QStyle_swt_pm_checkBoxLabelSpacing();
    public static final int QSTYLE_PM_RADIOBUTTONLABELSPACING = OS.QStyle_swt_pm_radioButtonLabelSpacing();
    public static final int QSTYLE_PM_TABBARICONSIZE = OS.QStyle_swt_pm_tabBarIconSize();
    public static final int QSTYLE_PM_SIZEGRIPSIZE = OS.QStyle_swt_pm_sizeGripSize();
    public static final int QSTYLE_PM_MESSAGEBOXICONSIZE = OS.QStyle_swt_pm_messageBoxIconSize();
    public static final int QSTYLE_PM_BUTTONICONSIZE = OS.QStyle_swt_pm_buttonIconSize();
    public static final int QSTYLE_PM_TEXTCURSORWIDTH = OS.QStyle_swt_pm_textCursorWidth();
    public static final int QSTYLE_PM_TABBAR_SCROLLBUTTONOVERLAP = OS.QStyle_swt_pm_tabBarScrollButtonOverlap();
    public static final int QSTYLE_PM_TABCLOSEINDICATORWIDTH = OS.QStyle_swt_pm_tabCloseIndicatorWidth();
    public static final int QSTYLE_PM_TABCLOSEINDICATORHEIGHT = OS.QStyle_swt_pm_tabCloseIndicatorHeight();
    
    public Style() {
    }

    /**
    * Retrieve pixel metric value for given metric id. 
    * @param id metric id
    * @return Pixel metric for given id or -1 in case of invalid id. 
    */
    public static int pixelMetric(int id) {
        return OS.QStyle_pixelMetric(id);
    }
    

}
