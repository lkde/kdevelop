/*
 * Low level GDB interface.
 *
 * Copyright 1999 John Birch <jbb@kdevelop.org >
 * Copyright 2007 Vladimir Prus <ghost@cs.msu.su>
 * Copyright 2016 Aetf <aetf@unlimitedcodeworks.xyz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
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

#include "gdb.h"

#include "dbgglobal.h"
#include "debuglog.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KShell>

#include <QApplication>
#include <QFileInfo>
#include <QUrl>

using namespace KDevMI::GDB;
using namespace KDevMI::MI;

GdbDebugger::GdbDebugger(QObject* parent)
    : MIDebugger(parent)
{
}

GdbDebugger::~GdbDebugger()
{
}

bool GdbDebugger::start(KConfigGroup& config, const QStringList& extraArguments)
{
    // FIXME: verify that default value leads to something sensible
    QUrl gdbUrl = config.readEntry(Config::GdbPathEntry, QUrl());
    if (gdbUrl.isEmpty()) {
        m_debuggerExecutable = QStringLiteral("gdb");
    } else {
        // FIXME: verify its' a local path.
        m_debuggerExecutable = gdbUrl.url(QUrl::PreferLocalFile | QUrl::StripTrailingSlash);
    }

    QStringList arguments = extraArguments;
    arguments << QStringLiteral("--interpreter=mi2") << QStringLiteral("-quiet");

    QUrl shell = config.readEntry(Config::DebuggerShellEntry, QUrl());
    if(!shell.isEmpty()) {
        qCDebug(DEBUGGERGDB) << "have shell" << shell;
        QString shell_without_args = shell.toLocalFile().split(QChar(' ')).first();

        QFileInfo info(shell_without_args);
        /*if( info.isRelative() )
        {
            shell_without_args = build_dir + "/" + shell_without_args;
            info.setFile( shell_without_args );
        }*/
        if(!info.exists()) {
            KMessageBox::information(
                qApp->activeWindow(),
                i18n("Could not locate the debugging shell '%1'.", shell_without_args ),
                i18n("Debugging Shell Not Found") );
            return false;
        }

        arguments.insert(0, m_debuggerExecutable);
        arguments.insert(0, shell.toLocalFile());
        m_process->setShellCommand(KShell::joinArgs(arguments));
    } else {
        m_process->setProgram(m_debuggerExecutable, arguments);
    }

    m_process->start();

    qCDebug(DEBUGGERGDB) << "Starting GDB with command" << shell.toLocalFile() + QLatin1Char(' ') + m_debuggerExecutable
                           + QLatin1Char(' ') + arguments.join(QLatin1Char(' '));
    qCDebug(DEBUGGERGDB) << "GDB process pid:" << m_process->pid();
    emit userCommandOutput(shell.toLocalFile() + QLatin1Char(' ') + m_debuggerExecutable
                           + QLatin1Char(' ') + arguments.join(QLatin1Char(' ')) + QLatin1Char('\n'));
    return true;
}
