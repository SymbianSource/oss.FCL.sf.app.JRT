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
 * Description: Container for text editor objects.
 *
 */

// PACKAGE
package com.nokia.mid.ui.impl;

// IMPORTS
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Vector;
import java.lang.ref.WeakReference;
import com.nokia.mid.ui.TextEditor;

// CLASS DESCRIPTION
/**
 * Container for text editor objects. The objects are stored inside weak
 * references in order to enable garbage collection.
 */
public class TextEditorContainer
{
    // Map of parent objects and texteditors
    private Hashtable iContainer;

    /**
     * Represents a vector containing weak objects
     *
     * The class wraps a vector of weak objects inside it and acts similar way
     * than standard vectors.
     *
     * The class does not extend Vector class because the needed functionalities
     * are tapered compared to it
     *
     * Note that this class is thread-safe
     */
    class WeakVector
    {
        // Vector containing weak references
        private Vector iVector;

        /**
         * Default constructor
         */
        public WeakVector()
        {
            iVector = new Vector();
        }

        /**
         * Returns the size of the elements in this weak vector.
         *
         * @return the size of the elements in this weak vector.
         */
        public synchronized int size()
        {
            return iVector.size();
        }

        /**
         * Adds a new object to the vector. The actual object is wrapped inside
         * a weak reference. The vector is compreseed after the object has been
         * added to it. All garbage collected elements will be removed from the
         * vector
         *
         * @param The
         *            component to be added
         */
        public void addElement(Object aObject)
        {
            WeakReference reference = new WeakReference(aObject);

            synchronized (this)
            {
                iVector.addElement(reference);
                compress();
            }
        }

        /**
         * Removes and elemnt from the vector. The vector is compressed after
         * the object has been added to it
         *
         * @param The
         *            component to be removed
         */
        public synchronized void removeElement(Object aObject)
        {
            int index = getObjectIndex(aObject);

            if (index != -1)
            {
                iVector.removeElementAt(index);
            }

            compress();
        }

        /**
         * Returns an enumeration of the actual objects inside the weak vector
         * The compression is not done here to avoid performance drawback
         *
         * @return Enumeration containing non-garbage collected components
         *         inside the vector
         */
        public synchronized Enumeration elements()
        {
            int size = iVector.size();
            Vector objects = new Vector(size);

            // Add actual objects to the object vector
            for (int i = 0; i < size; i++)
            {
                Object object = getObject(i);

                if (object != null)
                {
                    objects.addElement(object);
                }
            }

            return objects.elements();
        }

        /**
         * Compresses the vector. All garbage collected components are removed
         * from the vector.
         */
        private void compress()
        {
            int index = iVector.size();

            while (index-- > 0)
            {
                if (getObject(index) == null)
                {
                    iVector.removeElementAt(index);
                }
            }
        }

        /**
         * Finds the index of the specified object
         *
         * @param aObject
         *            The object to be searched
         * @return The index of the specified object. -1 if not found
         */
        private int getObjectIndex(Object aObject)
        {
            int size = iVector.size();
            for (int i = 0; i < size; i++)
            {
                if (getObject(i) == aObject)
                {
                    return i;
                }
            }

            return -1;
        }

        /**
         * Gets an object from the specified index
         *
         * @param aIndex
         *            The index of the requested object
         * @return The actual object inside the weak reference. null is returned
         *         if the actual object does not exist anymore
         */
        private Object getObject(int aIndex)
        {
            WeakReference weak = (WeakReference) iVector.elementAt(aIndex);

            if (weak != null)
            {
                return weak.get();
            }

            return null;
        }
    }

    /**
     * Constructor.
     */
    public TextEditorContainer()
    {
        iContainer = new Hashtable();
    }

    /**
     * Adds a new editor to this container.
     *
     * @param aEditor
     *            The editor to be added.
     * @param aParent
     *            The parent of the editor.
     */
    public void addEditor(TextEditor aEditor, Object aParent)
    {
        // JDEBUG( "TextEditorContainer.addEditor() +" );

        synchronized (iContainer)
        {
            if (!iContainer.containsKey(aParent))
            {
                iContainer.put(aParent, new WeakVector());
            }

            WeakVector editors = (WeakVector) iContainer.get(aParent);
            editors.addElement(aEditor);

            // JDEBUG( "TextEditorContainer.addEditor(), count=" +
            // editors.size() );
        }

        // JDEBUG( "TextEditorContainer.addEditor() -" );
    }

    /**
     * Removes an existing editor from this container.
     *
     * @param aEditor
     *            The editor to be removed.
     * @param aParent
     *            The parent of the editor.
     */
    public void removeEditor(TextEditor aEditor, Object aParent)
    {
        // JDEBUG( "TextEditorContainer.removeEditor() + container count=" +
        // iContainer.size() );

        synchronized (iContainer)
        {
            WeakVector editors = (WeakVector) iContainer.get(aParent);
            if (editors != null)
            {
                editors.removeElement(aEditor);

                // JDEBUG( "TextEditorContainer.removeEditor(), count="
                // + editors.size() );

                if (editors.size() == 0)
                {
                    iContainer.remove(aParent);
                }
            }
        }

        // JDEBUG( "TextEditorContainer.removeEditor() - container count=" +
        // iContainer.size() );
    }

    /**
     * Returns all editors related to the given parent.
     *
     * @param aParent
     *            The parent object of the editors to be retrieved.
     * @return Enumeration containing editors of the given parent.
     */
    public Enumeration getEditors(Object aParent)
    {
        // JDEBUG( "TextEditorContainer.getEditors() +" );

        Enumeration ret = null;

        synchronized (iContainer)
        {
            WeakVector editors = (WeakVector) iContainer.get(aParent);
            if (editors != null)
            {
                ret = editors.elements();
            }
        }

        // JDEBUG( "TextEditorContainer.getEditors() -" );

        return ret;
    }
}

// End of file
