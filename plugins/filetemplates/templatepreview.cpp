/*
 * This file is part of KDevelop
 * Copyright 2012 Milian Wolff <mail@milianw.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "templatepreview.h"
#include "ui_templatepreview.h"

#include <language/codegen/templaterenderer.h>
#include <language/codegen/codedescription.h>
#include <language/codegen/sourcefiletemplate.h>
#include <language/codegen/documentchangeset.h>

#include <QDir>
#include <QVBoxLayout>

#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/View>
#include <KTextEditor/ConfigInterface>

#include <KTempDir>
#include <kmacroexpander.h>

using namespace KDevelop;

TemplatePreview::TemplatePreview(QWidget* parent, Qt::WindowFlags f)
: QWidget(parent, f)
, m_renderer(new TemplateRenderer)
{
    QVariantHash vars;
    vars["name"] = "Example";
    vars["license"] = "This file is licensed under the ExampleLicense 3.0";
    // TODO: More variables, preferably the ones from TemplateClassGenerator

    VariableDescriptionList members;
    members << VariableDescription("int", "number");
    members << VariableDescription("string", "name");
    vars["members"] = CodeDescription::toVariantList(members);

    FunctionDescriptionList functions;
    functions << FunctionDescription("doSomething", VariableDescriptionList(), VariableDescriptionList());
    FunctionDescription complexFunction("doSomethingElse", VariableDescriptionList(), VariableDescriptionList());
    complexFunction.arguments << VariableDescription("bool", "really");
    complexFunction.arguments << VariableDescription("int", "howMuch");
    complexFunction.returnArguments << VariableDescription("double", QString());
    functions << complexFunction;
    vars["functions"] = CodeDescription::toVariantList(functions);

    m_renderer->addVariables(vars);

    m_variables["APPNAME"] = "Example";
    m_variables["APPNAMELC"] = "example";
    m_variables["APPNAMEUC"] = "EXAMPLE";
    m_variables["APPNAMEID"] = "Example";

    m_variables["PROJECTDIR"] = QDir::homePath() + "/projects/ExampleProjectDir";
    m_variables["PROJECTDIRNAME"] = "ExampleProjectDir";
    m_variables["VERSIONCONTROLPLUGIN"] = "kdevgit";

    KTextEditor::Editor* editor = KTextEditor::EditorChooser::editor();
    m_preview.reset(editor->createDocument(this));
    m_preview->setReadWrite(false);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    m_view = m_preview->createView(this);
    if (KTextEditor::ConfigInterface* config = qobject_cast<KTextEditor::ConfigInterface*>(m_view)) {
        config->setConfigValue("icon-bar", false);
        config->setConfigValue("folding-bar", false);
        config->setConfigValue("line-numbers", false);
        config->setConfigValue("dynamic-word-wrap", true);
    }
    layout->addWidget(m_view);
}

TemplatePreview::~TemplatePreview()
{

}

QString TemplatePreview::setText(const QString& text, bool isProject)
{
    QString rendered;
    QString errorString;

    if (!text.isEmpty()) {
        if (isProject) {
            rendered = KMacroExpander::expandMacros(text, m_variables);
        } else {
            rendered = m_renderer->render(text);
            errorString = m_renderer->errorString();
        }
    }

    m_preview->setReadWrite(true);
    m_preview->setText(rendered);
    m_view->setCursorPosition(KTextEditor::Cursor(0, 0));
    m_preview->setReadWrite(false);

    return errorString;
}

QString TemplatePreview::setFileTemplate(const QString& file)
{
    SourceFileTemplate fileTemplate(file);
    if (!fileTemplate.isValid() || fileTemplate.outputFiles().isEmpty()) {
        return i18n("invalid file template: %1", file);
    }

    KTempDir dir;
    KUrl base(dir.name());
    QHash<QString, KUrl> fileUrls;
    foreach(const SourceFileTemplate::OutputFile& out, fileTemplate.outputFiles()) {
        KUrl url(base);
        url.addPath(out.outputName);
        fileUrls.insert(out.identifier, url);
    }
    m_renderer->setEmptyLinesPolicy(TemplateRenderer::TrimEmptyLines);
    DocumentChangeSet changes = m_renderer->renderFileTemplate(fileTemplate, base, fileUrls);
    changes.setActivationPolicy(DocumentChangeSet::DoNotActivate);
    changes.setUpdateHandling(DocumentChangeSet::NoUpdate);
    DocumentChangeSet::ChangeResult result = changes.applyAllChanges();
    if (!result) {
        return result.m_failureReason;
    }

    m_preview->openUrl(fileUrls[fileTemplate.outputFiles().first().identifier]);
    return QString();
}

KTextEditor::Document* TemplatePreview::document() const
{
    return m_preview.data();
}

TemplateRenderer* TemplatePreview::renderer() const
{
    return m_renderer.data();
}
