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

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.internal.extension.TableExtension;
import org.eclipse.swt.widgets.*;

/**
 * Implementation of LCDUI <code>List</code> class.
 */
public class List extends Screen implements Choice
{

    /**
     * The default command triggered when selecting an item on IMPLICIT lists.
     */
    public static final Command SELECT_COMMAND = new Command("", Command.SCREEN, 0);

    private Command selectCommand;

    private int type;

    private ChoiceImpl choiceImpl;

    // eSWT
    private TableExtension eswtTable;

    private EswtTableSelectionListener eswtTableListener =
        new EswtTableSelectionListener();

    /**
     * Constructor.
     *
     * @param title the list's title
     * @param type the type
     */
    public List(String title, int type)
    {
        this(title, type, new String[] {}, null);
    }

    /**
     * Constructor.
     *
     * @param title the list's title
     * @param type the type
     * @param textElements text elements
     * @param imgElements image elements
     * @throws IllegalArgumentException if type is invalid
     * @throws IllegalArgumentException if image elements array is not null and
     *             the length of the arrays don't match
     * @throws NullPointerException if the text elements array is null
     * @throws NullPointerException if any of the text elements is null
     */
    public List(String title, int type, String[] textElements,
                Image[] imgElements)
    {
        super(title);
        switch(type)
        {
        case Choice.IMPLICIT:
        case Choice.EXCLUSIVE:
            choiceImpl = new ChoiceImpl(false);
            break;
        case Choice.MULTIPLE:
            choiceImpl = new ChoiceImpl(true);
            break;
        default:
            throw new IllegalArgumentException(
                MsgRepository.LIST_EXCEPTION_INVALID_TYPE);
        }
        choiceImpl.check(textElements, imgElements);
        this.selectCommand = SELECT_COMMAND;
        this.type = type;
        construct();
        // append elements
        for(int i = 0; i < textElements.length; i++)
        {
            append(textElements[i], imgElements != null
                   ? imgElements[i] : null);
        }
    }

    /* (non-Javadoc)
     * @see Displayable#eswtConstructContent(int)
     */
    Composite eswtConstructContent(int style)
    {
        Composite comp = super.eswtConstructContent(style);
        eswtTable = new TableExtension(comp, getStyle(type));
        eswtTable.getHorizontalBar().setVisible(false);
        return comp;
    }

    /**
     * Called by Display when Displayable should become visible.
     */
    void eswtHandleShowCurrentEvent()
    {
        super.eswtHandleShowCurrentEvent();
        eswtTable.addSelectionListener(eswtTableListener);
    }

    /**
     * Called by Display when Displayable should become hidden.
     */
    void eswtHandleHideCurrentEvent()
    {
        super.eswtHandleHideCurrentEvent();
        eswtTable.removeSelectionListener(eswtTableListener);
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleResizeEvent(int, int)
     */
    void eswtHandleResizeEvent(int width, int height)
    {
        super.eswtHandleResizeEvent(width, height);
        eswtTable.setBounds(getContentComp().getClientArea());
    }

    /**
     * Get Table style based on list type.
     */
    private int getStyle(int listType)
    {
        int tableStyle = SWT.NONE;
        switch(listType)
        {
        case Choice.IMPLICIT:
            tableStyle |= SWT.SINGLE;
            break;
        case Choice.EXCLUSIVE:
            tableStyle |= SWT.SINGLE | SWT.RADIO;
            break;
        case Choice.MULTIPLE:
            tableStyle |= SWT.MULTI | SWT.CHECK;
            break;
        default:
            break;
        }
        return tableStyle;
    }

    private void updateSelection()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtUpdateSelection();
            }
        });
    }

    /**
     * Update eSWT Table selection.
     */
    private void eswtUpdateSelection()
    {
        if(type == IMPLICIT || type == EXCLUSIVE)
        {
            int sel = choiceImpl.getSelectedIndex();
            if((sel == 0) || (eswtTable.getSelectionIndex() != sel))
            {
                eswtTable.setSelection(sel);
            }
        }
        else
        {
            int size = choiceImpl.size();
            for(int i = 0; i < size; i++)
            {
                if(choiceImpl.isSelected(i))
                {
                    eswtTable.select(i);
                }
                else
                {
                    eswtTable.deselect(i);
                }
            }
        }
    }

    private void eswtInsertItem(int index)
    {
        TableItem item = new TableItem(eswtTable, SWT.NONE, index);
        Image img = choiceImpl.getImage(index);
        item.setImage(0, Image.getESWTImage(img));
        item.setText(0, choiceImpl.getString(index));
    }

    private void eswtSetItem(int index)
    {
        TableItem item = eswtTable.getItem(index);
        Image img = choiceImpl.getImage(index);
        item.setImage(0, Image.getESWTImage(img));
        item.setText(0, choiceImpl.getString(index));
    }

    private void eswtDeleteItem(int index)
    {
        eswtTable.getItem(index).dispose();
        choiceImpl.delete(index);
    }

    private void eswtDeleteAllItems()
    {
        for(int i = eswtTable.getItemCount() - 1; i >= 0; i--)
        {
            if(type == Choice.IMPLICIT)
            {
                choiceImpl.delete(i);
            }
            eswtTable.getItem(i).dispose();
        }
    }

    private void eswtSetItemFont(int index)
    {
        org.eclipse.swt.graphics.Font font = Font.getESWTFont(choiceImpl
                                             .getFont(index));
        eswtTable.getItem(index).setFont(0, font);
    }

    /**
     * Append item with specified text and image.
     *
     * @param text the text
     * @param img the image
     * @return index of added item
     */
    public int append(String text, Image img)
    {
        final int index = choiceImpl.append(text, img);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtInsertItem(index);
                eswtUpdateSelection();
            }
        });
        return index;
    }

    /**
     * Insert item with specified text and image.
     *
     * @param position the item index
     * @param text the text
     * @param img the image
     */
    public void insert(int position, String text, Image img)
    {
        choiceImpl.insert(position, text, img);
        final int index = position; // index of added element
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtInsertItem(index);
                eswtUpdateSelection();
            }
        });
    }

    /**
     * Set item with specified text and image.
     *
     * @param position the item index
     * @param text the text
     * @param img the image
     */
    public void set(int position, String text, Image img)
    {
        choiceImpl.set(position, text, img);
        final int index = position; // index of changed element
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtSetItem(index);
                eswtUpdateSelection();
            }
        });
    }

    /**
     * Remove item with at specified position.
     *
     * @param position the item index
     */
    public void delete(int position)
    {
        if (position < 0 || ( position >= size())) 
        {
            throw new IndexOutOfBoundsException(
                    MsgRepository.CHOICE_EXCEPTION_INVALID_ITEM_INDEX);
        }
        final int index = position; // index of changed element
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtDeleteItem(index);
                eswtUpdateSelection();
            }
        });
    }

    /**
     * Remove all items.
     */
    public void deleteAll()
    {
        if(type != Choice.IMPLICIT)
        {
            choiceImpl.deleteAll();
        }
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtDeleteAllItems();
            }
        });
    }

    /**
     * Get the fit policy of this list.
     *
     * @return the lists fir policy
     */
    public int getFitPolicy()
    {
        return choiceImpl.getFitPolicy();
    }

    /**
     * Get the font used in a list item.
     *
     * @param position the index of the item
     * @return the items font
     */
    public Font getFont(int position)
    {
        return choiceImpl.getFont(position);
    }

    /**
     * Get the image part of a list item.
     *
     * @param position the index of the item
     * @return the items image part
     */
    public Image getImage(int position)
    {
        return choiceImpl.getImage(position);
    }

    /**
     * Get the string part of a list item.
     *
     * @param position the index of the item
     * @return the items string part
     */
    public String getString(int position)
    {
        return choiceImpl.getString(position);
    }

    /**
     * Get selected flags.
     *
     * @param selectedArray an array with selected items
     * @return selected flags
     */
    public int getSelectedFlags(boolean[] selectedArray)
    {
        return choiceImpl.getSelectedFlags(selectedArray);
    }

    /**
     * Returns the selected index.
     *
     * @return the selected index
     */
    public int getSelectedIndex()
    {
        return choiceImpl.getSelectedIndex();
    }

    /**
     * Returns if the specified element is selected.
     *
     * @param position specified element index
     * @return true if its selected, false otherwise
     */
    public boolean isSelected(int position)
    {
        return choiceImpl.isSelected(position);
    }

    /**
     * Set the fit policy of this list.
     *
     * @param newFitPolicy the new fit policy
     */
    public void setFitPolicy(int newFitPolicy)
    {
        choiceImpl.setFitPolicy(newFitPolicy);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtTable.setWordWrap(choiceImpl.getFitPolicy()
                                      == Choice.TEXT_WRAP_ON);
            }
        });
    }

    /**
     * Set the font of a list item.
     *
     * @param position the index of the item
     * @param font the desired font
     */
    public void setFont(int position, Font font)
    {
        choiceImpl.setFont(position, font);
        final int index = position; // index of added element
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtSetItemFont(index);
            }
        });
    }

    /**
     * Set the select command of an IMPLICIT list.
     *
     * @param cmd a command.
     */
    public void setSelectCommand(Command cmd)
    {
        if(type == Choice.IMPLICIT)
        {
            if(cmd == null)
            {
            	if(selectCommand != null)
            	{
            		super.removeCommand(selectCommand);
            	}
            }
            else if(cmd != SELECT_COMMAND)
            {
                addCommand(cmd);
            }
            selectCommand = cmd;
        }
    }

    /**
     * Removes command from the List.
     *
     * @param cmd a command.
     */
    public void removeCommand(Command cmd)
    {
        if(cmd == selectCommand)
        {
            selectCommand = null;
        }
        super.removeCommand(cmd);
    }

    /**
     * Set selected flags.
     *
     * @param selectedArray an array with selected items
     */
    public void setSelectedFlags(boolean[] selectedArray)
    {
        choiceImpl.setSelectedFlags(selectedArray);
        updateSelection();
    }

    /**
     * Set selected index.
     *
     * @param position the index of the item
     * @param select selected or not
     */
    public void setSelectedIndex(int position, boolean select)
    {
        choiceImpl.setSelected(position, select);
        updateSelection();
    }

    /**
     * Returns the size of the list.
     *
     * @return the lists size
     */
    public int size()
    {
        return choiceImpl.size();
    }

    /**
     * Table Default Selection listener.
     */
    class EswtTableSelectionListener implements SelectionListener
    {

        private void update(SelectionEvent se)
        {
            if(se.widget != null && se.item != null)
            {
                int index = ((Table) se.widget).indexOf((TableItem) se.item);
                Logger.method(List.this, "updateSel", String.valueOf(index),
                              se);
                if(index >= 0)
                {
                    choiceImpl.setSelected(index, !isSelected(index));
                }
            }
        }

        public void widgetDefaultSelected(SelectionEvent se)
        {
            if(type == Choice.IMPLICIT)
            {
                if(size() > 0)
                {
                    callCommandAction(selectCommand);
                }
            }
        }

        public void widgetSelected(SelectionEvent se)
        {
            if(type == Choice.IMPLICIT || type == Choice.EXCLUSIVE)
            {
                update(se);
            }
            else if(type == Choice.MULTIPLE)
            {
                if(se.detail == SWT.CHECK)
                {
                    update(se);
                }
            }
        }

    }

}
