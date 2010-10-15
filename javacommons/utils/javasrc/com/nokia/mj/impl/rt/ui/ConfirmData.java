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

package com.nokia.mj.impl.rt.ui;

public class ConfirmData
{

    // constant identifying that the answer is not available
    public static final int NO_ANSWER = -1;

    // question, answer options, answer suggestion and answer
    private String iQuestion = null;
    private String[] iAnswerOptions = null;
    private int iAnswerSuggestion = 0;
    private int iAnswer = NO_ANSWER;

    /**
     * Constructor
     *
     * @param aQuestion              the question (as localized text) to be
     *                               asked from user
     * @param aAnswerOptions         the answer options (as localized texts).
     *                               If null UI loads default values.
     * @param aAnswerSuggestion      the suggestion for the option to be
     *                               selected (e.g. the option to be
     *                               highlighted when presenting the answer
     *                               options). This must be a valid index
     *                               within aAnswerOptions, otherwise the first
     *                               option from aAnswerOptions is treated as
     *                               the answer suggestion
     */
    public ConfirmData(String aQuestion, String[] aAnswerOptions,
                       int aAnswerSuggestion)
    {
        iQuestion = aQuestion;
        iAnswerOptions = aAnswerOptions;
        iAnswerSuggestion = aAnswerSuggestion;
        iAnswer = NO_ANSWER;
    }

    /**
     * Setter for the user's answer
     *
     * @param aAnswer This must be a valid index within the answer options
     *                encapsulated by this class, otherwise this parameter is
     *                ignored
     */
    public void setAnswer(int aAnswer)
    {
        iAnswer = aAnswer;
    }

    /**
     * Setter for the answer options.
     * @param aAnswerOptions Set answer options. Answer options are
     *                       button labels.
     */    
    public void setAnswerOptions(String[] aAnswerOptions)
    {
        iAnswerOptions = aAnswerOptions;
    }

    /**
     * Getter for the user's question
     *
     * @return the question (as localized text) to be asked from user
     */
    public String getQuestion()
    {
        return iQuestion;
    }

    /**
     * Getter for the user's answer options
     *
     * @return the answer options (as localized texts) or null if there are no
     *         answer options
     */
    public String[] getAnswerOptions()
    {
        return iAnswerOptions;
    }

    /**
     * Getter for the user's answer suggestion
     *
     * @return the suggestion for the option to be selected. This is be a valid
     *         index within return value of getAnswerOptions()
     */
    public int getAnswerSuggestion()
    {
        return iAnswerSuggestion;
    }

    /**
     * Getter for the user's answer
     *
     * @return  An index from within the answer options encapsulated by this
     *          class, indicating the user's response or NO_ANSWER constant
     *          if there is no answer available
     */
    public int getAnswer()
    {
        return iAnswer;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(iQuestion).append("\n");
        if (iAnswerOptions != null)
        {
            for (int i = 0; i < iAnswerOptions.length; i++)
            {
                buf.append("   Answer option ").append(i).append(": ")
                .append(iAnswerOptions[i]).append("\n");
            }
            buf.append("Answer suggestion: " + iAnswerSuggestion)
            .append("\n");
        }
        return buf.toString();
    }

}
