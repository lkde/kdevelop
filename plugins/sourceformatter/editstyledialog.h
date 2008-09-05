/* This file is part of KDevelop
*  Copyright (C) 2008 Cédric Pasteur <cedric.pasteur@free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.

*/
#ifndef EDITSTYLEDIALOG_H
#define EDITSTYLEDIALOG_H

#include <KDialog>
#include <KMimeType>
#include "ui_editstyledialog.h"

namespace KTextEditor
{
	class Document;
	class View;
}
class ISourceFormatter;
class SettingsWidget;

class EditStyleDialog : public KDialog
{
		Q_OBJECT

	public:
        EditStyleDialog(ISourceFormatter *formatter, const KMimeType::Ptr &mime,
                         const QString &content=QString(), QWidget *parent=0);
		virtual ~EditStyleDialog();

        QString content();

	protected:
		void init();

	public slots:
// 		void load();
// 		virtual void accept();
		void updatePreviewText(const QString &text);

	private:
		ISourceFormatter *m_sourceFormatter;
		KTextEditor::View *m_view;
		KTextEditor::Document *m_document;
		SettingsWidget *m_settingsWidget;
        KMimeType::Ptr m_mimeType;
		QWidget *m_content;
		Ui::EditStyle m_ui;
};

#endif // EDITSTYLEDIALOG_H
