/***************************************************************************
                          cppcodecompletion.h  -  description
                             -------------------
    begin                : Sat Jul 21 2001
    copyright            : (C) 2001 by Victor R�der
    email                : victor_roeder@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __CPPCODECOMPLETION_H__
#define __CPPCODECOMPLETION_H__

#include <qobject.h>
#include <qstringlist.h>

#include "keditor/editor.h"
#include "keditor/edit_iface.h"
#include "keditor/cursor_iface.h"
#include "keditor/codecompletion_iface.h"

#include "persistantclassstore.h"
/*#include "parsedclass.h"
#include "parsedmethod.h"*/

#include "kdevcore.h"

#include "cppccparser.h"
#include "cppsupportpart.h"
#include "simpleparser.h"

class KTempFile;

class CppCodeCompletion : public QObject
{
    Q_OBJECT

public:
    CppCodeCompletion ( CppSupportPart* part, ClassStore* pStore, ClassStore* pCCStore );
    void setEnableCodeCompletion( bool setEnable );
    virtual ~CppCodeCompletion();

public slots:
    void expandText();
    void completeText();
    void typeOf();
    QString typingTypeOf( int, int );

protected slots:
    void slotDocumentActivated ( KEditor::Document* pDoc );
    void slotArgHintHided();
    void slotCompletionBoxHided( /* int completionTextLine */ );
    void slotTextChanged( KEditor::Document *pDoc, int nLine, int nCol );
    void slotTextChangedRoberto( KEditor::Document *pDoc, int nLine, int nCol );
    void slotCursorPositionChanged( KEditor::Document *pDoc, int nLine, int nCol );

protected:
    QString evaluateExpression( const QString& expr,
                                const QValueList<SimpleVariable>& roo,
                                ClassStore* sigma );
    int expressionAt( const QString& text, int index );
    QStringList splitExpression( const QString& text );

    QString getMethodBody( int iLine, int iCol, QString* classname );
    QString getCompletionText ( int nLine, int nCol );
    QString getNodeText ( int nNode, int nLine );
    QString getNodeDelimiter ( int nNode, int nLine );
    int getNodePos ( int nLine, int nCol );

    QValueList<KEditor::CompletionEntry> getEntryListForExpr( const QString& expr, const QValueList<SimpleVariable>& vars );


    QString createTmpFileForParser (int iLine);

    QValueList<KEditor::CompletionEntry> getEntryListForClass( QString strClass );
    QValueList<KEditor::CompletionEntry> getEntryListForNamespace( const QString& strNamespace );
    QValueList<KEditor::CompletionEntry> getEntryListForStruct( const QString& strStruct );
    QValueList<KEditor::CompletionEntry> getEntryListForClassOfNamespace( QString strClass, const QString& strNamespace );

    /* methods which are called recursively by getEntryListForClass(...) */
    QList<ParsedMethod>* getParentMethodListForClass( ParsedClass* pClass, QList<ParsedMethod>* pList );
    QList<ParsedAttribute>* getParentAttributeListForClass( ParsedClass* pClass, QList<ParsedAttribute>* pList );

    QStringList getMethodListForClass( QString strClass, QString strMethod );
    void getParentMethodListForClass( ParsedClass* pClass, QString strMethod, QStringList& methodList );

private:
    KEditor::Editor* m_pEditor;
    CppSupportPart* m_pSupport;
    KDevCore* m_pCore;
    ClassStore* m_pStore;
    ClassStore* m_pCCStore;
    CppCCParser* m_pParser;
    KEditor::CursorDocumentIface* m_pCursorIface;
    KEditor::EditDocumentIface* m_pEditIface;
    KEditor::CodeCompletionDocumentIface* m_pCompletionIface;

    KTempFile* m_pTmpFile;
    QString m_currentClassName;
    bool m_bArgHintShow;
    bool m_bCompletionBoxShow;
    bool m_bCodeCompletion;

    QStringList m_CHCommentList;

    QString getTypeOfMethod( ParsedClass*, const QString& );
    QString getTypeOfAttribute( ParsedClass*, const QString& );
};

#endif
