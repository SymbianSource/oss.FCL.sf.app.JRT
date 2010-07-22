/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.UserPermission;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import com.nokia.mj.impl.security.utils.Logger;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.utils.exception.UserCancelException;
import java.util.Hashtable;
import java.util.Vector;

/**
 * Offers and handles runtime security user prompts. It maintains the logic
 * about when to prompt according to MIDlet suite's security settings and
 * MIDlet suite's states
 */
public class SecurityPromptHandler
{
    // handler to runtime UI used to display the runtime prompts
    private RuntimeUi ui;

    // handler to localized security prompt messages
    private SecurityPromptMessage securityPromptMessages;

    // name of the running application
    private String appName;

    // keeps track of the prompts for session mode for
    // different function groups
    private Hashtable sessionPromptsShown;

    // keeps track of the denied answers for session mode for
    // different function groups
    private Hashtable sessionsDenied;

    // keeps track of the prompt texts shown, so that within the same session
    // it does not show same promtp text twice
    private Vector promptTexts;

    // the mode in which the security prompts operate
    // GeneralSecuritySettings.DEFAULT_SECURITY_MODE or
    // GeneralSecuritySettings.USER_SECURITY_MODE
    private int iSecurityPromptsMode = GeneralSecuritySettings.DEFAULT_SECURITY_MODE;

    /**
     * Constructor
     */
    public SecurityPromptHandler(String appName, int securityPromptsMode)
    {
        securityPromptMessages = SecurityPromptMessage.getInstance();
        sessionPromptsShown = new Hashtable();
        sessionsDenied = new Hashtable();
        promptTexts = new Vector();
        this.appName = appName;
        switch (securityPromptsMode)
        {
        case GeneralSecuritySettings.DEFAULT_SECURITY_MODE:
        case GeneralSecuritySettings.USER_SECURITY_MODE:
            iSecurityPromptsMode = securityPromptsMode;
        }
    }

    public synchronized UserSecuritySettings handleUserPermission(
        UserPermission userPermission, boolean blanketPromptShown)
    {
        UserSecuritySettings settings = userPermission
                                        .getUserSecuritySettings();
        Boolean tmp = (Boolean)sessionsDenied.get(settings.getName());
        boolean sessionDenied = (tmp == null ? false : tmp.booleanValue());
        if (sessionDenied)
        {
            // return immediately with a NO_INTERACTION_MODE
            return getDenyAnswer(settings);
        }
        tmp = (Boolean)sessionPromptsShown.get(settings.getName());
        boolean sessionPromptShown = (tmp == null ? false : tmp.booleanValue());
        int currentInteractionMode = settings.getCurrentInteractionMode();
        if (currentInteractionMode
                == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                || (currentInteractionMode
                    == UserSecuritySettings.SESSION_INTERACTION_MODE
                    && !sessionPromptShown)
                || (currentInteractionMode
                    == UserSecuritySettings.BLANKET_INTERACTION_MODE
                    && !blanketPromptShown))
        {
            // figure out the texts for the current & most powerful interaction modes
            String currentInteractionModeText = null;
            String mostPowerfulInteractionModeText = null;
            int mostPowerfulInteractionMode = currentInteractionMode;
            switch (currentInteractionMode)
            {
            case UserSecuritySettings.ONESHOT_INTERACTION_MODE:
                currentInteractionModeText = userPermission.getPromptDetails()
                                             .getSecurityPromptAnswerOption(
                                                 PermissionBase.ONESHOT_ANSWER_OPTION);
                if (settings.isInteractionModeAllowed(UserSecuritySettings
                                                      .BLANKET_INTERACTION_MODE))
                {
                    mostPowerfulInteractionModeText = userPermission.getPromptDetails()
                                                      .getSecurityPromptAnswerOption(
                                                          PermissionBase.BLANKET_ANSWER_OPTION);
                    mostPowerfulInteractionMode = UserSecuritySettings
                                                  .BLANKET_INTERACTION_MODE;
                }
                else if (settings.isInteractionModeAllowed(UserSecuritySettings
                         .SESSION_INTERACTION_MODE))
                {
                    mostPowerfulInteractionModeText = userPermission.getPromptDetails()
                                                      .getSecurityPromptAnswerOption(
                                                          PermissionBase.SESSION_ANSWER_OPTION);
                    mostPowerfulInteractionMode = UserSecuritySettings
                                                  .SESSION_INTERACTION_MODE;
                }
                break;
            case UserSecuritySettings.SESSION_INTERACTION_MODE:
                currentInteractionModeText = userPermission.getPromptDetails()
                                             .getSecurityPromptAnswerOption(
                                                 PermissionBase.SESSION_ANSWER_OPTION);
                if (settings.isInteractionModeAllowed(UserSecuritySettings
                                                      .BLANKET_INTERACTION_MODE))
                {
                    mostPowerfulInteractionModeText = userPermission.getPromptDetails()
                                                      .getSecurityPromptAnswerOption(
                                                          PermissionBase.BLANKET_ANSWER_OPTION);
                    mostPowerfulInteractionMode = UserSecuritySettings
                                                  .BLANKET_INTERACTION_MODE;
                }
                break;
            case UserSecuritySettings.BLANKET_INTERACTION_MODE:
                currentInteractionModeText = userPermission.getPromptDetails()
                                             .getSecurityPromptAnswerOption(
                                                 PermissionBase.BLANKET_ANSWER_OPTION);
                break;
            }
            String promptText = null;
            switch (iSecurityPromptsMode)
            {
            case GeneralSecuritySettings.DEFAULT_SECURITY_MODE:
                if (mostPowerfulInteractionModeText != null)
                {
                    currentInteractionModeText = null;
                }
                promptText = userPermission.getPromptDetails()
                             .getSecurityPromptQuestion(mostPowerfulInteractionMode);
                break;
            case GeneralSecuritySettings.USER_SECURITY_MODE:
                mostPowerfulInteractionModeText = null;
                promptText = userPermission.getPromptDetails()
                             .getSecurityPromptQuestion(currentInteractionMode);
                break;
            }
            if (promptText == null)
            {
                // no text to show -> hande it as deny
                Logger.logWarning("User permission was denied by platform since it did not provide any security prompt question " + userPermission);
                return handleDenyAnswer(settings);
            }
            // check if the two texts are equal
            if ((mostPowerfulInteractionModeText != null
                    && mostPowerfulInteractionModeText.equalsIgnoreCase(
                        currentInteractionModeText)))
            {
                // no sense in showing the text for the current interaction mode,
                // since it is identical with the one for the most powerful one
                // (in this way we promote the most powerful interaction mode)
                currentInteractionModeText = null;
            }
            String[] answerOptions =
                new String[((mostPowerfulInteractionModeText != null
                             && currentInteractionModeText != null) ? 3 : 2)];
            int i = 0;
            if (mostPowerfulInteractionModeText != null)
            {
                // first answer option for the security prompt is the most powerful one (if applicable)
                answerOptions[i] = mostPowerfulInteractionModeText;
                i++;
            }
            if (currentInteractionModeText != null)
            {
                // second one is the current interaction mode
                answerOptions[i] = currentInteractionModeText;
                i++;
            }
            // third answer option for the security prompt is no
            answerOptions[i] = securityPromptMessages.getText(
                                   SecurityPromptMessage.ANSWER_ID_DENY, null /* no params*/);
            ConfirmData cData = new ConfirmData(promptText,
                                                answerOptions,
                                                0 /* highlight always the first option */);
            // optimization for the amount of security prompts: if the security
            // prompt text has been shown already, and the security mode is
            // normal and the most powerful interaction mode is NOT oneshot
            // then treat it as if the user has allowed access
            int answer = -1;
            if (iSecurityPromptsMode
                    == GeneralSecuritySettings.DEFAULT_SECURITY_MODE
                    && mostPowerfulInteractionMode != UserSecuritySettings.ONESHOT_INTERACTION_MODE
                    && promptTexts.contains(promptText))
            {
                // treat it as if the user has selected the first answer
                // option the most powerful one
                answer = 0;
            }
            else
            {
                getUiInstance();
                boolean responseAvailable = ui.confirm(
                                                appName,
                                                cData);
                if (!responseAvailable)
                {
                    throw new UserCancelException("User canceled the security prompt");
                }
                if (cData.getAnswer() == ConfirmData.NO_ANSWER
                        || cData.getAnswer() < 0
                        || cData.getAnswer() >= answerOptions.length)
                {
                    // return same settings
                    return settings;
                }
                answer = cData.getAnswer();
            }
            UserSecuritySettings newSettings = null;
            switch (answer)
            {
                // the user chose the first answer option
            case 0:
                if (mostPowerfulInteractionModeText == null)
                {
                    // the first option is the current interaction mode
                    // -> setting has not been changed
                    newSettings = settings;
                }
                else
                {
                    // the first option is the most powerful interaction mode
                    // -> change the settings accordingly
                    newSettings = new UserSecuritySettingsImpl(
                        settings.getName(),
                        mostPowerfulInteractionMode,
                        settings.getAllowedInteractionModes());
                }
                break;
                // the user chose the second answer option
            case 1:
                if (answerOptions.length == 2)
                {
                    // the second answer option is NO -> change the settings accordingly
                    newSettings = handleDenyAnswer(settings);
                }
                else
                {
                    // the second answer option is the curent interaction text
                    // -> setting has not been changed
                    newSettings = settings;
                }
                break;
                // the user chose the third answer option (which is alway NO if present)
            case 2:
                // current interaction mode has been changed to NO
                newSettings = handleDenyAnswer(settings);
                break;
            }
            // cache the security prompt text
            if (iSecurityPromptsMode
                    == GeneralSecuritySettings.DEFAULT_SECURITY_MODE
                    && newSettings.getCurrentInteractionMode()
                    != UserSecuritySettings.NO_INTERACTION_MODE)
            {
                promptTexts.addElement(promptText);
            }
            // mark that the prompt was shown (for the case of session)
            if (newSettings.getCurrentInteractionMode()
                    != UserSecuritySettings.NO_INTERACTION_MODE &&
                    (currentInteractionMode
                     == UserSecuritySettings.SESSION_INTERACTION_MODE
                     || newSettings.getCurrentInteractionMode()
                     == UserSecuritySettings.SESSION_INTERACTION_MODE))
            {
                sessionPromptsShown.put(settings.getName(),
                                        new Boolean(true));
            }
            return newSettings;
        }
        return settings;
    }

    /*
     * Checks if user prompt is required for a certain settings
     *
     * @return  0 if user denied a session prompt already,
     *          1 if user accepted a session prompt already,
     *         -1 if a session prompt was not shown yet,
     */
    public int getSessionPromptStatus(String settingsName)
    {
        Boolean tmp = (Boolean)sessionsDenied.get(settingsName);
        boolean sessionDenied = (tmp == null ? false : tmp.booleanValue());
        if (sessionDenied)
        {
            return 0;
        }
        tmp = (Boolean)sessionPromptsShown.get(settingsName);
        boolean sessionPromptShown = (tmp == null ? false : tmp.booleanValue());
        return (sessionPromptShown ? 1 : -1);
    }

    public void destroy()
    {
        if (ui != null)
        {
            ui.destroy();
        }
    }

    private void getUiInstance()
    {
        if (ui == null)
        {
            ui = RuntimeUiFactory.getRuntimeUi();
        }
    }

    private UserSecuritySettings handleDenyAnswer(UserSecuritySettings settings)
    {
        // if the securityMode is Normal and Session/Blanket is Allowed interaction mode or
        // (the securityMode is User and the currentInteractionMode is Session/Blanket)
        // -> treat this a session no
        if ((iSecurityPromptsMode
                == GeneralSecuritySettings.DEFAULT_SECURITY_MODE
                && (settings.isInteractionModeAllowed(
                        UserSecuritySettings.SESSION_INTERACTION_MODE)
                    || settings.isInteractionModeAllowed(
                        UserSecuritySettings.BLANKET_INTERACTION_MODE)))
                || (iSecurityPromptsMode
                    == GeneralSecuritySettings.USER_SECURITY_MODE
                    && (settings.getCurrentInteractionMode()
                        == UserSecuritySettings.SESSION_INTERACTION_MODE
                        || settings.getCurrentInteractionMode()
                        == UserSecuritySettings.BLANKET_INTERACTION_MODE)))
        {
            sessionsDenied.put(settings.getName(),
                               new Boolean(true));
        }
        return getDenyAnswer(settings);
    }

    private UserSecuritySettings getDenyAnswer(UserSecuritySettings settings)
    {
        return new UserSecuritySettingsImpl(
                   settings.getName(),
                   UserSecuritySettings.NO_INTERACTION_MODE,
                   settings.getAllowedInteractionModes());
    }
}
