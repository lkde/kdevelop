/***************************************************************************
                            KDevAppFrontend.java -  description
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

 The interface to a app frontend
 
 @short 
 The interface to a app frontend
 
*/
public class KDevAppFrontend extends KDevPart  {
	protected KDevAppFrontend(Class dummy){super((Class) null);}

	protected native void finalize() throws InternalError;

/** 
 The component shall start to execute an app-like command.
 Running the application is always asynchronous.
     
*/
	public native void startAppCommand( String command);

/** 
 Returns whether the application is currently running.
     
*/
	public native boolean isRunning();
}
