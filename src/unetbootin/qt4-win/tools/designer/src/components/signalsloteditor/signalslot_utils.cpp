/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "signalslot_utils_p.h"

#include <qdesigner_membersheet_p.h>
#include <widgetdatabase_p.h>
#include <metadatabase_p.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerLanguageExtension>

#include <QtCore/QPair>

QT_BEGIN_NAMESPACE

typedef QPair<QString, QString> ClassNameSignaturePair;

// Find all member functions that match a predicate on the signature string
// using the member sheet and the fake methods stored in the widget
// database and the meta data base.
// Assign a pair of <classname,  signature> to OutputIterator.

template <class SignaturePredicate, class OutputIterator>
static void memberList(QDesignerFormEditorInterface *core,
                       QObject *object,
                       qdesigner_internal::MemberType member_type,
                       bool showAll,
                       SignaturePredicate predicate,
                       OutputIterator it)
{
    if (!object)
        return;

    // 1) member sheet
    const QDesignerMemberSheetExtension *members = qt_extension<QDesignerMemberSheetExtension*>(core->extensionManager(), object);
    Q_ASSERT(members != 0);
    const int count = members->count();
    for (int i = 0; i < count; ++i) {
        if (!members->isVisible(i))
            continue;

        if (member_type == qdesigner_internal::SignalMember && !members->isSignal(i))
            continue;

        if (member_type == qdesigner_internal::SlotMember && !members->isSlot(i))
            continue;

        if (!showAll && members->inheritedFromWidget(i))
            continue;

        const QString signature = members->signature(i);
        if (predicate(signature)) {
            *it = ClassNameSignaturePair(members->declaredInClass(i), signature);
            ++it;
        }
    }
    // 2) fake slots from widget DB
    const qdesigner_internal::WidgetDataBase *wdb = qobject_cast<qdesigner_internal::WidgetDataBase *>(core->widgetDataBase());
    if (!wdb)
        return;
    const int idx = wdb->indexOfObject(object);
    Q_ASSERT(idx != -1);
    // get the promoted class name
    const qdesigner_internal::WidgetDataBaseItem *wdbItem = static_cast<qdesigner_internal::WidgetDataBaseItem *>(wdb->item(idx));
    const QString className = wdbItem->name();

    const QStringList wdbFakeMethods = member_type == qdesigner_internal::SlotMember ? wdbItem->fakeSlots() : wdbItem->fakeSignals();
    if (!wdbFakeMethods.empty())
        foreach (const QString &fakeMethod, wdbFakeMethods)
            if (predicate(fakeMethod)) {
                *it = ClassNameSignaturePair(className, fakeMethod);
                ++it;
            }
    // 3) fake slots from meta DB
    qdesigner_internal::MetaDataBase *metaDataBase = qobject_cast<qdesigner_internal::MetaDataBase *>(core->metaDataBase());
    if (!metaDataBase)
        return;

    const qdesigner_internal::MetaDataBaseItem *mdbItem = metaDataBase->metaDataBaseItem(object);
    Q_ASSERT(mdbItem);
    const QStringList mdbFakeMethods =  member_type == qdesigner_internal::SlotMember ? mdbItem->fakeSlots() : mdbItem->fakeSignals();
    if (!mdbFakeMethods.empty())
        foreach (const QString &fakeMethod, mdbFakeMethods)
            if (predicate(fakeMethod)) {
                *it = ClassNameSignaturePair(className, fakeMethod);
                ++it;
            }
}

namespace {
    // Predicate that matches the exact signature string
    class EqualsPredicate {
    public:
        EqualsPredicate(const QString &pattern) : m_pattern(pattern) {}
        bool operator()(const QString &s) const { return s == m_pattern; }
    private:
        const QString  m_pattern;
    };
    // Predicate for a QString member signature that matches signals up with slots and vice versa
    class SignalMatchesSlotPredicate {
    public:
        SignalMatchesSlotPredicate(QDesignerFormEditorInterface *core, const QString &peer, qdesigner_internal::MemberType memberType);
        bool operator()(const QString &s) const;

    private:
        bool signalMatchesSlot(const QString &signal, const QString &slot) const;

        const QString  m_peer;
        qdesigner_internal::MemberType m_memberType;
        const QDesignerLanguageExtension *m_lang;
    };

    SignalMatchesSlotPredicate::SignalMatchesSlotPredicate(QDesignerFormEditorInterface *core, const QString &peer, qdesigner_internal::MemberType memberType) :
        m_peer(peer),
        m_memberType(memberType),
        m_lang(qt_extension<QDesignerLanguageExtension*>(core->extensionManager(), core))
    {
    }

    bool SignalMatchesSlotPredicate::operator()(const QString &s) const
    {
        return m_memberType == qdesigner_internal::SlotMember ? signalMatchesSlot(m_peer, s) :  signalMatchesSlot(s, m_peer);
    }

    bool SignalMatchesSlotPredicate::signalMatchesSlot(const QString &signal, const QString &slot) const
    {
        if (m_lang)
            return m_lang->signalMatchesSlot(signal, slot);

        return QDesignerMemberSheet::signalMatchesSlot(signal, slot);
    }

    // Output iterator for a pair of pair of <classname,  signature>
    // that builds the reverse class list for reverseClassesMemberFunctions()
    // (for the combos of the ToolWindow)
    class ReverseClassesMemberIterator {
    public:
        ReverseClassesMemberIterator(qdesigner_internal::ClassesMemberFunctions *result);

        ReverseClassesMemberIterator &operator*()     { return *this; }
        ReverseClassesMemberIterator &operator++(int) { return *this; }
        ReverseClassesMemberIterator &operator++()    { return *this; }
        void operator=(const ClassNameSignaturePair &classNameSignature);

    private:
        qdesigner_internal::ClassesMemberFunctions *m_result;
        QString m_lastClassName;
        QStringList *m_memberList;
    };

    ReverseClassesMemberIterator::ReverseClassesMemberIterator(qdesigner_internal::ClassesMemberFunctions *result) :
       m_result(result),
       m_memberList(0)
    {
    }

    void ReverseClassesMemberIterator::operator=(const ClassNameSignaturePair &classNameSignature)
    {
        // prepend a new entry if class changes
        if (!m_memberList || classNameSignature.first != m_lastClassName) {
            m_lastClassName = classNameSignature.first;
            m_result->push_front(qdesigner_internal::ClassMemberFunctions(m_lastClassName));
            m_memberList = &(m_result->front().m_memberList);
        }
        m_memberList->push_back(classNameSignature.second);
    }

    // Output iterator for a pair of pair of <classname,  signature>
    // that adds the signatures to a string list
    class SignatureIterator {
    public:
        SignatureIterator(QMap<QString, QString> *result) : m_result(result) {}

        SignatureIterator &operator*()     { return *this; }
        SignatureIterator &operator++(int) { return *this; }
        SignatureIterator &operator++()    { return *this; }
        void operator=(const ClassNameSignaturePair &classNameSignature) {
            m_result->insert(classNameSignature.second, classNameSignature.first);
        }

    private:
        QMap<QString, QString> *m_result;
    };

    class Qt3Members
      {
      public:
          static Qt3Members *instance();
          QMap<QString, QStringList> getSignals() const { return m_classNameToSignals; }
          QMap<QString, QStringList> getSlots() const { return m_classNameToSlots; }
      private:
          Qt3Members();
          static Qt3Members *m_instance;
          QMap<QString, QStringList> m_classNameToSignals;
          QMap<QString, QStringList> m_classNameToSlots;
      };

    Qt3Members *Qt3Members::m_instance = 0;

    Qt3Members::Qt3Members()
    {
        m_classNameToSignals[QLatin1String("QTextEdit")].append(QLatin1String("currentFontChanged(QFont)"));
        m_classNameToSignals[QLatin1String("QTextEdit")].append(QLatin1String("currentColorChanged(QColor)"));
        m_classNameToSignals[QLatin1String("QTabWidget")].append(QLatin1String("currentChanged(QWidget*)"));
        m_classNameToSignals[QLatin1String("QTabWidget")].append(QLatin1String("selected(QString)"));
        m_classNameToSignals[QLatin1String("QTabBar")].append(QLatin1String("selected(int)"));
        m_classNameToSignals[QLatin1String("QMenuBar")].append(QLatin1String("activated(int)"));
        m_classNameToSignals[QLatin1String("QMenuBar")].append(QLatin1String("highlighted(int)"));
        m_classNameToSignals[QLatin1String("QMenu")].append(QLatin1String("activated(int)"));
        m_classNameToSignals[QLatin1String("QMenu")].append(QLatin1String("highlighted(int)"));
        m_classNameToSignals[QLatin1String("QLineEdit")].append(QLatin1String("lostFocus()"));
        m_classNameToSignals[QLatin1String("QDial")].append(QLatin1String("dialPressed()"));
        m_classNameToSignals[QLatin1String("QDial")].append(QLatin1String("dialMoved(int)"));
        m_classNameToSignals[QLatin1String("QDial")].append(QLatin1String("dialReleased()"));
        m_classNameToSignals[QLatin1String("QComboBox")].append(QLatin1String("textChanged(QString)"));
        m_classNameToSignals[QLatin1String("QActionGroup")].append(QLatin1String("selected(QAction*)"));
        m_classNameToSignals[QLatin1String("QAction")].append(QLatin1String("activated(int)"));
        m_classNameToSignals[QLatin1String("QAbstractSocket")].append(QLatin1String("connectionClosed()"));
        m_classNameToSignals[QLatin1String("QAbstractSocket")].append(QLatin1String("delayedCloseFinished()"));

        m_classNameToSlots[QLatin1String("QWidget")].append(QLatin1String("setShown(bool)"));
        m_classNameToSlots[QLatin1String("QToolButton")].append(QLatin1String("setTextPosition(QToolButton::TextPosition)"));
        m_classNameToSlots[QLatin1String("QToolButton")].append(QLatin1String("setUsesBigPixmap(bool)"));
        m_classNameToSlots[QLatin1String("QToolButton")].append(QLatin1String("setUsesTextLabel(bool)"));
        m_classNameToSlots[QLatin1String("QTextEdit")].append(QLatin1String("setModified(bool)"));
        m_classNameToSlots[QLatin1String("QTextEdit")].append(QLatin1String("setColor(QColor)"));
        m_classNameToSlots[QLatin1String("QTabWidget")].append(QLatin1String("setCurrentPage(int)"));
        m_classNameToSlots[QLatin1String("QTabWidget")].append(QLatin1String("showPage(QWidget*)"));
        m_classNameToSlots[QLatin1String("QTabWidget")].append(QLatin1String("removePage(QWidget*)"));
        m_classNameToSlots[QLatin1String("QTabBar")].append(QLatin1String("setCurrentTab(int)"));
        m_classNameToSlots[QLatin1String("QStatusBar")].append(QLatin1String("message(QString,int)"));
        m_classNameToSlots[QLatin1String("QStatusBar")].append(QLatin1String("clear()"));
        m_classNameToSlots[QLatin1String("QSplashScreen")].append(QLatin1String("message(QString,int)"));
        m_classNameToSlots[QLatin1String("QSplashScreen")].append(QLatin1String("clear()"));
        m_classNameToSlots[QLatin1String("QSlider")].append(QLatin1String("addStep()"));
        m_classNameToSlots[QLatin1String("QSlider")].append(QLatin1String("subtractStep()"));
        m_classNameToSlots[QLatin1String("QAbstractButton")].append(QLatin1String("setOn(bool)"));
        m_classNameToSlots[QLatin1String("QAction")].append(QLatin1String("setOn(bool)"));
        m_classNameToSlots[QLatin1String("QErrorMessage")].append(QLatin1String("message(QString)"));
        m_classNameToSlots[QLatin1String("QTimer")].append(QLatin1String("changeInterval(int)"));
        m_classNameToSlots[QLatin1String("QTimer")].append(QLatin1String("start(int,bool)"));
    }

    Qt3Members *Qt3Members::instance()
    {
        if (!m_instance)
            m_instance = new Qt3Members();
        return m_instance;
    }

}

static inline bool truePredicate(const QString &) { return true; }

namespace qdesigner_internal {

    ClassMemberFunctions::ClassMemberFunctions(const QString &class_name) :
        m_className(class_name)
    {
    }

    bool signalMatchesSlot(QDesignerFormEditorInterface *core, const QString &signal, const QString &slot)
    {
        const SignalMatchesSlotPredicate predicate(core, signal, qdesigner_internal::SlotMember);
        return predicate(slot);
    }

    // return classes and members in reverse class order to
    // populate of the combo of the ToolWindow
    ClassesMemberFunctions reverseClassesMemberFunctions(const QString &obj_name, MemberType member_type,
                                                         const QString &peer, QDesignerFormWindowInterface *form)
    {
        QObject *object = qFindChild<QObject*>(form, obj_name);
        if (!object)
            return ClassesMemberFunctions();

        QDesignerFormEditorInterface *core = form->core();

        ClassesMemberFunctions rc;
        memberList(form->core(), object, member_type, true, SignalMatchesSlotPredicate(core, peer,  member_type),
                   ReverseClassesMemberIterator(&rc));
        return rc;
    }

    QMap<QString, QString> getSignals(QDesignerFormEditorInterface *core, QObject *object, bool showAll)
    {
        QMap<QString, QString> rc;
        memberList(core, object, SignalMember, showAll, truePredicate, SignatureIterator(&rc));
        return rc;
    }

    bool isQt3Signal(const QString &className, const QString &signalSignature)
    {
        QMap<QString, QStringList> qt3signals = Qt3Members::instance()->getSignals();
        QMap<QString, QStringList>::const_iterator it = qt3signals.constFind(className);
        if (it != qt3signals.constEnd() && (*it).contains(signalSignature))
            return true;
        return false;
    }
    bool isQt3Slot(const QString &className, const QString &slotSignature)
    {
        QMap<QString, QStringList> qt3slots = Qt3Members::instance()->getSlots();
        QMap<QString, QStringList>::const_iterator it = qt3slots.constFind(className);
        if (it != qt3slots.constEnd() && (*it).contains(slotSignature))
            return true;
        return false;
    }

    QMap<QString, QString> getMatchingSlots(QDesignerFormEditorInterface *core, QObject *object, const QString &signalSignature, bool showAll)
    {
        QMap<QString, QString> rc;
        memberList(core, object, SlotMember, showAll, SignalMatchesSlotPredicate(core, signalSignature,  qdesigner_internal::SlotMember), SignatureIterator(&rc));
        return rc;
    }

    bool memberFunctionListContains(QDesignerFormEditorInterface *core, QObject *object, MemberType type, const QString &signature)
    {
        QMap<QString, QString> rc;
        memberList(core, object, type, true, EqualsPredicate(signature), SignatureIterator(&rc));
        return !rc.empty();
    }

    // ### deprecated
    QString realObjectName(QDesignerFormEditorInterface *core, QObject *object)
    {
        if (!object)
        return QString();

        const QDesignerMetaDataBaseInterface *mdb = core->metaDataBase();
        if (const QDesignerMetaDataBaseItemInterface *item = mdb->item(object))
            return item->name();

        return object->objectName();
    }
} // namespace qdesigner_internal

QT_END_NAMESPACE
