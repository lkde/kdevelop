/***************************************************************************
                            KDevVersionControl.java -  description
                             -------------------
    begin                : Wed Jul  4 12:40:03 2001
    copyright            : (C) 2000-2001 Lost Highway Ltd.
    email                : Richard_Dale@tipitina.demon.co.uk
    generated by         : duke@tipitina on Wed Jul  4 12:40:03 2001, using kdoc parser/custom Java conversion backend.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
***************************************************************************/

package org.kde.koala;

import org.kde.qt.*;
import java.util.*;
import java.lang.Error;

/**

 This is the abstract base class which encapsulates everything
 necessary for communicating with version control systems.
 
 @short 
 This is the abstract base class which encapsulates everything
 necessary for communicating with version control systems.
*/
public class KDevVersionControl extends KDevPart  {
	protected KDevVersionControl(Class dummy){super((Class) null);}

	private native void newKDevVersionControl( KDevApi api, QObject parent, String name);
	public KDevVersionControl( KDevApi api, QObject parent, String name) {
		super((Class) null);
		newKDevVersionControl( api, parent, name);
	}
	private native void newKDevVersionControl( KDevApi api);
	public KDevVersionControl( KDevApi api) {
		super((Class) null);
		newKDevVersionControl( api);
	}
	protected native void finalize() throws InternalError;
	public static final int canBeCommited = 1;
	public static final int canBeAdded = 2;
}
