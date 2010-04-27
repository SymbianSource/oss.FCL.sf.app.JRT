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
* Description:
*
*/

package javax.microedition.sensor;

/**
 * Please refer JSR 256 for more details.
 * */
public interface Channel
{

    /**
     * Please refer JSR 256 for more details.
     */
    public void addCondition(ConditionListener listener, Condition condition);

    /**
     *Please refer JSR 256 for more details.
     */
    public ChannelInfo getChannelInfo();

    /**
     * Please refer JSR 256 for more details.
     */
    public Condition[] getConditions(ConditionListener listener);

    /**
     * Please refer JSR 256 for more details.
     */
    public String getChannelUrl();



    /**
     * Please refer JSR 256 for more details.
     */
    public void removeAllConditions();




    /**
     * Please refer JSR 256 for more details.
     */
    public void removeCondition(ConditionListener listener, Condition condition);



    /**
     * Please refer JSR 256 for more details.
     */
    public void removeConditionListener(ConditionListener listener);

}
