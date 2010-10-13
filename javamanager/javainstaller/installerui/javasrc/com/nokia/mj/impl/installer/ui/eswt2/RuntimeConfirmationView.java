/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.ui.eswt2;

import com.nokia.mj.impl.rt.ui.ConfirmData;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * Displays error message.
 */
public class RuntimeConfirmationView extends ConfirmationViewBase
{
    private String iAppName = null;
    private ConfirmData iConfirmData = null;
    private Button[] iAnswerButtons = null;

    /** Constructor */
    protected RuntimeConfirmationView()
    {
        super();
    }

    /** Constructor */
    protected RuntimeConfirmationView(InstallerUiEswt aInstallerUi,
                                      Composite aParent, String aAppName,
                                      ConfirmData aConfirmData)
    {
        super(aInstallerUi, aParent, 8);
        iAppName = aAppName;
        iConfirmData = aConfirmData;
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALL_FAILED));
        String[] answerOptions = iConfirmData.getAnswerOptions();
        switch (answerOptions.length)
        {
        case 1: setCommands(answerOptions[0], null); break;
        case 2: setCommands(answerOptions[0], answerOptions[1]); break;
        default: setCommands(null, null); break;
        }
    }

    /**
     * This method is called once before view is opened.
     */
    protected void createView()
    {
        // Add title.
        if (iInstallerUi.getInstallInfo() != null)
        {
            Label titleLabel = createLabel(iAppName, getColumns() - 1, SWT.WRAP);
            titleLabel.setFont(iInstallerUi.getBoldFont());
            // Add security icon.
            iCertificates = iInstallerUi.getInstallInfo().getCertificates();
            createSecurityButton();
        }
        else
        {
            Label titleLabel = createLabel(iAppName, getColumns(), SWT.WRAP);
            titleLabel.setFont(iInstallerUi.getBoldFont());
        }

        GridData gridData = null;
        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Begin widgets creation.

        // Add question label.
        Label errorLabel = createLabel(
            iConfirmData.getQuestion(), horizontalSpan, labelStyle);


        // Add user answer buttons.
        String[] answerOptions = iConfirmData.getAnswerOptions();
        if (answerOptions.length > 2)
        {
            iAnswerButtons = new Button[answerOptions.length];
            for (int i = 0; i < iAnswerButtons.length; i++)
            {
                final int answerIndex = i;
                iAnswerButtons[i] = new Button(getComposite(), SWT.PUSH);
                iAnswerButtons[i].setText(answerOptions[i]);
                gridData = new GridData(GridData.FILL_HORIZONTAL);
                gridData.horizontalSpan = horizontalSpan;
                iAnswerButtons[i].setLayoutData(gridData);
                iAnswerButtons[i].addDisposeListener(new DisposeListener ()
                {
                    public void widgetDisposed(DisposeEvent aEvent)
                    {
                        answer(answerIndex);
                    }
                });
                iAnswerButtons[i].addSelectionListener(new SelectionListener ()
                {
                    public void widgetDefaultSelected(SelectionEvent aEvent)
                    {
                        widgetSelected(aEvent);
                    }
                    public void widgetSelected(SelectionEvent aEvent)
                    {
                        answer(answerIndex);
                    }
                });
            }
            // Set the default answer option.
            iParent.setDefaultButton(
                iAnswerButtons[iConfirmData.getAnswerSuggestion()]);
            iAnswerButtons[iConfirmData.getAnswerSuggestion()].setFocus();
        }

        // End of widgets creation.
    }

    /**
     * This method is called when user answers to the dialog.
     */
    protected void answer(int aAnswerIndex)
    {
        iConfirmData.setAnswer(aAnswerIndex);
        super.confirmOk();
    }

    /**
     * This method is called when user accepts the dialog.
     */
    protected void confirmOk()
    {
        iConfirmData.setAnswer(0);
        super.confirmOk();
    }

    /**
     * This method is called when user cancels the dialog.
     */
    protected void confirmCancel()
    {
        iConfirmData.setAnswer(1);
        super.confirmCancel();
    }

    /**
     * This method is called after user has answered
     * to confirmation.
     */
    protected void getDataFromView()
    {
        // nop
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }
}
