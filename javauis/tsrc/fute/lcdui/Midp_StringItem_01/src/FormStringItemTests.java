/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

/**
 * This class is to test all the StringItem tests
 */

public class FormStringItemTests extends Form implements CommandListener, ItemCommandListener
{

    //referemce to the parent MIDlet
    private Midp_StringItem_01 m = null;

    //the form to add the StringItems
    private Form stringItemForm = null;

    //the label to create the StringItem
    private TextField label = null;

    //the text to create the StringItem
    private TextField text = null;

    //the text to create the StringItem
    private TextField width = null;
    //the text to create the StringItem
    private TextField height = null;

    //the StringItem to test
    private StringItem si = null;

    private ChoiceGroup cg = null;
    private String[] cgTypes = {"LAYOUT_DEFAULT","LAYOUT_LEFT","LAYOUT_RIGHT","LAYOUT_CENTER","LAYOUT_TOP","LAYOUT_BOTTOM",
                                "LAYOUT_VCENTER","LAYOUT_NEWLINE_BEFORE","LAYOUT_NEWLINE_AFTER","LAYOUT_SHRINK","LAYOUT_VSHRINK",
                                "LAYOUT_EXPAND","LAYOUT_VEXPAND"
                               };

    //the command to go back to the main screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the command to create the StringItem with the entered label and text
    private Command cmdCreate = new Command("Create", Command.SCREEN, 1);

    //the command to create the StringItem with the entered label and text and with focus
    private Command cmdCreateHL = new Command("Create as Hyperlink", Command.SCREEN, 1);
    private Command cmdCreateHLM = new Command("Create as Hyperlink 2 cmds", Command.SCREEN, 1);

    //the command to create the StringItem with the entered label and text and with focus
    private Command cmdCreateButton = new Command("Create as Button", Command.SCREEN, 1);
    private Command cmdCreateButtonM = new Command("Create as Button 2 cmds", Command.SCREEN, 1);

    //the command to unlock the Item
    private Command cmdUnlock = new Command("Unlock", Command.SCREEN, 1);
    //the command for the item
    private Command cmdItem = new Command("Item cmd", Command.ITEM, 1);

    //the command to remove/restore the item command or the first item
    private Command cmdRemoveCommand = new Command("Remove command", Command.SCREEN, 1);
    private Command cmdRestoreCommand = new Command("Restore command", Command.SCREEN, 1);
    private Command cmdRemoveItem = new Command("Remove first item", Command.SCREEN, 1);

    //Command for adding the itemcommandlisteners for the stringitems in layout tests
    private Command cmdAddListeners = new Command("Add listeners", Command.SCREEN, 1);

    //the command for layout test
    private Command cmdLayout = new Command("Layout Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdLayoutButton = new Command("Layout Button Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdLayoutHL = new Command("Layout Hyperlink Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdVLayout = new Command("Vertical Layout Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdVLayoutHL = new Command("Vertical Layout Hyperlink Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdVLayoutButton = new Command("Vertical Layout Button Test", Command.SCREEN, 1);
    //the command for adding a StringItem with long text
    private Command cmdLongText = new Command("Long text layout test", Command.SCREEN, 1);

    private Command cmdNext = new Command("Next", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    // commands for the form
    private Command cmdItemF = new Command("ItemF", Command.ITEM, 1);
    private Command cmdOkF = new Command("OkF", Command.OK, 1);
    private Command cmdScreenF = new Command("Add form commands", Command.SCREEN, 1);

    static int change = -1;

    public FormStringItemTests(Midp_StringItem_01 m)
    {
        //set the name of the form to be the name of MIDlet
        super(m.getClass().getName());
        this.m = m;

        //create TextFields
        label = new TextField("Label:", null, 200, TextField.ANY);
        text = new TextField("Text:", null, 200, TextField.ANY);
        width = new TextField("Set preferred width:", null, 5, TextField.NUMERIC);
        height = new TextField("Set preferred height:", null, 5, TextField.NUMERIC);
        cg = new ChoiceGroup("Layout:",ChoiceGroup.EXCLUSIVE,cgTypes,null);
        append(label);
        append(text);
        append(width);
        append(height);
        append(cg);
        addCommand(cmdCreate);
        addCommand(cmdCreateHL);
        addCommand(cmdCreateHLM);
        addCommand(cmdCreateButton);
        addCommand(cmdCreateButtonM);
        addCommand(cmdLayout);
        addCommand(cmdLayoutHL);
        addCommand(cmdLayoutButton);
        addCommand(cmdVLayout);
        addCommand(cmdVLayoutHL);
        addCommand(cmdVLayoutButton);
        addCommand(cmdLongText);
        addCommand(cmdExit);
        setCommandListener(this);

        Display.getDisplay(m).setCurrent(this);

        //create StringItemForm
        stringItemForm = new Form("StringItem");
        stringItemForm.addCommand(cmdBack);
        stringItemForm.addCommand(cmdScreenF);
        stringItemForm.setCommandListener(this);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if(c == cmdBack)
        {
            label.setString(null);
            text.setString(null);
            Display.getDisplay(m).setCurrent(this);
        }
        else if(c == cmdLongText)
        {
            String longString = "This is a really long string. Check that you can successfully" +
                                " read the last line (congratulations).\n" +
                                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                " urna, hendrerit id fringilla eu, feugiat blandit tellus. Curabitur" +
                                " scelerisque tempus ante, eu convallis ligula ultrices nec. Nullam" +
                                " vulputate aliquet ullamcorper. Lorem ipsum dolor sit amet," +
                                " consectetur adipiscing elit. Proin at odio vel urna luctus dignissim" +
                                " a quis urna. Lorem ipsum dolor sit amet, consectetur adipiscing elit." +
                                " Phasellus aliquet luctus eros vitae aliquam. Suspendisse ut magna a" +
                                " ipsum fermentum imperdiet. Maecenas eu nunc sed metus placerat" +
                                " adipiscing. Duis lacus lacus, vestibulum id vestibulum a, tempus at" +
                                " nulla. Proin sagittis nibh vitae dui interdum varius. Cras purus" +
                                " nulla, lobortis eu laoreet a, ultricies vitae sem. Integer aliquam" +
                                " ultricies metus. Nulla nec dignissim mi. Nullam risus augue, egestas" +
                                " vitae facilisis eu, adipiscing eu turpis. Nulla convallis semper" +
                                " lobortis. Nullam eget lacus nunc. Nam ipsum lectus, mattis in mollis" +
                                " eu, tristique nec nibh. Nam pellentesque euismod est, a laoreet turpis" +
                                " pulvinar bibendum. Sed malesuada lacus eu ante luctus ut faucibus odio" +
                                " tristique. Donec quis mi ante. Praesent a porta velit. Duis tincidunt" +
                                " velit eget sapien viverra pulvinar.\n" +
                                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                " urna, hendrerit id fringilla eu, feugiat blandit tellus. Curabitur" +
                                " scelerisque tempus ante, eu convallis ligula ultrices nec. Nullam" +
                                " vulputate aliquet ullamcorper. Lorem ipsum dolor sit amet," +
                                " consectetur adipiscing elit. Proin at odio vel urna luctus dignissim" +
                                " a quis urna. Lorem ipsum dolor sit amet, consectetur adipiscing elit." +
                                " Phasellus aliquet luctus eros vitae aliquam. Suspendisse ut magna a" +
                                " ipsum fermentum imperdiet. Maecenas eu nunc sed metus placerat" +
                                " adipiscing. Duis lacus lacus, vestibulum id vestibulum a, tempus at" +
                                " nulla. Proin sagittis nibh vitae dui interdum varius. Cras purus" +
                                " nulla, lobortis eu laoreet a, ultricies vitae sem. Integer aliquam" +
                                " ultricies metus. Nulla nec dignissim mi. Nullam risus augue, egestas" +
                                " vitae facilisis eu, adipiscing eu turpis. Nulla convallis semper" +
                                " lobortis. Nullam eget lacus nunc. Nam ipsum lectus, mattis in mollis" +
                                " eu, tristique nec nibh. Nam pellentesque euismod est, a laoreet turpis" +
                                " pulvinar bibendum. Sed malesuada lacus eu ante luctus ut faucibus odio" +
                                " tristique. Donec quis mi ante. Praesent a porta velit. Duis tincidunt" +
                                " velit eget sapien viverra pulvinar.\n" +
                                "Now, congratulations if you can read this final line!";
            si = new StringItem("The StringItem", longString);
            stringItemForm.append(si);
            Display.getDisplay(m).setCurrent(stringItemForm);
        }
        else if(c == cmdNext)
        {
            change++;
            changeForm();
        }
        else if(c == cmdRemoveCommand)
        {
            Item item = null;
            item = stringItemForm.get(0);
            item.removeCommand(cmdUnlock);
            item.removeCommand(cmdItem);
        }
        else if(c == cmdRestoreCommand)
        {
            Item item = null;
            item = stringItemForm.get(0);
            item.addCommand(cmdUnlock);
            item.setDefaultCommand(cmdItem);
        }
        else if(c == cmdRemoveItem)
        {
            stringItemForm.delete(0);
        }
        else if(c == cmdLayout)
        {
            layoutTest(Item.PLAIN);
        }
        else if(c == cmdLayoutHL)
        {
            layoutTest(Item.HYPERLINK);
        }
        else if(c == cmdLayoutButton)
        {
            layoutTest(Item.BUTTON);
        }
        else if(c == cmdVLayout)
        {
            verticalLayoutTest(Item.PLAIN);
        }
        else if(c == cmdVLayoutHL)
        {
            verticalLayoutTest(Item.HYPERLINK);
        }
        else if(c == cmdVLayoutButton)
        {
            verticalLayoutTest(Item.BUTTON);
        }
        else if(c == cmdAddListeners)
        {
            int size = stringItemForm.size();
            for(int i = 0; i < size; i++)
            {
                Item item = null;
                item = stringItemForm.get(i);
                item.setItemCommandListener(this);
            }
        }
        else if(c == cmdExit)
        {
            m.destroyApp(false);
            m.notifyDestroyed();
        }
        else if(c == cmdScreenF)
        {
            stringItemForm.addCommand(cmdItemF);
            stringItemForm.addCommand(cmdOkF);
        }
        else
        {
            String l = label.getString();
            String t = text.getString();
            int index = cg.getSelectedIndex();
            String lo = cg.getString(index);
            int layout = 0;

            if(c == cmdCreate)
                si = new StringItem(l, t);
            else if(c == cmdCreateHL || c == cmdCreateHLM)
            {
                si = new StringItem(l, t, Item.HYPERLINK);
                si.setDefaultCommand(cmdItem);
            }
            else if(c == cmdCreateButton || c == cmdCreateButtonM)
            {
                si = new StringItem(l, t, Item.BUTTON);
                si.setDefaultCommand(cmdItem);
            }

            if(lo.equals(cgTypes[0]))
            {
                layout = Item.LAYOUT_DEFAULT;
            }
            else if(lo.equals(cgTypes[1]))
            {
                layout = Item.LAYOUT_LEFT;
            }
            else if(lo.equals(cgTypes[2]))
            {
                layout = Item.LAYOUT_RIGHT;
            }
            else if(lo.equals(cgTypes[3]))
            {
                layout = Item.LAYOUT_CENTER;
            }
            else if(lo.equals(cgTypes[4]))
            {
                layout = Item.LAYOUT_TOP;
            }
            else if(lo.equals(cgTypes[5]))
            {
                layout = Item.LAYOUT_BOTTOM;
            }
            else if(lo.equals(cgTypes[6]))
            {
                layout = Item.LAYOUT_VCENTER;
            }
            else if(lo.equals(cgTypes[7]))
            {
                layout = Item.LAYOUT_NEWLINE_BEFORE;
            }
            else if(lo.equals(cgTypes[8]))
            {
                layout = Item.LAYOUT_NEWLINE_AFTER;
            }
            else if(lo.equals(cgTypes[9]))
            {
                layout = Item.LAYOUT_SHRINK;
            }
            else if(lo.equals(cgTypes[10]))
            {
                layout = Item.LAYOUT_VSHRINK;
            }
            else if(lo.equals(cgTypes[11]))
            {
                layout = Item.LAYOUT_EXPAND;
            }
            else if(lo.equals(cgTypes[12]))
            {
                layout = Item.LAYOUT_VEXPAND;
            }
            si.setItemCommandListener(this);
            si.setLayout(layout);
            setSize(width.getString(),height.getString());
            stringItemForm.append(si);
            stringItemForm.addCommand(cmdRemoveCommand);
            stringItemForm.addCommand(cmdRestoreCommand);
            stringItemForm.addCommand(cmdRemoveItem);
            if(c == cmdCreateButtonM || c == cmdCreateHLM)
            {
                si.addCommand(cmdBack);
            }
            Display.getDisplay(m).setCurrent(stringItemForm);
        }
    }
    public void commandAction(Command c, Item i)
    {
        if(c == cmdBack)
        {
            label.setString(null);
            text.setString(null);
            Display.getDisplay(m).setCurrent(this);
        }
        else if(c == cmdUnlock)
        {
            i.setPreferredSize(-1,-1);
        }
        else if(c == cmdItem)
        {
            i.setLabel(i.getLabel() + ".");
        }
    }
    private void setSize(String width, String height)
    {
        if(width.equals("") && height.equals(""))
        {
            si.setPreferredSize(-1,-1);
        }
        else if(height.equals(""))
        {
            si.setPreferredSize(Integer.parseInt(width),-1);
        }
        else if(width.equals(""))
        {
            si.setPreferredSize(-1,Integer.parseInt(height));
        }
        else
        {
            si.setPreferredSize(Integer.parseInt(width),Integer.parseInt(height));
        }
    }
    private void layoutTest(int mode)
    {
        stringItemForm.deleteAll();
        stringItemForm.addCommand(cmdAddListeners);
        si = new StringItem("Default Layout","StringItem",mode);

        StringItem si2 = new StringItem("Layout Right","StringItem",mode);
        si2.setLayout(StringItem.LAYOUT_RIGHT);

        StringItem si3 = new StringItem("Layout Left","StringItem",mode);
        si3.setLayout(StringItem.LAYOUT_LEFT);

        StringItem si4 = new StringItem("Layout Center","StringItem",mode);
        si4.setLayout(StringItem.LAYOUT_CENTER);

        StringItem si5 = new StringItem("Layout Expand","StringItem",mode);
        si5.setLayout(StringItem.LAYOUT_EXPAND | StringItem.LAYOUT_NEWLINE_BEFORE);
        si5.setDefaultCommand(cmdBack);

        if(mode != Item.PLAIN)
        {
            si.setDefaultCommand(cmdBack);
            si2.setDefaultCommand(cmdBack);
            si3.setDefaultCommand(cmdBack);
            si4.setDefaultCommand(cmdBack);
        }
        stringItemForm.append(si);
        stringItemForm.append(si2);
        stringItemForm.append(si3);
        stringItemForm.append(si4);
        stringItemForm.append(si5);

        Display.getDisplay(m).setCurrent(stringItemForm);
    }

    private void verticalLayoutTest(int mode)
    {
        stringItemForm.deleteAll();
        si = new StringItem("Default","String",mode);
        Spacer sp = new Spacer(10, this.getHeight());
        if(mode != Item.PLAIN)
        {
            si.setDefaultCommand(cmdBack);
        }
        stringItemForm.addCommand(cmdNext);

        stringItemForm.append(si);
        stringItemForm.append(sp);
        Display.getDisplay(m).setCurrent(stringItemForm);
    }
    private void changeForm()
    {

        switch(change)
        {
        case -1:
            stringItemForm.setTitle("Default");
            si.setLabel("Default");
            si.setLayout(StringItem.LAYOUT_DEFAULT);
            break;
        case 0:
            stringItemForm.setTitle("Default(75,75)");
            si.setPreferredSize(75,75);
            break;
        case 1:
            stringItemForm.setTitle("Top");
            si.setPreferredSize(-1,-1);
            si.setLabel("Top");
            si.setLayout(StringItem.LAYOUT_TOP);
            break;
        case 2:
            stringItemForm.setTitle("Top(75,75)");
            si.setPreferredSize(75,75);
            break;
        case 3:
            stringItemForm.setTitle("Vcenter");
            si.setPreferredSize(-1,-1);
            si.setLabel("VCenter");
            si.setLayout(StringItem.LAYOUT_VCENTER);
            break;
        case 4:
            stringItemForm.setTitle("Vcenter(75,75)");
            si.setPreferredSize(75,75);
            break;
        case 5:
            stringItemForm.setTitle("Bottom");
            si.setPreferredSize(-1,-1);
            si.setLabel("Bottom");
            si.setLayout(StringItem.LAYOUT_BOTTOM);
            break;
        case 6:
            stringItemForm.setTitle("Bottom(75,75)");
            si.setPreferredSize(75,75);
            break;
        case 7:
            stringItemForm.setTitle("Vexpand");
            si.setPreferredSize(-1,-1);
            si.setLabel("Vexpand");
            si.setLayout(StringItem.LAYOUT_VEXPAND);
            break;
        case 8:
            change = -1;
            changeForm();
            return;
        }
    }
}
