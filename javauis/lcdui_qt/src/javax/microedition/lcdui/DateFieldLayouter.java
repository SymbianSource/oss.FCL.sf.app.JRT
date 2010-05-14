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

import java.util.Calendar;

import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * Super class for all DateField Layouters. Contains their common
 * functionality.
 */
class DateFieldLayouter extends ItemLayouter
{

    /**
     * Key name for modify listener.
     */
    private static final String MODIFY_LISTENER = "modify";

    private static DateEditor staticDate;

    /**
     * Constructor.
     *
     * @param dflp DefaultFormLayoutPolicy used for layouting.
     */
    DateFieldLayouter(DefaultFormLayoutPolicy dflp)
    {
        super(dflp);
    }

    /**
     * Get static eSWT DateEditor widget for size calculations.
     */
    static DateEditor eswtGetStaticDate()
    {
        if(staticDate == null)
        {
            staticDate = new DateEditor(eswtGetStaticShell(), SWT.NONE,
                                        DateEditor.DATE_TIME | DateEditor.COMPACT);
        }
        return staticDate;
    }

    /**
     * Creates the eSWT control.
     *
     * @param parent where to create.
     * @param item on which it is based. Must be DateField.
     * @return Control.
     */
    Control eswtGetControl(Composite parent, Item item)
    {
        DateField dateField = (DateField) item;
        DateEditor dateEditor = null;

        switch(dateField.getInputMode())
        {
        case DateField.TIME:
            dateEditor = new DateEditor(parent, SWT.NONE,
                                        DateEditor.TIME | DateEditor.COMPACT);
            break;
        case DateField.DATE:
            dateEditor = new DateEditor(parent, SWT.NONE,
                                        DateEditor.DATE | DateEditor.COMPACT);
            break;
        case DateField.DATE_TIME:
            dateEditor = new DateEditor(parent, SWT.NONE,
                                        DateEditor.DATE_TIME | DateEditor.COMPACT);
            break;
        default:
            break;
        }

        if(dateField.getTimeZone() != null)
        {
            dateEditor.setTimeZone(dateField.getTimeZone());
        }
        if(dateField.getDate() != null)
        {
            dateEditor.setDate(dateField.getDate());
        }
        return dateEditor;
    }

    /**
     * Add listeners to Layouter specific control.
     */
    void eswtAddSpecificListeners(Item item, Control control)
    {
        super.eswtAddSpecificListeners(item, control);
        DateField dateField = (DateField) item;
        DateEditor de = (DateEditor) control;
        ModifyListener listener = new DateEditorModifyListener(dateField);
        de.addModifyListener(listener);
        de.setData(MODIFY_LISTENER, listener);
    }

    /**
     * Remove listeners from Layouter specific control.
     */
    void eswtRemoveSpecificListeners(Item item, Control control)
    {
        super.eswtRemoveSpecificListeners(item, control);
        DateEditor de = (DateEditor) control;
        ModifyListener listener = (ModifyListener) de.getData(MODIFY_LISTENER);
        if(listener != null)
        {
            de.removeModifyListener(listener);
            de.setData(MODIFY_LISTENER, null);
        }
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control)
    {
        return (control instanceof DateEditor);
    }

    /**
     * Updates the values of DateField.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param)
    {
        DateField dateField = (DateField) item;
        DateEditor dateEditor = (DateEditor) control;

        if(dateField.getDate() != null)
        {
            dateEditor.setDate(dateField.getDate());
        }

        if(dateField.getTimeZone() != null)
        {
            dateEditor.setTimeZone(dateField.getTimeZone());
        }
    }

    /**
     * Handles key events when DateField is selected. If moving to the right or
     * left focus stays in DateField and only the selection inside of DateField
     * is changed. Otherwise method returns false and allows form to transfer
     * focus.
     */
    boolean eswtOfferKeyPressed(Item item, int key)
    {
        if(key == SWT.ARROW_LEFT || key == SWT.ARROW_RIGHT)
        {
            return true;
        }
        return false;
    }

    /**
     * Returns the minimum area needed to display a DateField.
     *
     * @param datefield DateField object.
     * @return Minimum area needed to display DateField.
     */
    static Point calculateMinimumBounds(final DateField datefield)
    {
        final Point minSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                DateEditor tempEditor = eswtGetStaticDate();
                minSize.x = getMaximumItemWidth(datefield);
                minSize.y = tempEditor.computeSize(minSize.x, SWT.DEFAULT).y;
                applyMinMargins(datefield, minSize);
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
        final DateField datefield = (DateField) item;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                DateEditor tempEditor = eswtGetStaticDate();
                prefSize.x = getMaximumItemWidth(datefield);
                prefSize.y = tempEditor.computeSize(prefSize.x, SWT.DEFAULT).y;
                applyPrefMargins(datefield, prefSize);
            }
        });
        return prefSize;
        // return calculateMinimumBounds((DateField) item);
    }

    /**
     * Class that receives events from DateEditor and updates DateField's value.
     */
    class DateEditorModifyListener implements ModifyListener
    {

        private DateField dateField;

        /**
         * Constructor.
         * @param dateField DateField to be updated.
         */
        public DateEditorModifyListener(DateField dateField)
        {
            this.dateField = dateField;
        }

        /**
         * Called by eSWT when DateEditor's value is modified.
         *
         * @param e Event.
         */
        public void modifyText(ModifyEvent e)
        {
            // Actions needed only if value is adjusted. Compare values
            // depending of dateField's type and exit if values are same.
            if(!isEqualDate(dateField, (DateEditor) e.getSource()))
            {
                Logger.method(dateField, "modify", e);
                dateField.internalSetDate(((DateEditor) e.getSource()).getDate());
                // notify item state listener
                dateField.notifyStateChanged();
            }
        }
    }

    /**
     * Compares date-values of DateField and DateEditor and if values are same,
     * returns true. Method checks the input mode of DateField and compares only
     * values that are visible in DateField.
     *
     * @param dateField  DateField to compare.
     * @param dateEditor DateEditor to compare.
     *
     * @return true if values are equal.
     */
    private boolean isEqualDate(DateField dateField, DateEditor dateEditor)
    {
        Calendar c1 = Calendar.getInstance();
        Calendar c2 = Calendar.getInstance();

        if(dateField.getDate() != null && dateEditor.getDate() != null)
        {
            // set dates of calendars
            c1.setTime(dateField.getDate());
            c2.setTime(dateEditor.getDate());

            switch(dateField.getInputMode())
            {
            case DateField.DATE:
                if((c1.get(Calendar.DAY_OF_MONTH) == c2.get(Calendar.DAY_OF_MONTH))
                        && (c1.get(Calendar.MONTH) == c2.get(Calendar.MONTH))
                        && (c1.get(Calendar.YEAR) == c2.get(Calendar.YEAR)))
                {
                    return true;
                }
                break;
            case DateField.TIME:
                if((c1.get(Calendar.MINUTE) == c2.get(Calendar.MINUTE))
                        && (c1.get(Calendar.HOUR_OF_DAY) == c2.get(Calendar.HOUR_OF_DAY)))
                {
                    return true;
                }
                break;
            case DateField.DATE_TIME:
                if((c1.get(Calendar.DAY_OF_MONTH) == c2.get(Calendar.DAY_OF_MONTH))
                        && (c1.get(Calendar.MONTH) == c2.get(Calendar.MONTH))
                        && (c1.get(Calendar.YEAR) == c2.get(Calendar.YEAR))
                        && (c1.get(Calendar.MINUTE) == c2.get(Calendar.MINUTE))
                        && (c1.get(Calendar.HOUR_OF_DAY) == c2.get(Calendar.HOUR_OF_DAY)))
                {
                    return true;
                }
                break;
            default:
                break;
            }
        }
        return false;
    }

}
