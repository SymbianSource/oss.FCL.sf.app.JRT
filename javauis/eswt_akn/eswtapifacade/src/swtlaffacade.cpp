/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/



#include <aknbutton.h>
#include <avkon.mbg>
#include <aknlayoutscalable_avkon.cdl.h>
#include <skinlayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <centralrepository.h>
#include <gfxtranseffect/gfxtranseffect.h>
#include <hwrmdomainpskeys.h>
#include <oommonitorplugin.h>
#include <AknSettingCache.h>
#include <featmgr.h>
#include <aknappui.h>
#include <AiwGenericParam.h>
#include <aknextendedinputcapabilities.h>
#include "swtlaffacade.h"




// ---------------------------------------------------------------------------
// CSwtLafFacade::GetLayoutRect
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutRect CSwtLafFacade::GetLayoutRect(
    TSwtLafFacadeRectId aRectId, const TRect& aRect,
    TInt aArg1, TInt aArg2, TInt aArg3)
{
    TAknLayoutRect layoutRect;
    TAknTextComponentLayout compose;

    switch (aRectId)
    {
    case EMainEswtPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              main_eswt_pane(aArg1).LayoutLine());
        break;
    case EControlButtonPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              control_button_pane_g2(aArg1).LayoutLine());
        break;
    case EToolbarButtonPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              toolbar_button_pane_g2().LayoutLine());
        break;
    case EToolbarButtonPaneG5:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              toolbar_button_pane_g5().LayoutLine());
        break;
    case EInputFieldSkinPlacingGeneralLine2:
        layoutRect.LayoutRect(aRect, SkinLayout::
                              Input_field_skin_placing__general__Line_2());
        break;
    case EInputFieldSkinPlacingGeneralLine5:
        layoutRect.LayoutRect(aRect, SkinLayout::
                              Input_field_skin_placing__general__Line_5());
        break;
    case EEswtControlPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              eswt_control_pane_g1(aArg1));
        break;
    case EEswtControlPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              eswt_control_pane_g2(aArg1));
        break;
    case EEswtControlPaneG3:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              eswt_control_pane_g3(aArg1));
        break;
    case EEswtControlPaneG4:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              eswt_control_pane_g4(aArg1));
        break;
    case EPopupWindowsSkinPlacingFrameGeneralLine2:
        layoutRect.LayoutRect(aRect, SkinLayout::
                              Popup_windows_skin_placing__frame_general__Line_2());
        break;
    case EPopupWindowsSkinPlacingFrameGeneralLine5:
        layoutRect.LayoutRect(aRect, SkinLayout::
                              Popup_windows_skin_placing__frame_general__Line_5());
        break;
    case EForm2MidpLabelPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              form2_midp_label_pane(0));
        break;
    case ESeparatorVerticalPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_vertical_pane());
        break;
    case ESeparatorVerticalPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_vertical_pane_g1());
        break;
    case ESeparatorVerticalPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_vertical_pane_g2());
        break;
    case ESeparatorVerticalPaneG3:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_vertical_pane_g3());
        break;
    case ESeparatorHorizontalPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_horizontal_pane());
        break;
    case ESeparatorHorizontalPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_horizontal_pane_g1());
        break;
    case ESeparatorHorizontalPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_horizontal_pane_g2());
        break;
    case ESeparatorHorizontalPaneG3:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              separator_horizontal_pane_g3());
        break;
    case EListScrollGenPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              listscroll_gen_pane(aArg1).LayoutLine());
        break;
    case EListGenPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_gen_pane(aArg1).LayoutLine());
        break;
    case EListSinglePane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_pane(aArg1).LayoutLine());
        break;
    case EListSinglePaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_pane_g1(aArg1).LayoutLine());
        break;
    case EListSinglePaneG1Cp2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_pane_g1_cp2(aArg1).LayoutLine());
        break;
    case EListSingleNumberPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_number_pane(aArg1).LayoutLine());
        break;
    case EListSingleNumberPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_number_pane_g1(aArg1).LayoutLine());
        break;
    case EListSingleGraphicH1Pane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_hl_pane(aArg1).LayoutLine());
        break;
    case EListSingleGraphicPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_pane(aArg1).LayoutLine());
        break;
    case EListSingleGraphicPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_pane_g1(aArg1).LayoutLine());
        break;
    case EListSingleGraphicPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_pane_g2(aArg1).LayoutLine());
        break;
    case EListSingleLargeGraphicPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_large_graphic_pane(aArg1, aArg2).LayoutLine());
        break;
    case EListSingleLargeGraphicPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_large_graphic_pane_g1(aArg1).LayoutLine());
        break;
    case EListSingle2GraphicPaneG1Cp4:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_2graphic_pane_g1_cp4().LayoutLine());
        break;
    case EListSingleLargeGraphicPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_large_graphic_pane_g2(aArg1).LayoutLine());
        break;
    case EListSingleHeadingPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_heading_pane(aArg1).LayoutLine());
        break;
    case EListSingleHeadingPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_heading_pane_g1(aArg1).LayoutLine());
        break;
    case EListSingle2HeadingMsgPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_2heading_msg_pane(aArg1).LayoutLine());
        break;
    case EListSingle2HeadingMsgPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_2heading_msg_pane_g1(aArg1).LayoutLine());
        break;
    case EListSingle2HeadingMsgPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_2heading_msg_pane_g2(aArg1).LayoutLine());
        break;
    case EListSingleGraphicHeadingPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_heading_pane(aArg1).LayoutLine());
        break;
    case EListSingleGraphicHeadingPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_heading_pane_g1(aArg1).LayoutLine());
        break;
    case EListSingleGraphicHeadingPaneG4:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_heading_pane_g4(aArg1).LayoutLine());
        break;
    case EListSingleNumberHeadingPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_number_heading_pane(aArg1).LayoutLine());
        break;
    case EListSingleNumberHeadingPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_number_heading_pane_g1(aArg1).LayoutLine());
        break;
    case EListDoublePane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_pane(aArg1).LayoutLine());
        break;
    case EListDoublePaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_pane_g1(aArg1).LayoutLine());
        break;
    case EListDoubleNumberPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_number_pane(aArg1).LayoutLine());
        break;
    case EListDoubleNumberPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_number_pane_g1(aArg1).LayoutLine());
        break;
    case EListDoubleGraphicPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_graphic_pane(aArg1).LayoutLine());
        break;
    case EListDoubleGraphicPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_graphic_pane_g1(aArg1).LayoutLine());
        break;
    case EListDoubleGraphicPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_graphic_pane_g2(aArg1).LayoutLine());
        break;
    case EListDoubleLargeGraphicPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_large_graphic_pane(aArg1).LayoutLine());
        break;
    case EListDoubleLargeGraphicPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_large_graphic_pane_g1(aArg1).LayoutLine());
        break;
    case EListDoubleLargeGraphicPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_double_large_graphic_pane_g2(aArg1).LayoutLine());
        break;
    case EESwtCtrlWaitPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              eswt_ctrl_wait_pane().LayoutLine());
        break;
    case EESwtCtrlWaitPaneH:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              wait_bar_pane_cp1(aArg1).LayoutLine());
        break;
    case EWaitBorderPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              wait_border_pane_g1().LayoutLine());
        break;
    case EWaitBorderPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              wait_border_pane_g2().LayoutLine());
        break;
    case EWaitBorderPaneG3:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              wait_border_pane_g3().LayoutLine());
        break;
    case ECellEswtAppPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_eswt_app_pane(aArg1, aArg2, aArg3).LayoutLine());
        break;
    case ECellEswtAppPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_eswt_app_pane_g1(aArg1).LayoutLine());
        break;
    case ECellEswtAppPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_eswt_app_pane_g2(aArg1).LayoutLine());
        break;
    case EListHighlightSkinPlacingGeneralLine2:
        layoutRect.LayoutRect(aRect, SkinLayout::
                              List_highlight_skin_placing__general__Line_2());
        break;
    case EListHighlightSkinPlacingGeneralLine5:
        layoutRect.LayoutRect(aRect, SkinLayout::
                              List_highlight_skin_placing__general__Line_5());
        break;
    case EButtonValueAdjustPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              button_value_adjust_pane(aArg1));
        break;
    case EButtonValueAdjustPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              button_value_adjust_pane_g1());
        break;
    case EButtonValueAdjustPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              button_value_adjust_pane_g2());
        break;
    case ESliderSetPaneCP3:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              slider_set_pane_cp3());
        break;
    case ESliderSetPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              slider_set_pane_g1());
        break;
    case ESliderSetPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              slider_set_pane_g2());
        break;
    case EListSettingNumberPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_setting_number_pane(aArg1));
        break;
    case ESetValuePaneVC:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              set_value_pane_vc(aArg1));
        break;
    case ESetValuePane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              set_value_pane(aArg1));
        break;
    case EBgSetOptPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              bg_set_opt_pane(aArg1));
        break;
    case EBgSetOptPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              bg_set_opt_pane_g2());
        break;
    case EBgSetOptPaneG5:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              bg_set_opt_pane_g5());
        break;
    case EChildShellWithTitlePane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              bg_popup_window_pane_cp18(2));
        break;
    case EChildShellWithoutTitlePane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              bg_popup_window_pane_cp18(1));
        break;
    case EMenuItemPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_pane_cp2(0).LayoutLine());
        break;
    case ESubMenuItemPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              list_single_popup_submenu_pane(0).LayoutLine());
        break;

    case EPopupEswtTasktipWindow:
#ifdef RD_JAVA_S60_RELEASE_9_2
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              popup_discreet_window(aArg1).LayoutLine());
#else
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              popup_eswt_tasktip_window(aArg1).LayoutLine());
#endif

        break;
    case EWaitBarPaneCp71:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              wait_bar_pane_cp71(aArg1).LayoutLine());
        break;
    case EFindPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::find_pane());
        break;
    case EChildShellTitleImagePane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::heading_pane_g2());
        break;
    case EPopupMidpNoteAlarmWindowG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              popup_midp_note_alarm_window_g1(0));
        break;
    case EPopupWindowGeneral:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              popup_query_data_window(aArg1));
        break;
    case EHeadingPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              heading_pane(aArg1));
        break;
    case ECellHcAppPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_hc_apps_pane(aArg1, aArg2, aArg3).LayoutLine());
        break;
    case ECellHcAppPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_hc_apps_pane_g1(aArg1).LayoutLine());
        break;
    case ECellHcAppPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_hc_apps_pane_g2(aArg1).LayoutLine());
        break;
    case ECellAppPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_app_pane(aArg1, aArg2, aArg3).LayoutLine());
        break;
    case ECellAppPaneG1:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_app_pane_g1(aArg1).LayoutLine());
        break;
    case ECellAppPaneG2:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              cell_app_pane_g2(aArg1).LayoutLine());
        break;
    case EAreaBottomPane:
        layoutRect.LayoutRect(aRect, AknLayoutScalable_Avkon::
                              area_bottom_pane(aArg1));
        break;
    case EUndefinedRectId:
        // Intentional fall through.
    default:
        ASSERT(EFalse);
        break;
    }

    return layoutRect;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetLayoutRect
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutRect CSwtLafFacade::GetComposeAndLayoutRect(
    TSwtComposeLayoutRectId aRectId, const TRect& aParentRect, TInt aArg1,
    TInt /*aArg2*/, TInt /*aArg3*/)
{
    TAknWindowComponentLayout spaceLayout;
    TAknWindowLineLayout      lineLayout;
    TAknLayoutRect            layoutRect;

    switch (aRectId)
    {
    case EButtonValueAdjustPaneG1Compose:
        spaceLayout = AknLayoutScalable_Avkon::button_value_adjust_pane(aArg1);
        lineLayout = TAknWindowComponentLayout::Compose(
                         spaceLayout,
                         AknLayoutScalable_Avkon::button_value_adjust_pane_g1()).LayoutLine();
        layoutRect.LayoutRect(aParentRect, lineLayout);
        break;
    case EButtonValueAdjustPaneG2Compose:
        spaceLayout = AknLayoutScalable_Avkon::button_value_adjust_pane(aArg1);
        lineLayout = TAknWindowComponentLayout::Compose(
                         spaceLayout,
                         AknLayoutScalable_Avkon::button_value_adjust_pane_g2()).LayoutLine();
        layoutRect.LayoutRect(aParentRect, lineLayout);
        break;
    case EMainPaneCompose:
        layoutRect.LayoutRect(aParentRect, TAknWindowComponentLayout::Compose(
                                  AknLayoutScalable_Avkon::application_window(0),
                                  AknLayoutScalable_Avkon::main_pane(aArg1)));
        break;
    case EUndefinedComposeLayoutId:
        // Intentional flow through.
    default:
        ASSERT(EFalse);
        break;
    }

    return layoutRect;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetLayoutText
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutText CSwtLafFacade::GetLayoutText(
    TSwtLafFacadeTextId aTextId,
    const TRect& aRect, TInt aArg1)
{
    TAknLayoutText layoutText;

    switch (aTextId)
    {
    case EListSinglePaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_pane_t1(aArg1).LayoutLine());
        break;
    case EListSinglePaneT1Cp2:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_pane_t1_cp2(aArg1).LayoutLine());
        break;
    case EListSingleNumberPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_number_pane_t1(aArg1).LayoutLine());
        break;
    case EListSingleGraphicPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_pane_t1(aArg1).LayoutLine());
        break;
    case EListSingleLargeGraphicPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_large_graphic_pane_t1(aArg1).LayoutLine());
        break;
    case EListSingleHeadingPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_heading_pane_t1(aArg1).LayoutLine());
        break;
    case EListSingle2HeadingMsgPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_2heading_msg_pane_t1(aArg1).LayoutLine());
        break;
    case EListSingleNumberHeadingPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_number_heading_pane_t1(aArg1).LayoutLine());
        break;
    case EListSingleGraphicHeadingPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_graphic_heading_pane_t1(aArg1).LayoutLine());
        break;
    case EListSinglePopupSubmenuPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_single_popup_submenu_pane_t1(aArg1).LayoutLine());
        break;
    case EListDoublePaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_pane_t1(aArg1).LayoutLine());
        break;
    case EListDoublePaneT2:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_pane_t2(aArg1).LayoutLine());
        break;
    case EListDoubleNumberPaneT2:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_number_pane_t2(aArg1).LayoutLine());
        break;
    case EListDoubleNumberPaneT3:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_number_pane_t3(aArg1).LayoutLine());
        break;
    case EListDoubleGraphicPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_graphic_pane_t1(aArg1).LayoutLine());
        break;
    case EListDoubleGraphicPaneT2:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_graphic_pane_t2(aArg1).LayoutLine());
        break;
    case EListDoubleLargeGraphicPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_large_graphic_pane_t1(aArg1).LayoutLine());
        break;
    case EListDoubleLargeGraphicPaneT2:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              list_double_large_graphic_pane_t2(aArg1).LayoutLine());
        break;
    case ECellEswtAppPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              cell_eswt_app_pane_t1(aArg1).LayoutLine());
        break;
    case EFormDataWideGraphicFieldTextsLine2:
        layoutText.LayoutText(aRect, AknLayout::
                              Form_data_wide_graphic_field_texts_Line_2(aArg1));
        break;
    case ETabs4ActivePaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              tabs_4_active_pane_t1(aArg1).LayoutLine());
        break;
    case ECellHcAppPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              cell_hc_apps_pane_t1(aArg1).LayoutLine());
        break;
    case ECellAppPaneT1:
        layoutText.LayoutText(aRect, AknLayoutScalable_Avkon::
                              cell_app_pane_t1(aArg1).LayoutLine());
        break;
    case EUndefinedTextId:
        // Intentional flow through.
    default:
        ASSERT(EFalse);
        break;
    }

    return layoutText;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetFontId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtLafFacade::GetFontId(TSwtLafFacadeFontId aFontId,
                                       TInt aArg1, TInt aArg2)
{
    TInt fontId = EUndefinedFontId;

    switch (aFontId)
    {
    case EForm2MidpLabelPaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 form2_midp_label_pane_t1(aArg1).Font();
        break;
    case EFont2MidpTextPaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 form2_mdip_text_pane_t1(aArg1, aArg2).Font();
        break;

    case EFont2MidpTimePaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 form2_midp_time_pane_t1().Font();
        break;

    case ECellEswtAppPaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 cell_eswt_app_pane_t1(aArg1).Font();
        break;
    case EListSingleGraphicPaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 list_single_graphic_pane_t1(aArg1).Font();
        break;
    case ECellHcAppPaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 cell_hc_apps_pane_t1(aArg1).Font();
        break;
    case ECellAppPaneT1Font:
        fontId = AknLayoutScalable_Avkon::
                 cell_app_pane_t1(aArg1).Font();
        break;
    case EUndefinedFontId:
        // Intentional flow through.
    default:
        ASSERT(EFalse);
        break;
    }

    return fontId;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetChoiceListPaneLayout
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtLafFacade::TSwtLafFacadeWindowLineLayout
CSwtLafFacade::GetChoiceListPaneLayout()
{
    TAknWindowLineLayout layoutInt =
        AknLayoutScalable_Avkon::choice_list_pane().LayoutLine();

    CSwtLafFacade::TSwtLafFacadeWindowLineLayout layout;
    layout.iC = layoutInt.iC;
    layout.il = layoutInt.il;
    layout.it = layoutInt.it;
    layout.ir = layoutInt.ir;
    layout.ib = layoutInt.ib;
    layout.iW = layoutInt.iW;
    layout.iH = layoutInt.iH;
    return layout;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetChoiceListPaneT1Layout
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtLafFacade::TSwtLafFacadeTextLineLayout
CSwtLafFacade::GetChoiceListPaneT1Layout()
{
    TAknTextLineLayout layoutInt =
        AknLayoutScalable_Avkon::choice_list_pane_t1().LayoutLine();

    CSwtLafFacade::TSwtLafFacadeTextLineLayout layout;
    layout.iC = layoutInt.iC;
    layout.il = layoutInt.il;
    layout.ir = layoutInt.ir;
    layout.iB = layoutInt.iB;
    layout.iW = layoutInt.iW;
    layout.iJ = layoutInt.iJ;
    layout.iFont = layoutInt.iFont;
    return layout;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::IsLandscapeOrientation
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtLafFacade::IsLandscapeOrientation()
{
    return Layout_Meta_Data::IsLandscapeOrientation();
}


EXPORT_C TUint32 CSwtLafFacade::JavaAPNL(TUint32 aAppId)
{
    TUid uid;
    uid.iUid = GetUintConstant(EJavaAPNRepositoryUid);
    CRepository* rep = CRepository::NewLC(uid) ;
    TInt tmpApnId;
    TUint32 retApnId;
    TInt err = rep->Get(aAppId, tmpApnId);
    retApnId = tmpApnId;
    CleanupStack::PopAndDestroy(rep);

    if (err != KErrNone)
    {
        if (err == KErrNotFound)
        {
            //return value - use the system default
            retApnId = GetUintConstant(EMIDletSuiteAPNNotSpecified);
        }
        else
        {
            //error
            User::Leave(err);
        }
    }
    return retApnId;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetChoiceListPaneLayout
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtLafFacade::TSwtLafFacadeWindowLineLayout
CSwtLafFacade::WindowLineLayout(
    TSwtLafFacadeRectId aRectId,
    TInt aVariety /* = 0 */)
{
    TAknWindowLineLayout layoutLine;
    switch (aRectId)
    {
    case EListSinglePaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_pane_g1(aVariety).LayoutLine();
        break;
    case EListSingleGraphicPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_graphic_pane_g1(aVariety).LayoutLine();
        break;
    case EListSingleGraphicPaneG2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_graphic_pane_g2(aVariety).LayoutLine();
        break;
    case EListSingleLargeGraphicPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_large_graphic_pane_g1(aVariety).LayoutLine();
        break;
    case EListSingleLargeGraphicPaneG2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_large_graphic_pane_g2(aVariety).LayoutLine();
        break;
    case EListSingleNumberPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_number_pane_g1(aVariety).LayoutLine();
        break;
    case EListSingleHeadingPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_heading_pane_g1(aVariety).LayoutLine();
        break;
    case EListSingleGraphicHeadingPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_graphic_heading_pane_g1(aVariety).LayoutLine();
        break;
    case EListSingleGraphicHeadingPaneG4:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_graphic_heading_pane_g4(aVariety).LayoutLine();
        break;
    case EListSingleNumberHeadingPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_number_heading_pane_g1(aVariety).LayoutLine();
        break;
    case EListDoublePaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_pane_g1(aVariety).LayoutLine();
        break;
    case EListDoubleNumberPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_number_pane_g1(aVariety).LayoutLine();
        break;
    case EListDoubleGraphicPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_graphic_pane_g1(aVariety).LayoutLine();
        break;
    case EListDoubleGraphicPaneG2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_graphic_pane_g2(aVariety).LayoutLine();
        break;
    case EListDoubleLargeGraphicPaneG1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_large_graphic_pane_g1(aVariety).LayoutLine();
        break;
    case EListDoubleLargeGraphicPaneG2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_large_graphic_pane_g2(aVariety).LayoutLine();
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    CSwtLafFacade::TSwtLafFacadeWindowLineLayout layout;
    layout.iC = layoutLine.iC;
    layout.il = layoutLine.il;
    layout.it = layoutLine.it;
    layout.ir = layoutLine.ir;
    layout.ib = layoutLine.ib;
    layout.iW = layoutLine.iW;
    layout.iH = layoutLine.iH;
    return layout;
}

// ---------------------------------------------------------------------------
// CSwtLafFacade::GetChoiceListPaneT1Layout
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtLafFacade::TSwtLafFacadeTextLineLayout
CSwtLafFacade::TextLineLayout(
    TSwtLafFacadeTextId aTextId,
    TInt aVariety /* = 0 */)
{
    TAknTextLineLayout layoutLine;
    switch (aTextId)
    {
    case EListSinglePaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingleNumberPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_number_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingleGraphicPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_graphic_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingleLargeGraphicPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_large_graphic_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingleHeadingPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_heading_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingle2HeadingMsgPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_2heading_msg_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingleNumberHeadingPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_number_heading_pane_t1(aVariety).LayoutLine();
        break;
    case EListSingleGraphicHeadingPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_single_graphic_heading_pane_t1(aVariety).LayoutLine();
        break;
    case EListDoublePaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_pane_t1(aVariety).LayoutLine();
        break;
    case EListDoublePaneT2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_pane_t2(aVariety).LayoutLine();
        break;
    case EListDoubleNumberPaneT2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_number_pane_t2(aVariety).LayoutLine();
        break;
    case EListDoubleNumberPaneT3:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_number_pane_t3(aVariety).LayoutLine();
        break;
    case EListDoubleGraphicPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_graphic_pane_t1(aVariety).LayoutLine();
        break;
    case EListDoubleGraphicPaneT2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_graphic_pane_t2(aVariety).LayoutLine();
        break;
    case EListDoubleLargeGraphicPaneT1:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_large_graphic_pane_t1(aVariety).LayoutLine();
        break;
    case EListDoubleLargeGraphicPaneT2:
        layoutLine = AknLayoutScalable_Avkon::
                     list_double_large_graphic_pane_t2(aVariety).LayoutLine();
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    CSwtLafFacade::TSwtLafFacadeTextLineLayout layout;
    layout.iC = layoutLine.iC;
    layout.il = layoutLine.il;
    layout.ir = layoutLine.ir;
    layout.iB = layoutLine.iB;
    layout.iW = layoutLine.iW;
    layout.iJ = layoutLine.iJ;
    layout.iFont = layoutLine.iFont;
    return layout;
}

EXPORT_C TInt CSwtLafFacade::GetConstant(TSwtLafFacadeConstantId aConstantId)
{
    TInt constant(0);
    switch (aConstantId)
    {
    case EBrowserFreeRam:
        constant = KAppOomMonitor_FreeRam;
        break;
    case EBrowserMemoryGood:
        constant = KAppOomMonitor_MemoryGood;
        break;
    case EBrowserMultipleWindows:
        constant = 0x01;
        break;
    case EBrowserGraphicalPage:
        constant = 0x20;
        break;
    case EBrowserAutoFormFill:
        constant = 0x40;
        break;
    case EBrowserGraphicalHistory:
        constant = 0x80;
        break;
    case EBrowserAccessKeys:
        constant = 0x100;
        break;
    case ESwtBrowserGenericParamURL:
        constant = EGenericParamURL;
        break;
    case ESwtBrowserGenericParamMIMEType:
        constant = EGenericParamMIMEType;
        break;
    case ESwtBrowserGenericParamCharSet:
        constant = EGenericParamCharSet;
        break;
    case ESwtBrowserGenericParamAccessPoint:
        constant = EGenericParamAccessPoint;
        break;
    case ESwtBrowserGenericParamReferringURI:
        constant = EGenericParamReferringURI;
        break;
    case ESwtBrowserGenericParamAllowSave:
        constant = EGenericParamAllowSave;
        break;
    case ESwtBrowserGenericParamAllowMove:
        constant = EGenericParamAllowMove;
        break;
    default:
        ASSERT(EFalse);
        break;
    }
    return constant;
}

EXPORT_C TUint CSwtLafFacade::GetUintConstant(TSwtLafFacadeConstantId aConstantId)
{
    TInt constant(0);
    switch (aConstantId)
    {
    case EBrowserCRCommonFlags:
        constant = 0x00000002; // from browser NG
        break;
    case EBrowserCRCdmaFlags:
        constant = 0x00000003; // from browser NG
        break;
    case EMIDletSuiteAPNNotSpecified:
        // APN Id return value if no APN is specified for the given suite
        constant = KMaxTUint32;
        break;
    case EJavaDefaultAPNKey:
        //CenRep key for Java default APN key-value pair
        constant = KMaxTUint32;
        break;
    case EJavaAPNRepositoryUid:
        // APN table CenRep UID
        constant = 0x10206EC5;
        break;
    case EBrowserConnectionDialogs:
        constant = 0x00000006; // from browser NG
        break;
    case EBrowserNGPopupBlocking:
        constant = 0x0000002F; // from browser NG
        break;
    case EBrowserFormDataSaving:
        constant = 0x00000022; // from browser NG
        break;
    case ESwtAknExplicitStartupEffectCompletion:
        constant = CAknAppUi::EAknExplicitStartupEffectCompletion;
        break;

    default:
        ASSERT(EFalse);
        break;
    }
    return constant;

}

EXPORT_C TInt CSwtLafFacade::GetFlipWatchEnumerationStatus()
{
    return EPSHWRMFlipClosed;
}

EXPORT_C TUint32 CSwtLafFacade::GetFlipWatchStatus()
{
    return KHWRMFlipStatus;
}

EXPORT_C TUid CSwtLafFacade::GetFlipWatchUid()
{
    return KPSUidHWRM;
}

EXPORT_C TLanguage CSwtLafFacade::GetLanguage()
{
    return CAknEnv::Static()->SettingCache().InputLanguage();
}

EXPORT_C void CSwtLafFacade::InitFeatureManagerL()
{
    FeatureManager::InitializeLibL();
}

EXPORT_C void CSwtLafFacade::UnInitFetureManager()
{
    FeatureManager::UnInitializeLib();
}

EXPORT_C TBool CSwtLafFacade::FeatureSupported(TInt aFeature)
{
    return FeatureManager::FeatureSupported(aFeature);
}

EXPORT_C void CSwtLafFacade::SetPasswordCapability(TCoeInputCapabilities aInputCapabilities)
{
    MObjectProvider* mop = aInputCapabilities.ObjectProvider();

    if (mop)
    {
        CAknExtendedInputCapabilities* extendedInputCapabilities = NULL;
        extendedInputCapabilities = mop->MopGetObject(
                                        extendedInputCapabilities);
        if (extendedInputCapabilities)
        {
            extendedInputCapabilities->SetMIDPConstraints(0x10000);
        }
    }
}

EXPORT_C void CSwtLafFacade::SignalFullScreenTransitionReady()
{
    GfxTransEffect::EndFullScreen();
}
