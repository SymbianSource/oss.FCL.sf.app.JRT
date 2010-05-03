/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * LCDUI <code>Choice</code> interface.
 */
public interface Choice {

    /**
     * One element selected at a time.
     */
    static final int EXCLUSIVE = 1;

    /**
     * Multiple elements can be selected.
     */
    static final int MULTIPLE = 2;

    /**
     * Currently focused element is selected.
     */
    static final int IMPLICIT = 3;

    /**
     * One element is selected at a time, and only that is shown.
     */
    static final int POPUP = 4;

    /**
     * Default wrapping policy.
     */
    static final int TEXT_WRAP_DEFAULT = 0;

    /**
     * Elements should be wrapped.
     */
    static final int TEXT_WRAP_ON = 1;

    /**
     * Elements should not be wrapped.
     */
    static final int TEXT_WRAP_OFF = 2;


    /**
     * Get the image of an item.
     *
     * @param elementNum the element index
     * @return the image
     */
    Image getImage(int elementNum);

    /**
     * Get the string of an item.
     *
     * @param elementNum the element index
     * @return the string
     */
    String getString(int elementNum);

    /**
     * Append item with specified text and image.
     *
     * @param stringElement the text
     * @param imageElement the image
     * @return index of added item
     */
    int append(String stringElement, Image imageElement);

    /**
     * Insert item with specified text and image.
     *
     * @param elementNum the item index
     * @param stringElement the text
     * @param imageElement the image
     */
    void insert(int elementNum, String stringElement, Image imageElement);

    /**
     * Set item with specified text and image.
     *
     * @param elementNum the item index
     * @param stringElement the text
     * @param imageElement the image
     */
    void set(int elementNum, String stringElement, Image imageElement);

    /**
     * Remove item at specified position.
     *
     * @param elementNum the item index
     */
    void delete(int elementNum);

    /**
     * Remove all items.
     */
    void deleteAll();

    /**
     * Get the font used in a Choice item.
     *
     * @param elementNum the index of the item
     * @return the items font
     */
    Font getFont(int elementNum);

    /**
     * Set the font of a ChoiceGroup item.
     *
     * @param elementNum the index of the item
     * @param font the desired font
     */
    void setFont(int elementNum, Font font);

    /**
     * Get selected flags.
     *
     * @param selectedArray an array with selected items
     * @return selected flags
     */
    int getSelectedFlags(boolean[] selectedArray);

    /**
     * Set selected flags.
     *
     * @param selectedArray an array with selected items
     */
    void setSelectedFlags(boolean[] selectedArray);

    /**
     * Returns if the specified element is selected.
     *
     * @param elementNum specified element index
     * @return true if its selected, false otherwise
     */
    boolean isSelected(int elementNum);

    /**
     * Returns the selected item's index.
     *
     * @return the selected index
     */
    int getSelectedIndex();

    /**
     * Set selected index.
     *
     * @param elementNum the index of the item
     * @param selected selected or not
     */
    void setSelectedIndex(int elementNum, boolean selected);


    /**
     * Get the fit policy of this Choice.
     *
     * @return the fit policy
     */
    int getFitPolicy();

    /**
     * Set the fit policy of this Choice.
     *
     * @param fitPolicy the new fit policy
     */
    void setFitPolicy(int fitPolicy);

    /**
     * Returns the size of the Choice.
     *
     * @return the size
     */
    int size();

}
