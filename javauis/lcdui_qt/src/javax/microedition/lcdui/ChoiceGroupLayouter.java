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
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.extension.TableExtension;
import org.eclipse.swt.widgets.*;

/**
 * Responsible for layouting ChoiceGroup item.
 */
class ChoiceGroupLayouter extends ItemLayouter
{

    /**
     * Key name for selection listener.
     */
    private static final String SELECTION_LISTENER = "selection";
    private static final String TRAVERSE_LISTENER = "traverse";
    private static final String FOCUS_LISTENER = "focus";

    private static TableExtension staticTable;

    private static Shell dialogShell;
    private static TableExtension popupTable;

    private MouseListener mouseListener = new MouseListener();

    /**
     * Constructor.
     */
    ChoiceGroupLayouter(DefaultFormLayoutPolicy dflp)
    {
        super(dflp);
    }

    /**
     * Get static eSWT Table widget for size calculations.
     *
     * @param numItems
     */
    static Table eswtGetStaticTable(int numItems)
    {
        if(staticTable == null)
        {
            staticTable = new TableExtension(eswtGetStaticShell(), SWT.NONE);
            staticTable.getHorizontalBar().setVisible(false);
            staticTable.getVerticalBar().setVisible(false);
        }
        staticTable.removeAll();
        for(int i = 0; i < numItems; i++)
        {
            new TableItem(staticTable, SWT.NONE);
        }
        return staticTable;
    }

    /**
     * Set table contents.
     *
     * @param table
     * @param choice
     */
    static void eswtSetTableContents(Table table, ChoiceGroup choice)
    {

        if(choice.getType() == Choice.POPUP)
        {
            // set the only TableItem to be the selected item
            TableItem ti = table.getItem(0);
            int sel = choice.getSelectedIndex();
            if(sel >= 0)
            {
                ti.setImage(0, Image.getESWTImage(choice.getImage(sel)));
                ti.setText(0, choice.getString(sel));
                ti.setFont(0, Font.getESWTFont(choice.getFont(sel)));
            }
            if(ESWTUIThreadRunner.getInstance().getDisplay().getActiveShell()
                    == dialogShell)
            {
                // The popup dialog is open, update and set the items in the popup dialog
                popupTable.removeAll();
                popupTable.getHorizontalBar().setVisible(false);
                popupTable.getVerticalBar().setVisible(false);

                TableItem titem = null;
                for(int i = 0; i < choice.size(); i++)
                {
                    titem= new TableItem(popupTable, SWT.NONE);
                    titem.setImage(0, Image.getESWTImage(choice.getImage(i)));
                    titem.setText(0, choice.getString(i));
                    titem.setFont(0, Font.getESWTFont(choice.getFont(i)));
                }

                // calculate table size, allow maximum dialogshell's height
                // and enable vertical scroll bar if needed
                Point compSize = popupTable.computeSize(
                                     dialogShell.getClientArea().width, SWT.DEFAULT);
                if(compSize.y > dialogShell.getClientArea().height)
                {
                    popupTable.getVerticalBar().setVisible(true);
                    int itemH = popupTable.getItemHeight() + 2;
                    // make height a multiple of item height
                    compSize.y = (dialogShell.getClientArea().height / itemH) * itemH;
                }
                popupTable.setSize(compSize);
            }

        }
        else
        {
            int size = choice.size();
            if(size == table.getItemCount())
            {
                TableItem titem = null;
                for(int i = 0; i < size; i++)
                {
                    titem = table.getItem(i);
                    titem.setImage(0, Image.getESWTImage(choice.getImage(i)));
                    titem.setText(0, choice.getString(i));
                    titem.setFont(0, Font.getESWTFont(choice.getFont(i)));
                }
                // update selection
                if(choice.getType() == Choice.EXCLUSIVE)
                {
                    table.setSelection(choice.getSelectedIndex());
                }
                else
                {
                    boolean[] sel = new boolean[size];
                    choice.getSelectedFlags(sel);
                    for(int i = 0; i < size; i++)
                    {
                        if(sel[i])
                        {
                            table.select(i);
                        }
                        else
                        {
                            table.deselect(i);
                        }
                    }
                }
            }
        }
    }

    /**
     * Creates the eSWT Table for this item.
     */
    Control eswtGetControl(Composite parent, Item item)
    {
        ChoiceGroup choice = (ChoiceGroup) item;
        // create Table
        TableExtension table = new TableExtension(parent,
                getTableStyle(choice.getType()));
        table.getHorizontalBar().setVisible(false);
        table.getVerticalBar().setVisible(false);
        table.setWordWrap(choice.getFitPolicy() == Choice.TEXT_WRAP_ON);
        if(choice.getType() == Choice.POPUP)
        {
            // create one TableItem
            new TableItem(table, SWT.NONE);
        }
        else
        {
            // add all items to table
            for(int i = 0; i < choice.size(); i++)
            {
                new TableItem(table, SWT.NONE, i);
            }
        }
        eswtUpdateItem(choice, table, Item.UPDATE_NONE, null);
        return table;
    }

    /**
     * Add listeners to Layouter specific control.
     */
    void eswtAddSpecificListeners(Item item, Control control)
    {
        super.eswtAddSpecificListeners(item, control);
        Table table = (Table) control;
        SelectionListener sl = new ChoiceGroupSelectionListener((ChoiceGroup) item);
        table.addSelectionListener(sl);
        table.setData(SELECTION_LISTENER, sl);
        TraverseListener tl = new ChoiceGroupTraverseListener((ChoiceGroup) item);
        table.addTraverseListener(tl);
        table.setData(TRAVERSE_LISTENER, tl);
        FocusListener fl = new ChoiceGroupFocusListener((ChoiceGroup) item);
        table.addFocusListener(fl);
        table.setData(FOCUS_LISTENER, fl);
    }

    /**
     * Remove listeners from Layouter specific control.
     */
    void eswtRemoveSpecificListeners(Item item, Control control)
    {
        super.eswtRemoveSpecificListeners(item, control);
        Table table = (Table) control;
        SelectionListener sl = (SelectionListener) table.getData(SELECTION_LISTENER);
        if(sl != null)
        {
            table.removeSelectionListener(sl);
            table.setData(SELECTION_LISTENER, null);
        }
        TraverseListener tl = (TraverseListener) table.getData(TRAVERSE_LISTENER);
        if(tl != null)
        {
            table.removeTraverseListener(tl);
            table.setData(TRAVERSE_LISTENER, null);
        }
        FocusListener fl = (FocusListener) table.getData(FOCUS_LISTENER);
        if(fl != null)
        {
            table.removeFocusListener(fl);
            table.setData(FOCUS_LISTENER, null);
        }
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control)
    {
        return (control instanceof TableExtension);
    }

    /**
     * Updates the values and selections of ChoiceGroup.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param)
    {
        eswtSetTableContents((Table) control, (ChoiceGroup) item);
    }

    /**
     * Returns whether the key event was consumed by ChoiceGroup or not.
     *
     * @param item ChoiceGroup.
     * @param key key code.
     */
    boolean eswtOfferKeyPressed(Item item, int key)
    {
        LayoutObject lo = dfi.getFirstLayoutObjectOfItem(item);
        TableExtension tempExt;
        ChoiceGroup chgr = (ChoiceGroup) item;
        tempExt = (TableExtension) eswtFindSpecificControl(item,
                  lo.getControl());
        boolean ret;
        if(isPopupDialogOpen())
        {
            ret =  true;
        }
        else if((tempExt.getFocusIndex() == 0 && key == SWT.ARROW_UP)
                || (tempExt.getFocusIndex() == (tempExt.getItemCount() - 1)
                    && key == SWT.ARROW_DOWN)
                || (chgr.getType() == Choice.POPUP
                    && !isPopupDialogOpen()))
        {
            ret = false;
        }
        /*else if (tempExt.getFocusIndex() == (tempExt.getItemCount() - 1)
                && key == SWT.ARROW_DOWN) {
            return false;
        }
        if (chgr.getType() == Choice.POPUP && !isPopupDialogOpen()) {
            return false;
        } */
        else if(key == SWT.ARROW_UP || key == SWT.ARROW_DOWN)
        {
            ret = true;
        }
        else
        {
            ret = false;
        }
        return ret;
    }

    /**
     * Returns the minimum area needed to display a ChoiceGroup.
     *
     * @param choicegroup ChoiceGroup object.
     * @return Minimum area needed to display ChoiceGroup.
     */
    static Point calculateMinimumBounds(final ChoiceGroup choicegroup)
    {
        final Point minSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                Table table = eswtGetStaticTable(1);
                minSize.x = getMaximumItemWidth(choicegroup);
                minSize.y = table.computeSize(minSize.x, SWT.DEFAULT).y;
                applyMinMargins(choicegroup, minSize);
            }
        });
        return minSize;
    }

    /**
     * Returns the preferred area needed to display an Item.
     *
     * @param item Item.
     * @return Preferred area needed to display Item. x is width and y is
     *         height.
     */
    static Point calculatePreferredBounds(Item item)
    {
        final Point prefSize = new Point(0, 0);
        final ChoiceGroup cg = (ChoiceGroup) item;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                int numItem = (cg.getType() == Choice.POPUP ? 1 : cg.size());
                Table table = eswtGetStaticTable(numItem);
                eswtSetTableContents(table, cg);
                prefSize.x = getMaximumItemWidth(cg);
                prefSize.y = table.computeSize(prefSize.x, SWT.DEFAULT).y;
                applyPrefMargins(cg, prefSize);
            }
        });
        return prefSize;
    }

    /**
     * Get eSWT style based on List type.
     */
    private int getTableStyle(int listType)
    {
        int tableStyle = SWT.NONE;
        switch(listType)
        {
        case Choice.EXCLUSIVE:
            tableStyle |= SWT.SINGLE | SWT.RADIO;
            break;
        case Choice.MULTIPLE:
            tableStyle |= /*SWT.MULTI |*/ SWT.CHECK;
            break;
        default:
            break;
        }
        return tableStyle;
    }

    private boolean isPopupDialogOpen()
    {
        return (ESWTUIThreadRunner.getInstance().getDisplay().getActiveShell()
                == dialogShell);
    }

    /**
     * Open POPUP dialog.
     *
     * @param choiceGroup ChoiceGroup
     * @param control already layouted control
     */
    private void eswtOpenPopupDialog(final ChoiceGroup choiceGroup,
                                     final Control control)
    {

        final Shell parentShell = choiceGroup.getParent().getShell();
        dialogShell = new Shell(parentShell, SWT.DIALOG_TRIM | SWT.SYSTEM_MODAL);

        dialogShell.setText(choiceGroup.getLabel());
        //org.eclipse.swt.widgets.Display d = parentShell.getDisplay();
        // add Back command
        org.eclipse.ercp.swt.mobile.Command backCommand =
            new org.eclipse.ercp.swt.mobile.Command(dialogShell,
                    org.eclipse.ercp.swt.mobile.Command.BACK, 0);
        backCommand.setText(Command.getDefaultLabel(Command.BACK));

        popupTable = new TableExtension(dialogShell, SWT.NONE);
        popupTable.getHorizontalBar().setVisible(false);
        popupTable.getVerticalBar().setVisible(false);

        TableItem ti = null;
        for(int i = 0; i < choiceGroup.size(); i++)
        {
            ti = new TableItem(popupTable, SWT.NONE);
            ti.setImage(0, Image.getESWTImage(choiceGroup.getImage(i)));
            ti.setText(0, choiceGroup.getString(i));
            ti.setFont(0, Font.getESWTFont(choiceGroup.getFont(i)));
        }

        // calculate table size, allow maximum dialogshell's height
        // and enable vertical scroll bar if needed
        Point compSize = popupTable.computeSize(
                             dialogShell.getClientArea().width, SWT.DEFAULT);
        if(compSize.y > dialogShell.getClientArea().height)
        {
            popupTable.getVerticalBar().setVisible(true);
            int itemH = popupTable.getItemHeight() + 2;
            // make height a multiple of item height
            compSize.y = (dialogShell.getClientArea().height / itemH) * itemH;
        }
        popupTable.setSize(compSize);

        Listener disposeHandler = new Listener()
        {
            public void handleEvent(Event se)
            {
                dialogShell.close();
                dialogShell.dispose();
                dialogShell = null;
                org.eclipse.swt.widgets.Display.getCurrent().
                removeFilter(SWT.MouseDown, mouseListener);
            }
        };

        // if Back command is pressed - dispose
        backCommand.addListener(SWT.Selection, disposeHandler);

        popupTable.addListener(SWT.DefaultSelection, new Listener()
        {
            public void handleEvent(Event se)
            {
                int index = ((Table) se.widget).indexOf((TableItem) se.item);
                // if table gets selected - save selection and dispose dialog
                choiceGroup.internalSetSelectedIndex(index, true);
                eswtUpdateItem(choiceGroup, control, Item.UPDATE_NONE, null);
                dialogShell.close();
                dialogShell.dispose();
                dialogShell = null;
                org.eclipse.swt.widgets.Display.getCurrent().
                removeFilter(SWT.MouseDown, mouseListener);
            }
        });

        dialogShell.pack();
        // place dialogShell to bottom of screen
        Point location = dialogShell.getLocation();
        location.y = parentShell.getSize().y - dialogShell.getSize().y;
        dialogShell.setLocation(location);
        dialogShell.open();
        //Add a mouse listener to the display to listen to a screen tap
        org.eclipse.swt.widgets.Display.getCurrent().
        addFilter(SWT.MouseDown, mouseListener);
    }

    /**
     * Class that receives events from Table and updates ChoiceGroup's value.
     */
    class ChoiceGroupSelectionListener implements SelectionListener
    {

        private ChoiceGroup choiceGroup;

        /**
         * Constructor.
         *
         * @param choiceGroup ChoiceGroup to be updated.
         */
        public ChoiceGroupSelectionListener(ChoiceGroup choiceGroup)
        {
            this.choiceGroup = choiceGroup;
        }

        private void update(SelectionEvent se)
        {
            //int vPosition = dfi.vPosition;
            int index = ((Table) se.widget).indexOf((TableItem) se.item);
            choiceGroup.internalSetSelectedIndex(index,
                                                 !choiceGroup.isSelected(index));
            eswtSetTableContents(eswtGetStaticTable(1), choiceGroup);

        }

        public void widgetDefaultSelected(SelectionEvent se)
        {
            if(choiceGroup.getType() == Choice.EXCLUSIVE)
            {
                update(se);

                // because Table doesn't update our custom selection
                // we have to do it ourselves
                // eswtUpdateItem(choiceGroup, ((Table) se.widget),
                // Item.UPDATE_NONE, null);
            }
            else if(choiceGroup.getType() == Choice.POPUP)
            {
                eswtOpenPopupDialog(choiceGroup, ((Table) se.widget));
            }
        }

        public void widgetSelected(SelectionEvent se)
        {
            if(choiceGroup.getType() == Choice.MULTIPLE)
            {
                if(se.detail == SWT.CHECK)
                {
                    update(se);
                }
            }
            else if(choiceGroup.getType() == Choice.EXCLUSIVE)
            {
                update(se);
                //run the default command if it exists for a exclusive choice group upon selection
                if(choiceGroup.getDefaultCommand()!= null)
                {
                    choiceGroup.callCommandAction(choiceGroup.getDefaultCommand());
                }
            }
        }
    }

    /**
     * ChoiceGroupTraverseListener listens to KeyTraverseEvent and scrolls
     * ChoiceGroup if needed.
     */
    class ChoiceGroupTraverseListener implements TraverseListener
    {
        private ChoiceGroup choicegroup;

        public ChoiceGroupTraverseListener(ChoiceGroup choiceGr)
        {
            choicegroup = choiceGr;
        }

        public void keyTraversed(TraverseEvent te)
        {
            TableExtension table = (TableExtension) te.widget;
            int focusedIndex = table.getFocusIndex();
            // compute focused item location
            int itemHeight = table.getItemHeight();
            int topOfTable = table.getLocation().y;
            if(choicegroup.hasLabel())
            {
                LayoutObject lo = dfi.getFirstLayoutObjectOfItem(choicegroup);
                topOfTable += lo.getY();
            }
            int topYOfSelectedItem = topOfTable + (focusedIndex * itemHeight);
            if(te.keyCode == SWT.ARROW_DOWN)
            {
                topYOfSelectedItem += itemHeight;
                dfi.eswtScrollIfNeeded(topYOfSelectedItem,
                                       topYOfSelectedItem + itemHeight);
            }
            else if(te.keyCode == SWT.ARROW_UP)
            {
                topYOfSelectedItem -= itemHeight;
                dfi.eswtScrollIfNeeded(topYOfSelectedItem,
                                       topYOfSelectedItem + itemHeight);
            }
        }

    }

    /**
     * ChoiceGroupFocusListener reacts on eSWT focusGained event.
     */
    class ChoiceGroupFocusListener implements FocusListener
    {

        private ChoiceGroup choicegroup;

        ChoiceGroupFocusListener(ChoiceGroup cg)
        {
            choicegroup = cg;
        }

        public void focusGained(FocusEvent focusEvent)
        {
            TableExtension te = (TableExtension) focusEvent.widget;
            int direction = dfi.getDirection();
            if(choicegroup.getType() != ChoiceGroup.POPUP)
            {
                if(direction == -1)
                {
                    //do nothing
                }
                else
                {
                    if(direction == SWT.ARROW_UP || direction == SWT.ARROW_LEFT)
                    {
                        te.setFocusIndex(te.getItemCount() - 1);
                    }
                    else if(direction == SWT.ARROW_DOWN || direction == SWT.ARROW_RIGHT)
                    {
                        te.setFocusIndex(0);
                    }
                }
            }

            int focusedIndex = te.getFocusIndex();
            //compute focused item location
            int itemHeight = te.getItemHeight();
            int topOfTable = te.getLocation().y;
            if(choicegroup.hasLabel())
            {
                LayoutObject lo = dfi.getFirstLayoutObjectOfItem(choicegroup);
                topOfTable += lo.getY();
            }

            int topYOfSelectedItem = topOfTable + (focusedIndex * itemHeight);
            if(direction == SWT.ARROW_DOWN || direction == SWT.ARROW_RIGHT)
            {
                topYOfSelectedItem += itemHeight;
                dfi.eswtScrollIfNeeded(topYOfSelectedItem,
                                       topYOfSelectedItem + itemHeight);
            }
            else if(direction == SWT.ARROW_UP || direction == SWT.ARROW_LEFT)
            {
                topYOfSelectedItem -= itemHeight;
                dfi.eswtScrollIfNeeded(topYOfSelectedItem,
                                       topYOfSelectedItem + itemHeight);
            }
        }

        public void focusLost(FocusEvent fe)
        {
        }
    }

    /**
    * Mouse listener for disposing popup choice group on screen tap.
    */
    class MouseListener implements Listener
    {

        public void handleEvent(Event e)
        {
            if(e.type == SWT.MouseDown)
            {
                //Mouse button pressed
                if(isPopupDialogOpen())
                {
                    //popup choice group popup is open
                    if(e.y < 0)
                    {
                        //Tapping done outside of the popup dialog, close and dispose the popup
                        dialogShell.close();
                        dialogShell.dispose();
                        dialogShell = null;
                        //remove the listener as it is no more needed
                        org.eclipse.swt.widgets.Display.getCurrent().removeFilter(
                            SWT.MouseDown, mouseListener);
                    }
                }
            }
        }
    }//end Listener
}
