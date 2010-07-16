/*
 * This file is part of KDevelop
 * Copyright 2010 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef REVIEWPATCHDIALOG_H
#define REVIEWPATCHDIALOG_H
#include <KDialog>

namespace Ui {
    class ReviewPatch;
}

class ReviewPatchDialog : public KDialog
{
    Q_OBJECT
    public:
        ReviewPatchDialog(QWidget* parent = 0);
        
        void setBaseDir(const QString& dir);
        void setServer(const KUrl& server);
        void setUsername(const QString& user);
        
        /** @returns the server url with the username and password */
        KUrl server() const;
    private:    
        Ui::ReviewPatch* m_ui;
};

#endif