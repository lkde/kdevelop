/***************************************************************************
 *   Copyright 2007 Robert Gruber <rgruber@users.sourceforge.net>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cvsplugin.h"

#include <QAction>
#include <QMenu>
#include <QFileInfo>
#include <QDir>

#include <KParts/PartManager>
#include <KParts/Part>
#include <KParts/ReadOnlyPart>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KActionCollection>
#include <KMessageBox>
#include <KAboutData>
#include <KLocalizedString>

#include <interfaces/iproject.h>
#include <project/projectmodel.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/icore.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/ipartcontroller.h>
#include <interfaces/context.h>

#include <interfaces/contextmenuextension.h>

#include "cvsmainview.h"
#include "cvsproxy.h"
#include "cvsjob.h"
#include "editorsview.h"
#include "commitdialog.h"
#include "cvsgenericoutputview.h"
#include "checkoutdialog.h"
#include "importdialog.h"
#include "importmetadatawidget.h"
#include "debug.h"
#include <language/interfaces/editorcontext.h>
#include <vcs/vcspluginhelper.h>
#include <vcs/widgets/standardvcslocationwidget.h>

K_PLUGIN_FACTORY_WITH_JSON(KDevCvsFactory, "kdevcvs.json", registerPlugin<CvsPlugin>();)
// K_EXPORT_PLUGIN(KDevCvsFactory(KAboutData("kdevcvs", "kdevcvs", ki18n("CVS"), "0.1", ki18n("Support for CVS version control system"), KAboutData::License_GPL)))

class KDevCvsViewFactory: public KDevelop::IToolViewFactory
{
public:
    explicit KDevCvsViewFactory(CvsPlugin *plugin): m_plugin(plugin) {}
    QWidget* create(QWidget *parent = nullptr) override {
        return new CvsMainView(m_plugin, parent);
    }
    Qt::DockWidgetArea defaultPosition() override {
        return Qt::BottomDockWidgetArea;
    }
    QString id() const override {
        return QStringLiteral("org.kdevelop.CVSView");
    }

private:
    CvsPlugin *m_plugin;
};

class CvsPluginPrivate
{
public:
    explicit CvsPluginPrivate(CvsPlugin *pThis)
            : m_factory(new KDevCvsViewFactory(pThis))
            , m_proxy(new CvsProxy(pThis))
            , m_common(new KDevelop::VcsPluginHelper(pThis, pThis)) {}

    KDevCvsViewFactory* m_factory;
    QPointer<CvsProxy> m_proxy;
    QScopedPointer<KDevelop::VcsPluginHelper> m_common;
};

CvsPlugin::CvsPlugin(QObject *parent, const QVariantList &)
        : KDevelop::IPlugin(QStringLiteral("kdevcvs"), parent)
        , d(new CvsPluginPrivate(this))
{
    core()->uiController()->addToolView(i18n("CVS"), d->m_factory);

    setXMLFile(QStringLiteral("kdevcvs.rc"));
    setupActions();
}

CvsPlugin::~CvsPlugin()
{
}


void CvsPlugin::unload()
{
    core()->uiController()->removeToolView( d->m_factory );
}


CvsProxy* CvsPlugin::proxy()
{
    return d->m_proxy;
}

void CvsPlugin::setupActions()
{
    QAction *action;

    action = actionCollection()->addAction(QStringLiteral("cvs_import"));
    action->setText(i18n("Import Directory..."));
    connect(action, &QAction::triggered, this, &CvsPlugin::slotImport);

    action = actionCollection()->addAction(QStringLiteral("cvs_checkout"));
    action->setText(i18n("Checkout..."));
    connect(action, &QAction::triggered, this, &CvsPlugin::slotCheckout);

    action = actionCollection()->addAction(QStringLiteral("cvs_status"));
    action->setText(i18n("Status..."));
    connect(action, &QAction::triggered, this, &CvsPlugin::slotStatus);
}

const QUrl CvsPlugin::urlFocusedDocument() const
{
    KParts::ReadOnlyPart *plugin =
        dynamic_cast<KParts::ReadOnlyPart*>(core()->partController()->activePart());
    if (plugin) {
        if (plugin->url().isLocalFile()) {
            return plugin->url();
        }
    }
    return QUrl();
}


void CvsPlugin::slotImport()
{
    QUrl url = urlFocusedDocument();

    ImportDialog dlg(this, url);
    dlg.exec();
}

void CvsPlugin::slotCheckout()
{
    ///@todo don't use proxy directly; use interface instead

    CheckoutDialog dlg(this);

    dlg.exec();
}

void CvsPlugin::slotStatus()
{
    QUrl url = urlFocusedDocument();
    QList<QUrl> urls;
    urls << url;

    KDevelop::VcsJob* j = status(urls, KDevelop::IBasicVersionControl::Recursive);
    CvsJob* job = dynamic_cast<CvsJob*>(j);
    if (job) {
        CvsGenericOutputView* view = new CvsGenericOutputView(job);
        emit addNewTabToMainView(view, i18n("Status"));
        KDevelop::ICore::self()->runController()->registerJob(job);
    }
}


KDevelop::ContextMenuExtension CvsPlugin::contextMenuExtension(KDevelop::Context* context, QWidget* parent)
{
    d->m_common->setupFromContext(context);
    QList<QUrl> const & ctxUrlList = d->m_common->contextUrlList();

    bool hasVersionControlledEntries = false;
    foreach(const QUrl &url, ctxUrlList) {
        if (d->m_proxy->isValidDirectory(url)) {
            hasVersionControlledEntries = true;
            break;
        }
    }

    qCDebug(PLUGIN_CVS) << "version controlled?" << hasVersionControlledEntries;

    if (!hasVersionControlledEntries)
        return IPlugin::contextMenuExtension(context, parent);

    QMenu* menu = d->m_common->commonActions(parent);
    menu->addSeparator();

    QAction *action;
    // Just add actions which are not covered by the cvscommon plugin
    action = new QAction(i18n("Edit"), menu);
    connect(action, &QAction::triggered, this, &CvsPlugin::ctxEdit);
    menu->addAction(action);

    action = new QAction(i18n("Unedit"), menu);
    connect(action, &QAction::triggered, this, &CvsPlugin::ctxUnEdit);
    menu->addAction(action);

    action = new QAction(i18n("Show Editors"), menu);
    connect(action, &QAction::triggered, this, &CvsPlugin::ctxEditors);
    menu->addAction(action);

    KDevelop::ContextMenuExtension menuExt;
    menuExt.addAction(KDevelop::ContextMenuExtension::VcsGroup, menu->menuAction());

    return menuExt;
}

void CvsPlugin::ctxEdit()
{
    QList<QUrl> const & urls = d->m_common->contextUrlList();
    Q_ASSERT(!urls.empty());

    KDevelop::VcsJob* j = edit(urls.front());
    CvsJob* job = dynamic_cast<CvsJob*>(j);
    if (job) {
        connect(job, &CvsJob::result,
                this, &CvsPlugin::jobFinished);
        KDevelop::ICore::self()->runController()->registerJob(job);
    }
}

void CvsPlugin::ctxUnEdit()
{
    QList<QUrl> const & urls = d->m_common->contextUrlList();
    Q_ASSERT(!urls.empty());

    KDevelop::VcsJob* j = unedit(urls.front());
    CvsJob* job = dynamic_cast<CvsJob*>(j);
    if (job) {
        connect(job, &CvsJob::result,
                this, &CvsPlugin::jobFinished);
        KDevelop::ICore::self()->runController()->registerJob(job);
    }
}

void CvsPlugin::ctxEditors()
{
    QList<QUrl> const & urls = d->m_common->contextUrlList();
    Q_ASSERT(!urls.empty());

    CvsJob* job = d->m_proxy->editors(findWorkingDir(urls.front()),
                                      urls);
    if (job) {
        KDevelop::ICore::self()->runController()->registerJob(job);
        EditorsView* view = new EditorsView(job);
        emit addNewTabToMainView(view, i18n("Editors"));
    }
}

QString CvsPlugin::findWorkingDir(const QUrl& location)
{
    QFileInfo fileInfo(location.toLocalFile());

    // find out correct working directory
    if (fileInfo.isFile()) {
        return fileInfo.absolutePath();
    } else {
        return fileInfo.absoluteFilePath();
    }
}


// Begin:  KDevelop::IBasicVersionControl

bool CvsPlugin::isValidRemoteRepositoryUrl(const QUrl& remoteLocation)
{
    Q_UNUSED(remoteLocation);
    // TODO
    return false;
}

bool CvsPlugin::isVersionControlled(const QUrl & localLocation)
{
    return d->m_proxy->isVersionControlled(localLocation);
}

KDevelop::VcsJob * CvsPlugin::repositoryLocation(const QUrl & localLocation)
{
    Q_UNUSED(localLocation);
    return nullptr;
}

KDevelop::VcsJob * CvsPlugin::add(const QList<QUrl> & localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    CvsJob* job = d->m_proxy->add(findWorkingDir(localLocations[0]),
                                  localLocations,
                                  (recursion == KDevelop::IBasicVersionControl::Recursive) ? true : false);
    return job;
}

KDevelop::VcsJob * CvsPlugin::remove(const QList<QUrl> & localLocations)
{
    CvsJob* job = d->m_proxy->remove(findWorkingDir(localLocations[0]),
                                     localLocations);
    return job;
}

KDevelop::VcsJob * CvsPlugin::localRevision(const QUrl & localLocation, KDevelop::VcsRevision::RevisionType)
{
    Q_UNUSED(localLocation)
    return nullptr;
}

KDevelop::VcsJob * CvsPlugin::status(const QList<QUrl> & localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    CvsJob* job = d->m_proxy->status(findWorkingDir(localLocations[0]),
                                     localLocations,
                                     (recursion == KDevelop::IBasicVersionControl::Recursive) ? true : false);
    return job;
}

KDevelop::VcsJob * CvsPlugin::unedit(const QUrl& localLocation)
{
    CvsJob* job = d->m_proxy->unedit(findWorkingDir(localLocation),
                                     QList<QUrl>() << localLocation);
    return job;
}

KDevelop::VcsJob * CvsPlugin::edit(const QUrl& localLocation)
{
    CvsJob* job = d->m_proxy->edit(findWorkingDir(localLocation),
                                   QList<QUrl>() << localLocation);
    return job;
}

KDevelop::VcsJob * CvsPlugin::copy(const QUrl & localLocationSrc, const QUrl & localLocationDstn)
{
    bool ok = QFile::copy(localLocationSrc.toLocalFile(), localLocationDstn.path());
    if (!ok) {
        return nullptr;
    }

    QList<QUrl> listDstn;
    listDstn << localLocationDstn;

    CvsJob* job = d->m_proxy->add(findWorkingDir(localLocationDstn),
                                  listDstn, true);

    return job;
}

KDevelop::VcsJob * CvsPlugin::move(const QUrl &, const QUrl &)
{
    return nullptr;
}

KDevelop::VcsJob * CvsPlugin::revert(const QList<QUrl> & localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    KDevelop::VcsRevision rev;
    CvsJob* job = d->m_proxy->update(findWorkingDir(localLocations[0]),
                                     localLocations,
                                     rev,
                                     QStringLiteral("-C"),
                                     (recursion == KDevelop::IBasicVersionControl::Recursive) ? true : false,
                                     false, false);
    return job;
}

KDevelop::VcsJob * CvsPlugin::update(const QList<QUrl> & localLocations, const KDevelop::VcsRevision & rev, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    CvsJob* job = d->m_proxy->update(findWorkingDir(localLocations[0]),
                                     localLocations,
                                     rev,
                                     QString(),
                                     (recursion == KDevelop::IBasicVersionControl::Recursive) ? true : false,
                                     false, false);
    return job;
}

KDevelop::VcsJob * CvsPlugin::commit(const QString & message, const QList<QUrl> & localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    Q_UNUSED(recursion);
    QString msg = message;
    if (msg.isEmpty()) {
        CommitDialog dlg;
        if (dlg.exec() == QDialog::Accepted) {
            msg = dlg.message();
        }
    }

    CvsJob* job = d->m_proxy->commit(findWorkingDir(localLocations[0]),
                                     localLocations,
                                     msg);
    return job;
}

KDevelop::VcsJob * CvsPlugin::diff(const QUrl & fileOrDirectory, const KDevelop::VcsRevision & srcRevision, const KDevelop::VcsRevision & dstRevision, KDevelop::VcsDiff::Type, KDevelop::IBasicVersionControl::RecursionMode)
{
    CvsJob* job = d->m_proxy->diff(fileOrDirectory, srcRevision, dstRevision, QStringLiteral("-uN")/*always unified*/);
    return job;
}

KDevelop::VcsJob * CvsPlugin::log(const QUrl & localLocation, const KDevelop::VcsRevision & rev, unsigned long limit)
{
    Q_UNUSED(limit)

    CvsJob* job = d->m_proxy->log(localLocation, rev);
    return job;
}

KDevelop::VcsJob * CvsPlugin::log(const QUrl & localLocation, const KDevelop::VcsRevision & rev, const KDevelop::VcsRevision & limit)
{
    Q_UNUSED(limit)
    return log(localLocation, rev, 0);
}

KDevelop::VcsJob * CvsPlugin::annotate(const QUrl & localLocation, const KDevelop::VcsRevision & rev)
{
    CvsJob* job = d->m_proxy->annotate(localLocation, rev);
    return job;
}

KDevelop::VcsJob * CvsPlugin::resolve(const QList<QUrl> & localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    Q_UNUSED(localLocations);
    Q_UNUSED(recursion);
    return nullptr;
}

KDevelop::VcsJob * CvsPlugin::import(const QString& commitMessage, const QUrl& sourceDirectory, const KDevelop::VcsLocation& destinationRepository)
{
    if (commitMessage.isEmpty()
            || !sourceDirectory.isLocalFile()
            || !destinationRepository.isValid()
            || destinationRepository.type() != KDevelop::VcsLocation::RepositoryLocation) {
        return nullptr;
    }

    qCDebug(PLUGIN_CVS) << "CVS Import requested "
    << "src:" << sourceDirectory.toLocalFile()
    << "srv:" << destinationRepository.repositoryServer()
    << "module:" << destinationRepository.repositoryModule();

    CvsJob* job = d->m_proxy->import(sourceDirectory,
                                     destinationRepository.repositoryServer(),
                                     destinationRepository.repositoryModule(),
                                     destinationRepository.userData().toString(),
                                     destinationRepository.repositoryTag(),
                                     commitMessage);
    return job;
}

KDevelop::VcsJob * CvsPlugin::createWorkingCopy(const KDevelop::VcsLocation & sourceRepository, const QUrl & destinationDirectory, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    Q_UNUSED(recursion);
    if (!destinationDirectory.isLocalFile()
            || !sourceRepository.isValid()
            || sourceRepository.type() != KDevelop::VcsLocation::RepositoryLocation) {
        return nullptr;
    }

    qCDebug(PLUGIN_CVS) << "CVS Checkout requested "
    << "dest:" << destinationDirectory.toLocalFile()
    << "srv:" << sourceRepository.repositoryServer()
    << "module:" << sourceRepository.repositoryModule()
    << "branch:" << sourceRepository.repositoryBranch() << endl;

    CvsJob* job = d->m_proxy->checkout(destinationDirectory,
                                       sourceRepository.repositoryServer(),
                                       sourceRepository.repositoryModule(),
                                       QString(),
                                       sourceRepository.repositoryBranch(),
                                       true, true);
    return job;
}

QString CvsPlugin::name() const
{
    return i18n("CVS");
}

KDevelop::VcsImportMetadataWidget* CvsPlugin::createImportMetadataWidget(QWidget* parent)
{
    return new ImportMetadataWidget(parent);
}

KDevelop::VcsLocationWidget* CvsPlugin::vcsLocation(QWidget* parent) const
{
    return new KDevelop::StandardVcsLocationWidget(parent);
}

// End:  KDevelop::IBasicVersionControl

#include "cvsplugin.moc"
