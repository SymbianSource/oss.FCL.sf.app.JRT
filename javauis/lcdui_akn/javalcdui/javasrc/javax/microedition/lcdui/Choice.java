/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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



package javax.microedition.lcdui;


public interface Choice
{
    static final int EXCLUSIVE = 1;
    static final int MULTIPLE = 2;
    static final int IMPLICIT = 3;
    static final int POPUP = 4;
    //
    static final int TEXT_WRAP_DEFAULT = 0;
    static final int TEXT_WRAP_ON = 1;
    static final int TEXT_WRAP_OFF = 2;

    int size();

    String getString(int aElementNum);

    Image getImage(int aElementNum);

    int append(String aStringElement,Image aImageElement);

    void insert(int aElementNum,String aStringElement,Image aImageElement);

    void delete(int aElementNum);

    void deleteAll();

    void set(int aElementNum,String aStringElement,Image aImageElement);

    boolean isSelected(int aElementNum);

    int getSelectedIndex();

    int getSelectedFlags(boolean[] aSelectedArray);

    void setSelectedIndex(int aElementNum,boolean aSelected);

    void setSelectedFlags(boolean[] aSelectedArray);

    void setFitPolicy(int aFitPolicy);

    int getFitPolicy();

    void setFont(int aElementNum,Font aFont);

    Font getFont(int aElementNum);
}
