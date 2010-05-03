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


#ifndef SWTLAFFACADE_H
#define SWTLAFFACADE_H


#include <AknUtils.h>

/**
 * CSwtLafFacade
 * Class for providing layout-related functionality not found in S60 SDK for eSWT.
 * @lib eswtapifacade.dll
 */
class CSwtLafFacade
        : public CBase
{
public:

    // Default scroll span, thumb span and thumb position for ScrollBar.
    static const TInt KSwtDefaultScrollBarModelScrollSpan    = 100;
    static const TInt KSwtDefaultScrollBarModelThumbSpan     = 50;
    static const TInt KSwtDefaultScrollBarModelThumbposition = 0;

    // Layout information structure returned by GetChoiceListPaneLayout.
    struct TSwtLafFacadeWindowLineLayout
    {
        TInt16 iC;
        TInt16 il;
        TInt16 it;
        TInt16 ir;
        TInt16 ib;
        TInt16 iW;
        TInt16 iH;
    };

    // Layout information structure returned by GetChoiceListPaneT1Layout.
    struct TSwtLafFacadeTextLineLayout
    {
        TInt16 iC;
        TInt16 il;
        TInt16 ir;
        TInt16 iB;
        TInt16 iW;
        TInt16 iJ;
        TInt32 iFont;
    };

    // Component id's for GetLayoutRect.
    enum TSwtLafFacadeRectId
    {
        EUndefinedRectId,

        /**
         * main_eswt_pane(0): classic status pane
         * main_eswt_pane(1): small status pane
         * main_eswt_pane(2): no status pane
         * main_eswt_pane(3): full screen
         */
        EMainEswtPane,

        EControlButtonPaneG2,
        EToolbarButtonPaneG2,
        EToolbarButtonPaneG5,
        EInputFieldSkinPlacingGeneralLine2,
        EInputFieldSkinPlacingGeneralLine5,
        EEswtControlPaneG1,
        EEswtControlPaneG2,
        EEswtControlPaneG3,
        EEswtControlPaneG4,
        EPopupWindowsSkinPlacingFrameGeneralLine2,
        EPopupWindowsSkinPlacingFrameGeneralLine5,
        EForm2MidpLabelPane,
        ESeparatorVerticalPane,
        ESeparatorVerticalPaneG1,
        ESeparatorVerticalPaneG2,
        ESeparatorVerticalPaneG3,
        ESeparatorHorizontalPane,
        ESeparatorHorizontalPaneG1,
        ESeparatorHorizontalPaneG2,
        ESeparatorHorizontalPaneG3,
        EListScrollGenPane,
        EListGenPane,
        EListSinglePane,
        EListSinglePaneG1,
        EListSinglePaneG1Cp2,
        EListSingleNumberPane,
        EListSingleNumberPaneG1,
        EListSingleGraphicH1Pane,
        EListSingleGraphicPane,
        EListSingleGraphicPaneG1,
        EListSingleGraphicPaneG2,
        EListSingleLargeGraphicPane,
        EListSingleLargeGraphicPaneG1,
        EListSingleLargeGraphicPaneG2,
        EListSingle2GraphicPaneG1Cp4,
        EListSingleHeadingPane,
        EListSingleHeadingPaneG1,
        EListSingle2HeadingMsgPane,
        EListSingle2HeadingMsgPaneG1,
        EListSingle2HeadingMsgPaneG2,
        EListSingleNumberHeadingPane,
        EListSingleNumberHeadingPaneG1,
        EListSingleGraphicHeadingPane,
        EListSingleGraphicHeadingPaneG1,
        EListSingleGraphicHeadingPaneG4,
        EListDoublePane,
        EListDoublePaneG1,
        EListDoubleNumberPane,
        EListDoubleNumberPaneG1,
        EListDoubleGraphicPane,
        EListDoubleGraphicPaneG1,
        EListDoubleGraphicPaneG2,
        EListDoubleLargeGraphicPane,
        EListDoubleLargeGraphicPaneG1,
        EListDoubleLargeGraphicPaneG2,
        EESwtCtrlWaitPane,
        EESwtCtrlWaitPaneH,
        EWaitBorderPaneG1,
        EWaitBorderPaneG2,
        EWaitBorderPaneG3,
        ECellEswtAppPane,
        ECellEswtAppPaneG1,
        ECellEswtAppPaneG2,
        EListHighlightSkinPlacingGeneralLine2,
        EListHighlightSkinPlacingGeneralLine5,
        EButtonValueAdjustPane,
        EButtonValueAdjustPaneG1,
        EButtonValueAdjustPaneG2,
        ESliderSetPaneCP3,
        ESliderSetPaneG1,
        ESliderSetPaneG2,
        EListSettingNumberPane,
        ESetValuePaneVC,
        ESetValuePane,
        EBgSetOptPane,
        EBgSetOptPaneG2,
        EBgSetOptPaneG5,
        EChildShellWithTitlePane,
        EChildShellWithoutTitlePane,
        EMenuItemPane,
        ESubMenuItemPane,

        /**
         * popup_eswt_tasktip_window(0): task tip bar + label
         * popup_eswt_tasktip_window(1): task tip bar or label only
         */
        EPopupEswtTasktipWindow,

        /**
         * wait_bar_pane_cp71(0): progress bar in task tip bar + label
         * wait_bar_pane_cp71(1): progress bar in task tip bar or label only
         */
        EWaitBarPaneCp71,

        EChildShellTitleImagePane,
        EFindPane,
        EPopupMidpNoteAlarmWindowG1,
        EPopupWindowGeneral,
        EHeadingPane,

        ECellHcAppPane,
        ECellHcAppPaneG1,
        ECellHcAppPaneG2,
        ECellAppPane,
        ECellAppPaneG1,
        ECellAppPaneG2,

        EAreaBottomPane,
    };

    // Component id's for GetLayoutText.
    enum TSwtLafFacadeTextId
    {
        EUndefinedTextId,
        EListSinglePaneT1,
        EListSinglePaneT1Cp2,
        EListSingleGraphicPaneT1,
        EListSingleLargeGraphicPaneT1,
        EListSingleNumberPaneT1,
        EListSingleHeadingPaneT1,
        EListSingle2HeadingMsgPaneT1,
        EListSingleNumberHeadingPaneT1,
        EListSingleGraphicHeadingPaneT1,
        EListSinglePopupSubmenuPaneT1,
        EListDoublePaneT1,
        EListDoublePaneT2,
        EListDoubleGraphicPaneT1,
        EListDoubleGraphicPaneT2,
        EListDoubleNumberPaneT2,
        EListDoubleNumberPaneT3,
        EListDoubleLargeGraphicPaneT1,
        EListDoubleLargeGraphicPaneT2,
        ECellEswtAppPaneT1,
        EFormDataWideGraphicFieldTextsLine2,

        /**
         * popup_eswt_tasktip_window_t1(0): label in task tip bar + label
         * popup_eswt_tasktip_window_t1(1): label in task tip bar or label only
         */
        EPopupEswtTasktipWindowT1,
        ETabs4ActivePaneT1,

        ECellHcAppPaneT1,
        ECellAppPaneT1,
    };

    // Component id's for GetFondId.
    enum TSwtLafFacadeFontId
    {
        EUndefinedFontId,
        EForm2MidpLabelPaneT1Font,
        EFont2MidpTextPaneT1Font,
        EFont2MidpTimePaneT1Font,
        ECellEswtAppPaneT1Font,
        EListSingleGraphicPaneT1Font,

        ECellHcAppPaneT1Font,
        ECellAppPaneT1Font,
    };

    enum TSwtComposeLayoutRectId
    {
        EUndefinedComposeLayoutId,
        EButtonValueAdjustPaneG1Compose,
        EButtonValueAdjustPaneG2Compose,
        EMainPaneCompose
    };

    // Id's for GetConstant.
    enum TSwtLafFacadeConstantId
    {
        EBrowserFreeRam,
        EBrowserMemoryGood,
        EBrowserCRCommonFlags,
        EBrowserCRCdmaFlags,
        EMIDletSuiteAPNNotSpecified,
        EJavaDefaultAPNKey,
        EJavaAPNRepositoryUid,
        EBrowserConnectionDialogs,
        EBrowserNGPopupBlocking,
        EBrowserFormDataSaving,
        EBrowserMultipleWindows,
        EBrowserGraphicalPage,
        EBrowserGraphicalHistory,
        EBrowserAutoFormFill,
        EBrowserAccessKeys,
        ESwtAknExplicitStartupEffectCompletion,     // CAknAppUiBase::EAknExplicitStartupEffectCompletion
        ESwtBrowserGenericParamURL,                 // TGenericParamIdValue::EGenericParamURL
        ESwtBrowserGenericParamMIMEType,            // TGenericParamIdValue::EGenericParamMIMEType
        ESwtBrowserGenericParamCharSet,             // TGenericParamIdValue::EGenericParamCharSet
        ESwtBrowserGenericParamAccessPoint,         // TGenericParamIdValue::EGenericParamAccessPoint
        ESwtBrowserGenericParamReferringURI,        // TGenericParamIdValue::EGenericParamReferringURI
        ESwtBrowserGenericParamAllowSave,           // TGenericParamIdValue::EGenericParamAllowSave
        ESwtBrowserGenericParamAllowMove            // TGenericParamIdValue::EGenericParamAllowMove
    };

public:
    /**
     * Get layout rectangle for given component id.
     *
     * @param aRectId Id of the component.
     * @param aParentRect Parent rectangle.
     * @param aArg1 Optional argument for the internal function.
     * @param aArg2 Optional argument for the internal function.
     * @param aArg3 Optional argument for the internal function.
     * @return Layout rectangle for the component.
     */
    IMPORT_C static TAknLayoutRect GetLayoutRect(
        TSwtLafFacadeRectId aRectId, const TRect& aParentRect,
        TInt aArg1 = 0, TInt aArg2 = 0, TInt aArg3 = 0);

    /**
     * Get layout rectangle for given component id. This version composes the
     * rectangle and returns the resulting rectangle.
     *
     * @param aRectId Id of the component.
     * @param aParentRect Parent rectangle.
     * @param aArg1 Optional argument for the internal function.
     * @param aArg2 Optional argument for the internal function.
     * @param aArg3 Optional argument for the internal function.
     * @return Layout rectangle for the component.
     */
    IMPORT_C static TAknLayoutRect GetComposeAndLayoutRect(
        TSwtComposeLayoutRectId aRectId, const TRect& aParentRect,
        TInt aArg1 = 0, TInt aArg2 = 0, TInt aArg3 = 0);

    /**
     * Get layout text rectangle for given component id.
     *
     * @param aTextId Id of the component.
     * @param aParentRect Parent rectangle.
     * @param aArg1 Optional argument for the internal function.
     * @return Layout rectangle for the component.
     */
    IMPORT_C static TAknLayoutText GetLayoutText(
        TSwtLafFacadeTextId aTextId, const TRect& aParentRect,
        TInt aArg1 = 0);

    /**
     * Get layout font id for given component id.
     *
     * @param aFontId Id of the component.
     * @param aArg1 Optional argument for the internal function.
     * @param aArg2 Optional argument for the internal function.
     * @return System font id for the component.
     */
    IMPORT_C static TInt GetFontId(TSwtLafFacadeFontId aFontId,
                                   TInt aArg1 = 0, TInt aArg2 = 0);

    /**
     * Get choice list pane layout information.
     *
     * @return Layout information.
     */
    IMPORT_C static TSwtLafFacadeWindowLineLayout GetChoiceListPaneLayout();

    /**
     * Get choice list pane text layout information.
     *
     * @return Layout information.
     */
    IMPORT_C static TSwtLafFacadeTextLineLayout GetChoiceListPaneT1Layout();

    /**
     * Get screen orientation.
     *
     * @return ETrue if landscape, EFalse otherwise.
     */
    IMPORT_C static TBool IsLandscapeOrientation();

    /**
     * Get java  apnid.
     *
     * @return apnid.
     */
    IMPORT_C static TUint32 JavaAPNL(TUint32 aAppId);

    /**
     * Window layout line
     */
    IMPORT_C static TSwtLafFacadeWindowLineLayout WindowLineLayout(
        TSwtLafFacadeRectId aRectId, TInt aVariety = 0);

    /**
     * Text layout line
     */
    IMPORT_C static TSwtLafFacadeTextLineLayout TextLineLayout(
        TSwtLafFacadeTextId aTextId, TInt aVariety = 0);

    /**
     * Get constant for given id.
     *
     * @param aConstantId  eSwt defined id for a specific constant
     * @return contant for given id.
     */
    IMPORT_C static TInt GetConstant(TSwtLafFacadeConstantId aConstantId);

    /**
     * Get TUint constant for given id.
     *
     * @param aConstantId  eSwt defined id for a specific constant
     * @return TUint contant for given id.
     */
    IMPORT_C static TUint GetUintConstant(TSwtLafFacadeConstantId aConstantId);

    /**
     * Gets Enumerator for mobile device flip status.
     */
    IMPORT_C static TInt GetFlipWatchEnumerationStatus();

    /**
     * Gets flip status for Mobile Device.
     */
    IMPORT_C static TUint32 GetFlipWatchStatus();

    /**
     * Gets UID for Flip Watch.
     */
    IMPORT_C static TUid GetFlipWatchUid();

    /**
     * Gets the current or cashe language for TextExtension.
     */
    IMPORT_C static TLanguage GetLanguage();

    /**
     * Initialize feature manager.
     *
     */
    IMPORT_C static void InitFeatureManagerL();

    /**
     * Uninitialize feature manager.
     *
     */
    IMPORT_C static void UnInitFetureManager();

    /**
     * Fetches information whether a certain feature is supported.
     *
     * @param aFeature feature ID
     * @return feature support status
     */
    IMPORT_C static TBool FeatureSupported(TInt aFeature);

    /**
     * Sets inputCapability for password text.
     *
     * @param aCapabilities InputCapability of text
     */
    IMPORT_C static void SetPasswordCapability(TCoeInputCapabilities aInputCapabilities);

    /**
     * Signals the transition effects engine for the second phase of the
     * full screen transition.
     */
    IMPORT_C static void SignalFullScreenTransitionReady();

private:
    // Private constructors to prevent the facade class from being instantiated
    CSwtLafFacade();
    CSwtLafFacade(const CSwtLafFacade&);
    ~CSwtLafFacade();
};

#endif // SWTLAFFACADE_H
