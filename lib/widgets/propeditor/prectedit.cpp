/***************************************************************************
 *   Copyright (C) 2004 by Alexander Dymo                                  *
 *   cloudtemple@mskat.net                                                 *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "prectedit.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qpainter.h>

namespace PropertyLib{

PRectEdit::PRectEdit(MultiProperty* property, QWidget* parent, const char* name): PropertyWidget(property, parent, name)
{
    QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
    m_edit = new QLineEdit(this);
    m_edit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    l->addWidget(m_edit);

    m_edit->setReadOnly(true);
}

QVariant PRectEdit::value() const
{
    return m_value;
}

void PRectEdit::drawViewer(QPainter* p, const QColorGroup& cg, const QRect& r, const QVariant& value)
{
    p->setPen(Qt::NoPen);
    p->setBrush(cg.background());
    p->drawRect(r);
    p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, QString("[ %1, %2, %3, %4 ]").arg(value.toRect().x()).arg(value.toRect().y()).arg(value.toRect().width()).arg(value.toRect().height()));
}

void PRectEdit::setValue(const QVariant& value, bool emitChange)
{
    m_value = value;
    m_edit->setText(QString("[ %1, %2, %3, %4 ]").arg(value.toRect().x()).arg(value.toRect().y()).arg(value.toRect().width()).arg(value.toRect().height()));

    if (emitChange)
        emit propertyChanged(m_property, value);
}

}

#include "prectedit.moc"
