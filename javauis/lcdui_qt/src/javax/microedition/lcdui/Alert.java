/*
* Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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

import java.util.Timer;
import java.util.TimerTask;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.extension.Style;
import org.eclipse.swt.internal.extension.LabelExtension;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;

/**
 * The <code>Alert</code> class is basically a message dialog which shows
 * information (in form of text and image) to the user. It's highly customizable
 * and it can be:
 * <li>timed - when it waits for a predefined period of time and dismisses
 *             itself automatically </li>
 * <li>modal - user has to choose a command and dismiss the dialog
 *             explicitly</li>
 */
public class Alert extends Screen
{

    /**
     * Timeout constant used for modal Alerts.
     *
     * @value for FOREVER is -2.
     */
    public static final int FOREVER = -2;

    /**
     * The default command triggered when dismissing an Alert.
     */
    public static final Command DISMISS_COMMAND =
        new Command("", Command.OK, 0);

    /**
     * Default command listener.
     */
    private AlertCommandListener implicitListener = new AlertCommandListener();

    /**
     * Alert Timer.
     */
    private Timer timer = new Timer();

    /**
     * Alert Timer Task.
     */
    private AlertTimerTask timerTask;

    /**
     * Stores next Displayable to switch to.
     */
    private Displayable nextDisplayable;

    private int timeout;
    private String text;
    private Image image;
    private AlertType type;
    private Gauge indicator;
    private boolean textScrolling;

    // eSWT widgets
    private ScrolledTextComposite eswtScrolledText;
    private LabelExtension eswtImgLabel;
    private ProgressBar eswtProgressBar;
    private FormData eswtProgbarLD;

    private Shell topShell;
    private ResizeListener resizeListener = new ResizeListener();
    private KeyListener keyListener = new KeyListener();

    /**
     * Create a new empty Alert with the specified title.
     *
     * @param aTitle the title string
     */
    public Alert(String aTitle)
    {
        this(aTitle, null, null, null);
    }

    /**
     * Create a new Alert with the specified title, text, image, and alert type.
     *
     * @param title the title string
     * @param text the text string
     * @param image the image
     * @param type the alert type
     */
    public Alert(String title, String text, Image image, AlertType type)
    {
        super(title);
        construct();
        this.type = type;
        setImage(image);
        setString(text);
        setTimeout(getDefaultTimeout());
        super.addCommand(DISMISS_COMMAND);
        super.setCommandListener(implicitListener);
    }

    /**
     * Constructs custom eSWT shell for alert dialog.
     *
     * @return custom eSWT dialog shell
     */
    Shell eswtConstructShell(int style)
    {
        topShell = super.eswtConstructShell(style);
        Shell dialogShell = new Shell(topShell, style | SWT.DIALOG_TRIM | SWT.RESIZE);
        return dialogShell;
    }

    /**
     * Creates content Composite.
     */
    Composite eswtConstructContent(int style)
    {
        Composite comp = super.eswtConstructContent(SWT.VERTICAL);

        FormLayout layout = new FormLayout();
        layout.marginBottom = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTBOTTOMMARGIN);
        layout.marginTop = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTTOPMARGIN);
        layout.marginLeft = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTLEFTMARGIN);
        layout.marginRight = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTRIGHTMARGIN);
        layout.spacing = Style.pixelMetric(Style.QSTYLE_PM_LAYOUTVERTICALSPACING);
        comp.setLayout(layout);

        eswtScrolledText = new ScrolledTextComposite(comp, comp
                .getVerticalBar());
        eswtImgLabel = new LabelExtension(comp, SWT.NONE);

        FormData imageLD = new FormData();
        imageLD.right = new FormAttachment(100);
        imageLD.top = new FormAttachment(0);
        eswtImgLabel.setLayoutData(imageLD);

        FormData scrolledTextLD = new FormData();
        scrolledTextLD.left = new FormAttachment(0);
        scrolledTextLD.top = new FormAttachment(0);
        scrolledTextLD.right = new FormAttachment(eswtImgLabel);
        eswtScrolledText.setLayoutData(scrolledTextLD);

        eswtProgbarLD = new FormData();
        eswtProgbarLD.left = new FormAttachment(0);
        eswtProgbarLD.right = new FormAttachment(100);
        eswtProgbarLD.bottom = new FormAttachment(100);

        eswtUpdateProgressbar(comp, false, false);

        return comp;
    }

    int eswtGetPreferredContentHeight()
    {
        int ret = getContentComp().computeSize(SWT.DEFAULT, SWT.DEFAULT).y;

        // Point imgSize = (eswtImgLabel != null
        // ? eswtImgLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT) : new Point(0, 0));
        // int ret = Math.max(
        // Math.min(
        // eswtScrolledText.computeSize(topShell.getClientArea().width - imgSize.x, SWT.DEFAULT).y,
        // topShell.getClientArea().height / 2),
        // imgSize.y);

        if(eswtProgressBar != null && eswtProgressBar.isVisible())
        {
            ret += eswtProgressBar.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        }
        return ret;
    }

    /**
     * Update ProgressBar widget.<br>
     * Try to reuse the old ProgressBar if possible, otherwise dispose it and
     * create a new one.
     *
     * @param parent
     * @param indeterminate
     * @param visible
     */
    void eswtUpdateProgressbar(Composite parent,
                               boolean indeterminate,
                               boolean visible)
    {
        // Only dispose old ProgressBar if it has wrong style
        if(eswtProgressBar != null)
        {
            boolean isIndeterminate =
                (eswtProgressBar.getStyle() & SWT.INDETERMINATE) != 0;
            if(indeterminate != isIndeterminate)
            {
                eswtProgressBar.setLayoutData(null);
                eswtProgressBar.dispose();
                eswtProgressBar = null;
            }
        }
        // create new ProgressBar
        if(eswtProgressBar == null)
        {
            int newStyle = indeterminate ? SWT.INDETERMINATE : SWT.NONE;
            eswtProgressBar = new ProgressBar(parent, newStyle);
            eswtProgressBar.setLayoutData(eswtProgbarLD);
            // update ScrolledText's layoutdata
            FormData imageLD = (FormData) eswtImgLabel.getLayoutData();
            imageLD.bottom = new FormAttachment(eswtProgressBar);
        }
        // set Progressbar visibility
        if(eswtProgressBar != null)
        {
            eswtProgbarLD.top = (visible ? null : new FormAttachment(100));
            eswtProgressBar.setVisible(visible);
        }
    }

    /**
     * Update ProgressBar values.
     *
     * @param minValue the minimum value
     * @param maxValue the maximum value
     * @param selValue the value
     */
    void eswtSetProgressbarValues(int minValue, int maxValue, int selValue)
    {
        if(eswtProgressBar != null)
        {
            eswtProgressBar.setMinimum(minValue);
            eswtProgressBar.setMaximum(maxValue);
            eswtProgressBar.setSelection(selValue);
        }
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Displayable#handleShowEvent()
     */
    void eswtHandleShowCurrentEvent()
    {
        super.eswtHandleShowCurrentEvent();
        topShell.addListener(SWT.Resize, resizeListener);

        // add key filter for scrollable text composite
        org.eclipse.swt.widgets.Display.getCurrent().addFilter(
            SWT.KeyDown, keyListener);
        org.eclipse.swt.widgets.Display.getCurrent().addFilter(
            SWT.Traverse, keyListener);
        org.eclipse.swt.widgets.Display.getCurrent().addFilter(
            SWT.MouseUp, keyListener);
        resetTimerTask(true);
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Displayable#handleHideEvent()
     */
    void eswtHandleHideCurrentEvent()
    {
        super.eswtHandleHideCurrentEvent();
        topShell.removeListener(SWT.Resize, resizeListener);

        nextDisplayable = null;
        resetTimerTask(false);

        // remove key filter for scrollable text composite
        org.eclipse.swt.widgets.Display.getCurrent().removeFilter(
            SWT.KeyDown, keyListener);
        org.eclipse.swt.widgets.Display.getCurrent().removeFilter(
            SWT.Traverse, keyListener);
        org.eclipse.swt.widgets.Display.getCurrent().removeFilter(
            SWT.MouseUp, keyListener);
    }


    /**
     * Set the next displayable to be shown.
     *
     * @param next next displayable
     */
    void setNextDisplayable(Displayable next)
    {
        nextDisplayable = next;
    }

    /**
     * Gets the default Alert image based on the type.
     *
     * @param type the alert type
     * @return the default image based on the type or null if the type is null
     */
    private static int getDefaultImageType(final AlertType type)
    {
        if(type == AlertType.ERROR)
        {
            return LabelExtension.STANDARDICON_ERROR;
        }
        else if(type == AlertType.WARNING)
        {
            return LabelExtension.STANDARDICON_WARNING;
        }
        else if(type == AlertType.INFO)
        {
            return LabelExtension.STANDARDICON_INFO;
        }
        else if(type == AlertType.CONFIRMATION)
        {
            return LabelExtension.STANDARDICON_CONFIRMATION;
        }
        else if(type == AlertType.ALARM)
        {
            return LabelExtension.STANDARDICON_ALARM;
        }
        else
        {
            return LabelExtension.STANDARDICON_ALARM;
        }

    }

    /**
     * Gets the default Alert text based on the type.
     *
     * @param type the alert type
     * @return the default text based on the type
     */
    private static String getDefaultText(final AlertType type)
    {
        if(type == AlertType.ERROR)
        {
            return MsgRepository.ALERT_DEFAULT_TEXT_ERROR;
        }
        else if(type == AlertType.WARNING)
        {
            return MsgRepository.ALERT_DEFAULT_TEXT_WARNING;
        }
        else if(type == AlertType.INFO)
        {
            return MsgRepository.ALERT_DEFAULT_TEXT_INFO;
        }
        else if(type == AlertType.CONFIRMATION)
        {
            return MsgRepository.ALERT_DEFAULT_TEXT_CONFIRMATION;
        }
        else if(type == AlertType.ALARM)
        {
            return MsgRepository.ALERT_DEFAULT_TEXT_ALARM;
        }
        else
        {
            return MsgRepository.ALERT_DEFAULT_TEXT_ALERT;
        }
    }

    /**
     * Returns if the Alert is modal.
     */
    private boolean isModal()
    {
        return ((timeout == FOREVER)
                || (getNumCommands() > 1)
                || isTextScrolling());
    }

    /**
     * Set the time for the Alert to be shown.
     *
     * @param timeout the timeout value in milliseconds or FOREVER
     * @throws IllegalArgumentException if time is not positive nor FOREVER.
     */
    public void setTimeout(int timeout)
    {
        if(timeout > 0 || timeout == FOREVER)
        {
            this.timeout = timeout;
            setCommandsVisibility(isModal());
        }
        else
        {
            throw new IllegalArgumentException(
                MsgRepository.ALERT_EXCEPTION_INVALID_TIMEOUT);
        }
    }

    /**
     * Get the time the Alert is shown.
     *
     * @return timeout in milliseconds, or FOREVER
     */
    public int getTimeout()
    {
        if(isModal())
        {
            return FOREVER;
        }
        else
        {
            return timeout;
        }
    }

    /**
     * Get the default time for showing an Alert.
     *
     * @return default timeout in milliseconds
     */
    public int getDefaultTimeout()
    {
        return Config.ALERT_DEFAULT_TIMEOUT;
    }

    /**
     * Sets the type of the Alert.
     *
     * @param type an AlertType or null if it doesn't have a specific type
     */
    public void setType(AlertType type)
    {
        this.type = type;
        if(text == null)
        {
            // show default text
            setString(text);
        }
        if(image == null)
        {
            // show default image
            setImage(image);
        }
    }

    /**
     * Gets the type of the Alert.
     *
     * @return an AlertType or null if it doesn't have a specific type
     */
    public AlertType getType()
    {
        return type;
    }

    /**
     * Sets the image of this Alert.
     *
     * @param newImage an Image, or null if there is no image
     */
    public void setImage(Image newImage)
    {
        image = newImage;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                Image temp = (image != null) ? image : null;
                //Get the image size from QT Style for scaling
                int scaleToSize = Style.pixelMetric(Style.QSTYLE_PM_MESSAGEBOXICONSIZE);

                if(temp != null)
                {

                    //calculate the aspect ratio
                    float aspectRatio = (float)temp.getHeight()/temp.getWidth();

                    //check the image size
                    if((temp.getWidth() > scaleToSize) ||
                            (temp.getHeight() > scaleToSize))
                    {
                        // we need to scale down the image
                        if(temp.getWidth() > scaleToSize)
                        {
                            //Width is greater
                            Image wScaled = Image.createImage(temp,
                                                              scaleToSize,
                                                              (int)(scaleToSize*aspectRatio));

                            //now check the new dimension against height
                            if(wScaled.getHeight() > scaleToSize)
                            {
                                //scale the image again
                                Image whScaled = Image.createImage(temp,
                                                                   scaleToSize,
                                                                   scaleToSize);
                                eswtImgLabel.setImage(Image.getESWTImage(whScaled));
                            }
                            else
                            {
                                //height was ok after scaling on width
                                eswtImgLabel.setImage(Image.getESWTImage(wScaled));
                            }
                        }
                        else if(temp.getHeight() > scaleToSize)
                        {
                            //Height is greater
                            Image hScaled = Image.createImage(temp,
                                                              (int)(scaleToSize/aspectRatio),
                                                              scaleToSize);

                            //now check the new dimension against width
                            if(hScaled.getWidth()> scaleToSize)
                            {
                                //scale the image again
                                Image hwScaled = Image.createImage(temp,
                                                                   scaleToSize,
                                                                   scaleToSize);
                                eswtImgLabel.setImage(Image.getESWTImage(hwScaled));
                            }
                            else
                            {
                                //widh was ok after scaling using height
                                eswtImgLabel.setImage(Image.getESWTImage(hScaled));
                            }
                        }

                    }
                    else
                    {
                        // image is right size
                        eswtImgLabel.setImage(Image.getESWTImage(temp));
                    }
                }
                else
                {
                    // no image
                    if(type != null)
                    {
                        //display the default image
                        eswtImgLabel.setStandardIcon(getDefaultImageType(type),scaleToSize,scaleToSize);
                        eswtImgLabel.pack();
                    }
                }
                eswtSetPreferredContentSize(-1, eswtGetPreferredContentHeight());
                getContentComp().layout();
            }
        });
    }

    /**
     * Gets the image of this Alert.
     *
     * @return an Image, or null if there is no image
     */
    public Image getImage()
    {
        return image;
    }

    /**
     * Checks if the text label's scrollbar is visible.
     *
     * @return true if the scrollbar is visible.
     */
    private boolean isTextScrolling()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                textScrolling = eswtScrolledText.isTextScrolling();
            }
        });
        return textScrolling;
    }

    /**
     * Sets the text used in the Alert.
     *
     * @param newText the Alert's text string, or null if there is no text
     */
    public void setString(String newText)
    {
        text = newText;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                String temp = (text != null) ? text : getDefaultText(type);
                eswtScrolledText.setText(temp);
                eswtSetPreferredContentSize(-1, eswtGetPreferredContentHeight());
                getContentComp().layout();
            }
        });
        setCommandsVisibility(isModal());
    }

    /**
     * Gets the text used in the Alert.
     *
     * @return the Alert's text string, or null if there is no text
     */
    public String getString()
    {
        return text;
    }

    /**
     * Sets an activity indicator on this Alert.
     *
     * @param newIndicator the activity indicator for this Alert, or null if
     *            there is none
     * @throws IllegalArgumentException if indicator does not meet the
     *             restrictions for its use in an Alert
     */
    public void setIndicator(Gauge newIndicator)
    {
        if(newIndicator != null && !newIndicator.isSuitableForAlert())
        {
            throw new IllegalArgumentException(
                MsgRepository.ALERT_EXCEPTION_INVALID_INDICATOR);
        }
        // remove old Gauge parent
        if(indicator != null)
        {
            indicator.setParent(null);
        }
        // store the indicator
        indicator = newIndicator;
        // set new Gauge parent
        if(indicator != null)
        {
            indicator.setParent(this);
        }
        updateIndicator();
    }

    /**
     * Gets the activity indicator of this Alert.
     *
     * @return the activity indicator of this Alert, or null if there is none
     */
    public Gauge getIndicator()
    {
        return indicator;
    }

    /**
     * Update indicator if it changed.
     */
    void updateIndicator()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                if(indicator != null)
                {
                    // show ProgressBar
                    if(indicator.isIndefinite())
                    {
                        // indefinite ProgressBar
                        switch(indicator.getValue())
                        {
                        case Gauge.CONTINUOUS_IDLE:
                        case Gauge.INCREMENTAL_IDLE:
                            // currently these are mapped to full progress bar
                            // TODO: eSWT support required
                            eswtUpdateProgressbar(getContentComp(), false, true);
                            eswtSetProgressbarValues(0, 1, 1);
                            break;
                        case Gauge.CONTINUOUS_RUNNING:
                            eswtUpdateProgressbar(getContentComp(), true, true);
                            break;
                        case Gauge.INCREMENTAL_UPDATING:
                            // currently this are mapped to blinking
                            // empty and full progress bar
                            // TODO: eSWT support required
                            eswtUpdateProgressbar(getContentComp(), false, true);
                            if(eswtProgressBar.getSelection() > 0)
                            {
                                eswtSetProgressbarValues(0, 1, 0);
                            }
                            else
                            {
                                eswtSetProgressbarValues(0, 1, 1);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        // definite ProgressBar
                        eswtUpdateProgressbar(getContentComp(), false, true);
                        eswtSetProgressbarValues(0, indicator.getMaxValue(),
                                                 indicator.getValue());
                    }
                }
                else
                {
                    // hide ProgressBar
                    eswtUpdateProgressbar(getContentComp(), false, false);
                }
                eswtSetPreferredContentSize(-1, eswtGetPreferredContentHeight());
                getContentComp().layout();
            }
        });
    }

    /* (non-Javadoc)
     * @see Displayable#addCommand(Command)
     */
    public void addCommand(Command command)
    {
        if(command != DISMISS_COMMAND)
        {
            super.addCommand(command);
            super.removeCommand(DISMISS_COMMAND);
            setCommandsVisibility(isModal());
        }
    }

    /* (non-Javadoc)
     * @see Displayable#removeCommand(Command)
     */
    public void removeCommand(Command command)
    {
        if(command != DISMISS_COMMAND)
        {
            super.removeCommand(command);
            if(getNumCommands() == 0)
            {
                super.addCommand(DISMISS_COMMAND);
            }
            setCommandsVisibility(isModal());
        }
    }

    /* (non-Javadoc)
     * @see Displayable#setCommandListener(CommandListener)
     */
    public void setCommandListener(CommandListener listener)
    {
        if(listener == null)
        {
            listener = implicitListener;
        }
        super.setCommandListener(listener);
    }

    void resetTimerTask(boolean reset)
    {
        if(timerTask != null)
        {
            timerTask.cancel();
            timerTask = null;
        }
        if(reset && !isModal())
        {
            // if not modal schedule new timer
            timerTask = new AlertTimerTask();
            timer.schedule(timerTask, timeout);
        }
    }

    /**
     * Alert Timer task. Triggers the first command on the Alert.
     */
    class AlertTimerTask extends TimerTask
    {

        public void run()
        {
            // trigger the first available command on the listener
            if(!isModal())
            {
                callCommandAction(getCommand(0));
            }
        }

    }

    /**
     * Default (implicit) command listener. Any Commands close the Alert.
     */
    class AlertCommandListener implements CommandListener
    {

        public void commandAction(Command aCommand, Displayable aSource)
        {
            final Alert alert = (Alert) aSource;
            Display.getDisplay().setCurrent(alert.nextDisplayable);
        }

    }

    /**
     * Key listener. Also handles scrolling of text composite.
     */
    class KeyListener implements Listener
    {

        public void handleEvent(Event e)
        {
            if(e.type == SWT.Traverse)
            {
                e.doit = false;
            }
            else if(e.type == SWT.KeyDown)
            {
                if(!isModal())
                {
                    resetTimerTask(false);
                    callCommandAction(getCommand(0));
                }
                else if(e.keyCode == SWT.ARROW_DOWN)
                {
                    Point p = eswtScrolledText.getOrigin();
                    eswtScrolledText.setOrigin(p.x, p.y
                                               + Config.ALERT_TEXT_SCROLLING_DELTA);
                }
                else if(e.keyCode == SWT.ARROW_UP)
                {
                    Point p = eswtScrolledText.getOrigin();
                    eswtScrolledText.setOrigin(p.x, p.y
                                               - Config.ALERT_TEXT_SCROLLING_DELTA);
                }
            }
            else if(e.type == SWT.MouseUp)
            {
                if(!isModal())
                {
                    resetTimerTask(false);
                    callCommandAction(getCommand(0));
                }
            }
        }

    }

    /**
     * Resize listener which listens to bottom shell's resize events and
     * forwards them to top shell.
     */
    class ResizeListener implements Listener
    {

        public void handleEvent(Event event)
        {
            // explicitly forward topShell resize events to dialogShell
            getShell().notifyListeners(SWT.Resize, event);
        }

    }

    /**
     * Dispose Alert.
     */
    void dispose()
    {
        super.dispose();
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                topShell.dispose();
            }
        });
    }

}
