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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.*;

import org.eclipse.swt.layout.*;
import org.eclipse.swt.internal.extension.Style;
/**
 * Class for layouting gauges.
 */
class GaugeLayouter extends ItemLayouter {

    /**
     * Key name for selection listener.
     */
    private static final String SELECTION_LISTENER = "selection";

    /**
     * Constructor.
     *
     * @param dflp DefaultFormLayoutPolicy
     */
    GaugeLayouter(DefaultFormLayoutPolicy dflp) {
        super(dflp);
    }

    /**
     * Creates the eSWT ProgressBar/Slider for this item.
     */
    Control eswtGetControl(Composite parent, Item item) {
        return eswtCreateControl(parent, item);
    }

    /**
     * Construts a gauge control surrounded with composite.
     */
    static Control eswtCreateControl(Composite parent, Item item) {
        Gauge gauge = (Gauge) item;
        // create an owning composite every time
        Composite comp = new Composite(parent, SWT.NONE);

        if (gauge.isInteractive()) {
        	  FormLayout layout = new FormLayout();
            layout.marginBottom = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTBOTTOMMARGIN);
            layout.marginTop = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTTOPMARGIN);
            layout.marginLeft = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTLEFTMARGIN);
            layout.marginRight = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTRIGHTMARGIN);
            layout.spacing = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTVERTICALSPACING);
        	  comp.setLayout(layout);

        	  // Current Value - Mutable Value
            Label currentlabel = new Label(comp, SWT.WRAP);
            currentlabel.setText(Integer.toString( gauge.getValue()));
            currentlabel.setAlignment(SWT.LEAD);

       	    FormData currLabelData = new FormData();
            currLabelData.left = new FormAttachment(0);
        	  currentlabel.setLayoutData(currLabelData);

            Slider slider = new Slider(comp, SWT.HORIZONTAL);
            slider.setMinimum(0);
            slider.setMaximum(gauge.getMaxValue() + 1);
            slider.setSelection(gauge.getValue());
            slider.setIncrement(1);
            slider.setPageIncrement(1);

       	    FormData SliderLayoutData = new FormData();
            SliderLayoutData.right = new FormAttachment(100);
            SliderLayoutData.left = new FormAttachment(currentlabel);
        	  slider.setLayoutData(SliderLayoutData);

            // Min Value
            Label minlabel = new Label(comp, SWT.WRAP);
            minlabel.setText("0");
            minlabel.setAlignment(SWT.LEAD);

       	    FormData minLabelData = new FormData();
            minLabelData.left = new FormAttachment(slider, 0, SWT.LEFT);
            minLabelData.top =  new FormAttachment(slider);
        	  minlabel.setLayoutData(minLabelData);

            // Max Value
						Label maxlabel = new Label(comp, SWT.WRAP);
            maxlabel.setText(Integer.toString(gauge.getMaxValue()) );
            maxlabel.setAlignment(SWT.LEAD);

       	    FormData maxLabelData = new FormData();
            maxLabelData.right = new FormAttachment(slider, 0, SWT.RIGHT);
            maxLabelData.top =  new FormAttachment(slider);
        	  maxlabel.setLayoutData(maxLabelData);
        }
        else {
            ProgressBar progressBar = null;
            if (gauge.isIndefinite()) {
                switch (gauge.getValue()) {
                    case Gauge.CONTINUOUS_IDLE:
                        // TODO: eSWT support required
                        // Gauge like busy-state indicator with no activity:
                        progressBar = new ProgressBar(comp,
                                SWT.HORIZONTAL | SWT.INDETERMINATE);
                        break;
                    case Gauge.CONTINUOUS_RUNNING:
                        // Gauge like busy-state indicator:
                        progressBar = new ProgressBar(comp,
                                SWT.HORIZONTAL | SWT.INDETERMINATE);
                        break;
                    case Gauge.INCREMENTAL_IDLE:
                        // TODO: eSWT support required
                        // Gauge like INCREMENTAL_UPDATING, but no activity.
                        progressBar = new ProgressBar(comp,
                                SWT.HORIZONTAL | SWT.INDETERMINATE);
                        break;
                    case Gauge.INCREMENTAL_UPDATING:
                        // TODO: eSWT support required
                        // Gauge which is updated on  calling Gauge.setValue().
                        progressBar = new ProgressBar(comp,
                                SWT.HORIZONTAL | SWT.INDETERMINATE);
                        break;
                    default:
                        // This is error and will cause NullPointerException
                        // later in this method. Gauge-class takes care that
                        // this is never executed.
                        Logger.warning("Unexpected gauge value: " + gauge.getValue());
                        break;
                }
            }
            else {
                progressBar = new ProgressBar(comp, SWT.HORIZONTAL);
                progressBar.setMaximum(gauge.getMaxValue() + 1);
                progressBar.setSelection(gauge.getValue());
            }
        }

        return comp;
    }

    /**
     * Set the size of the layouted Control.
     */
    void eswtResizeControl(Item item, Control control, int width, int height) {
        super.eswtResizeControl(item, control, width, height);
        control.getParent().setSize(width, height);
        // This will move gauge to the center of the screen:
        control.setLocation((width - control.getBounds().width) / 2,
                control.getLocation().y);
    }

    /**
     * Add listeners to Layouter specific control.
     */
    void eswtAddSpecificListeners(Item item, Control control) {
        super.eswtAddSpecificListeners(item, control);
        Gauge gauge = (Gauge) item;
        if (gauge.isInteractive()) {
            Slider slider = (Slider) control;
            SelectionListener listener = new GaugeSelectionListener(gauge);
            slider.addSelectionListener(listener);
            slider.setData(SELECTION_LISTENER, listener);
        }
    }

    /**
     * Remove listeners from Layouter specific control.
     */
    void eswtRemoveSpecificListeners(Item item, Control control) {
        super.eswtRemoveSpecificListeners(item, control);
        Gauge gauge = (Gauge) item;
        if (gauge.isInteractive()) {
            Slider slider = (Slider) control;
            SelectionListener listener = (SelectionListener) slider
                    .getData(SELECTION_LISTENER);
            if (listener != null) {
                slider.removeSelectionListener(listener);
                slider.setData(SELECTION_LISTENER, null);
            }
        }
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control) {
        if (((Gauge) item).isInteractive()) {
            return (control instanceof Slider);
        }
        else {
            return (control instanceof ProgressBar);
        }
    }

    /**
     * Updates the values of Gauge.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param) {
        Gauge gauge = (Gauge) item;
        if (control instanceof ProgressBar) {
            ((ProgressBar) control).setMaximum(gauge.getMaxValue() + 1);
            ((ProgressBar) control).setSelection(gauge.getValue());
        }
        else if (control instanceof Slider) {
            ((Slider) control).setMaximum(gauge.getMaxValue() + 1);
            ((Slider) control).setSelection(gauge.getValue());
        }
    }

    /**
     * Returns true if that key was consumed by Gauge.
     */
    boolean eswtOfferKeyPressed(Item item, int key) {
        Gauge gauge = (Gauge) item;
        if (gauge.isInteractive()) {
            if (key == SWT.ARROW_RIGHT) {
                gauge.internalSetValue(gauge.getValue() + 1);
               gauge.setLayout(gauge.internalGetLayout());
                
            }
            else if(key == SWT.ARROW_LEFT) {
               gauge.internalSetValue(gauge.getValue() -1);
               gauge.setLayout(gauge.internalGetLayout());
            }
            return true;
        }
        return false;
    }

    /**
     * Returns the minimum area needed to display a Gauge.
     *
     * @param gauge Gauge object.
     * @return Minimum area needed to display Gauge.
     */
    static Point calculateMinimumBounds(final Gauge gauge) {
        final Point minSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                Control comp = eswtCreateControl(eswtGetStaticShell(), gauge);
                minSize.x = getMaximumItemWidth(gauge);
                minSize.y = ((Composite) comp).computeSize(minSize.x, SWT.DEFAULT).y;
                applyMinMargins(gauge, minSize);
                comp.dispose();
            }
        });
        return minSize;
    }

    /**
     * Returns the preferred area needed to display an Item.
     *
     * @param item Item.
     * @return Preferred area needed to display Item. x is width
     *      and y is height.
     */
    static Point calculatePreferredBounds(Item item) {
        final Point prefSize = new Point(0, 0);
        final Gauge gauge = (Gauge)item;
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                Control comp = eswtCreateControl(eswtGetStaticShell(), gauge);
                prefSize.x = getMaximumItemWidth(gauge);
                prefSize.y = ((Composite) comp).computeSize(prefSize.x, SWT.DEFAULT).y;
                applyPrefMargins(gauge, prefSize);
                comp.dispose();
            }
        });
        return prefSize;
    }

    /**
     * Class that receives events from slider and updates gauge's value.
     */
    class GaugeSelectionListener implements SelectionListener {

        private Gauge gauge;

        /**
         * Constructor.
         * @param gauge Gauge to be updated.
         */
        public GaugeSelectionListener(Gauge gauge) {
            this.gauge = gauge;
        }

        public void widgetDefaultSelected(SelectionEvent e) { }

        /**
         * Called by eSWT when Slider's value is changed.
         * Updates Gauge's value.
         */
        public void widgetSelected(SelectionEvent e) {
            int newValue = ((Slider) e.getSource()).getSelection();
            // Actions needed only if value is adjusted:
            if (newValue != gauge.getValue()) {
                // set Gauge value
                gauge.internalSetValue(newValue);
                gauge.setLayout(gauge.internalGetLayout());
            }
        }
    }
}
