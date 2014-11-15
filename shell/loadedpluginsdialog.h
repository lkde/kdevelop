/**************************************************************************
*   Copyright 2009 Andreas Pakulat <apaku@gmx.de                          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#ifndef KDEVPLATFORM_LOADEDPLUGINSDIALOG_H
#define KDEVPLATFORM_LOADEDPLUGINSDIALOG_H

#include <QDialog>

class LoadedPluginsDialog : public QDialog
{
    Q_OBJECT
public:
    LoadedPluginsDialog( QWidget* parent = 0 );
};

#endif // KDEVPLATFORM_LOADEDPLUGINSDIALOG_H
