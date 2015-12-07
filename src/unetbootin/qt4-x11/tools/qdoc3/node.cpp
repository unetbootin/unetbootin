/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  node.cpp
*/

#include "node.h"

QT_BEGIN_NAMESPACE

/*!
  \class Node
  \brief A node in a Tree.
 */

/*!
 */
Node::~Node()
{
    if (par)
        par->removeChild(this);
    if (rel)
        rel->removeRelated(this);
}

/*!
 */
void Node::setDoc(const Doc& doc, bool replace)
{
    if (!d.isEmpty() && !replace) {
        doc.location().warning(tr("Overrides a previous doc"));
        d.location().warning(tr("(The previous doc is here)"));
    }
    d = doc;
}

/*!
 */
Node::Node(Type type, InnerNode *parent, const QString& name)
    : typ(type),
      acc(Public),
      sta(Commendable),
      saf(UnspecifiedSafeness),
      par(parent),
      rel(0),
      nam(name)
{
    if (par)
        par->addChild(this);
}

/*!
 */
QString Node::url() const
{
    return u;
}

/*!
 */
void Node::setUrl(const QString &url)
{
    u = url;
}

/*!
 */
void Node::setRelates(InnerNode *pseudoParent)
{
    if (rel)
            rel->removeRelated(this);
    rel = pseudoParent;
    pseudoParent->related.append(this);
}

/*!
 */
void Node::setLink(LinkType linkType, const QString &link, const QString &desc)
{
    QPair<QString,QString> linkPair;
    linkPair.first = link;
    linkPair.second = desc;
    linkMap[linkType] = linkPair;
}

/*!
 */
Node::Status Node::inheritedStatus() const
{
    Status parentStatus = Commendable;
    if (par)
        parentStatus = par->inheritedStatus();
    return (Status)qMin((int)sta, (int)parentStatus);
}

/*!
 */
Node::ThreadSafeness Node::threadSafeness() const
{
    if (par && saf == par->inheritedThreadSafeness())
        return UnspecifiedSafeness;
    return saf;
}

/*!
 */
Node::ThreadSafeness Node::inheritedThreadSafeness() const
{
    if (par && saf == UnspecifiedSafeness)
        return par->inheritedThreadSafeness();
    return saf;
}

/*!
 */
QString Node::fileBase() const
{
    QString base = name();
    if (base.endsWith(".html"))
        base.chop(5);
    base.replace(QRegExp("[^A-Za-z0-9]+"), " ");
    base = base.trimmed();
    base.replace(" ", "-");
    return base.toLower();
}

/*!
  \class InnerNode
 */

/*!
 */
InnerNode::~InnerNode()
{
    deleteChildren();
    removeFromRelated();
}

/*!
 */
Node *InnerNode::findNode(const QString& name)
{
    Node *node = childMap.value(name);
    if (node)
        return node;
    return primaryFunctionMap.value(name);
}

/*!
 */
Node *InnerNode::findNode(const QString& name, Type type)
{
    if (type == Function) {
        return primaryFunctionMap.value(name);
    }
    else {
        Node *node = childMap.value(name);
        if (node && node->type() == type) {
            return node;
        }
        else {
            return 0;
        }
    }
}

/*!
 */
FunctionNode *InnerNode::findFunctionNode(const QString& name)
{
    return static_cast<FunctionNode *>(primaryFunctionMap.value(name));
}

/*!
 */
FunctionNode *InnerNode::findFunctionNode(const FunctionNode *clone)
{
    QMap<QString, Node *>::ConstIterator c =
	    primaryFunctionMap.find(clone->name());
    if (c != primaryFunctionMap.end()) {
	if (isSameSignature(clone, (FunctionNode *) *c)) {
	    return (FunctionNode *) *c;
	}
        else if (secondaryFunctionMap.contains(clone->name())) {
	    const NodeList& secs = secondaryFunctionMap[clone->name()];
	    NodeList::ConstIterator s = secs.begin();
	    while (s != secs.end()) {
		if (isSameSignature(clone, (FunctionNode *) *s))
		    return (FunctionNode *) *s;
		++s;
	    }
	}
    }
    return 0;
}

/*!
 */
void InnerNode::setOverload(const FunctionNode *func, bool overlode)
{
    Node *node = (Node *) func;
    Node *&primary = primaryFunctionMap[func->name()];

    if (secondaryFunctionMap.contains(func->name())) {
	NodeList& secs = secondaryFunctionMap[func->name()];
	if (overlode) {
	    if (primary == node) {
		primary = secs.first();
		secs.erase(secs.begin());
		secs.append(node);
	    }
            else {
		secs.removeAll(node);
                secs.append(node);
            }
	}
        else {
	    if (primary != node) {
		secs.removeAll(node);
		secs.prepend(primary);
		primary = node;
	    }
	}
    }
}

/*!
 */
void InnerNode::makeUndocumentedChildrenInternal()
{
    foreach (Node *child, childNodes()) {
        if (child->doc().isEmpty()) {
            child->setAccess(Node::Private);
            child->setStatus(Node::Internal);
        }
    }
}

/*!
 */
void InnerNode::normalizeOverloads()
{
    QMap<QString, Node *>::Iterator p1 = primaryFunctionMap.begin();
    while (p1 != primaryFunctionMap.end()) {
        FunctionNode *primaryFunc = (FunctionNode *) *p1;
        if (secondaryFunctionMap.contains(primaryFunc->name()) &&
            (primaryFunc->status() != Commendable ||
             primaryFunc->access() == Private)) {

	    NodeList& secs = secondaryFunctionMap[primaryFunc->name()];
	    NodeList::ConstIterator s = secs.begin();
	    while (s != secs.end()) {
		FunctionNode *secondaryFunc = (FunctionNode *) *s;

                // Any non-obsolete, non-compatibility, non-private functions
                // (i.e, visible functions) are preferable to the primary
                // function.

                if (secondaryFunc->status() == Commendable &&
                    secondaryFunc->access() != Private) {

                    *p1 = secondaryFunc;
                    int index = secondaryFunctionMap[primaryFunc->name()].indexOf(secondaryFunc);
                    secondaryFunctionMap[primaryFunc->name()].replace(index, primaryFunc);
                    break;
                }
                ++s;
            }
        }
        ++p1;
    }

    QMap<QString, Node *>::ConstIterator p = primaryFunctionMap.begin();
    while (p != primaryFunctionMap.end()) {
	FunctionNode *primaryFunc = (FunctionNode *) *p;
	if (primaryFunc->isOverload())
	    primaryFunc->ove = false;
	if (secondaryFunctionMap.contains(primaryFunc->name())) {
	    NodeList& secs = secondaryFunctionMap[primaryFunc->name()];
	    NodeList::ConstIterator s = secs.begin();
	    while (s != secs.end()) {
		FunctionNode *secondaryFunc = (FunctionNode *) *s;
		if (!secondaryFunc->isOverload())
		    secondaryFunc->ove = true;
		++s;
	    }
	}
	++p;
    }

    NodeList::ConstIterator c = childNodes().begin();
    while (c != childNodes().end()) {
	if ((*c)->isInnerNode())
	    ((InnerNode *) *c)->normalizeOverloads();
	++c;
    }
}

/*!
 */
void InnerNode::removeFromRelated() 
{
    while (!related.isEmpty()) {
        Node *p = static_cast<Node *>(related.takeFirst());

        if (p != 0 && p->relates() == this) p->clearRelated();
    }
}

/*!
 */
void InnerNode::deleteChildren()
{
    qDeleteAll(children);
}

/*!
  Returns true.
 */
bool InnerNode::isInnerNode() const
{
    return true;
}

/*!
 */
const Node *InnerNode::findNode(const QString& name) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findNode(name);
}

/*!
 */
const Node *InnerNode::findNode(const QString& name, Type type) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findNode(name, type);
}

/*!
 */
const FunctionNode *InnerNode::findFunctionNode(const QString& name) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findFunctionNode(name);
}

/*!
 */
const FunctionNode *InnerNode::findFunctionNode(
	const FunctionNode *clone) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findFunctionNode(clone);
}

/*!
 */
const EnumNode *InnerNode::findEnumNodeForValue(const QString &enumValue) const
{
    foreach (const Node *node, enumChildren) {
        const EnumNode *enume = static_cast<const EnumNode *>(node);
        if (enume->hasItem(enumValue))
            return enume;
    }
    return 0;
}

/*!
 */
int InnerNode::overloadNumber(const FunctionNode *func) const
{
    Node *node = (Node *) func;
    if (primaryFunctionMap[func->name()] == node) {
        return 1;
    }
    else {
        return secondaryFunctionMap[func->name()].indexOf(node) + 2;
    }
}

/*!
 */
int InnerNode::numOverloads(const QString& funcName) const
{
    if (primaryFunctionMap.contains(funcName)) {
        return secondaryFunctionMap[funcName].count() + 1;
    }
    else {
        return 0;
    }
}

/*!
 */
NodeList InnerNode::overloads(const QString &funcName) const
{
    NodeList result;
    Node *primary = primaryFunctionMap.value(funcName);
    if (primary) {
        result << primary;
        result += secondaryFunctionMap[funcName];
    }
    return result;
}

/*!
 */
InnerNode::InnerNode(Type type, InnerNode *parent, const QString& name)
    : Node(type, parent, name)
{
}

/*!
 */
void InnerNode::addInclude(const QString& include)
{
    inc.append(include);
}

/*!
 */
void InnerNode::setIncludes(const QStringList& includes)
{
    inc = includes;
}

/*!
  f1 is always the clone
 */
bool InnerNode::isSameSignature(const FunctionNode *f1, const FunctionNode *f2)
{
    if (f1->parameters().count() != f2->parameters().count())
        return false;
    if (f1->isConst() != f2->isConst())
        return false;

    QList<Parameter>::ConstIterator p1 = f1->parameters().begin();
    QList<Parameter>::ConstIterator p2 = f2->parameters().begin();
    while (p2 != f2->parameters().end()) {
	if ((*p1).hasType() && (*p2).hasType()) {
            if ((*p1).rightType() != (*p2).rightType())
                return false;

            QString t1 = p1->leftType();
            QString t2 = p2->leftType();

            if (t1.length() < t2.length())
                qSwap(t1, t2);

            /*
              ### hack for C++ to handle superfluous
              "Foo::" prefixes gracefully
            */
            if (t1 != t2 && t1 != (f2->parent()->name() + "::" + t2))
                return false;
        }
        ++p1;
        ++p2;
    }
    return true;
}

/*!
 */
void InnerNode::addChild(Node *child)
{
    children.append(child);
    if (child->type() == Function) {
        FunctionNode *func = (FunctionNode *) child;
        if (!primaryFunctionMap.contains(func->name())) {
            primaryFunctionMap.insert(func->name(), func);
	}
        else {
	    NodeList &secs = secondaryFunctionMap[func->name()];
	    secs.append(func);
	}
    }
    else {
        if (child->type() == Enum)
            enumChildren.append(child);
	childMap.insert(child->name(), child);
    }
}

/*!
 */
void InnerNode::removeChild(Node *child)
{
    children.removeAll(child);
    enumChildren.removeAll(child);
    if (child->type() == Function) {
	QMap<QString, Node *>::Iterator prim =
		primaryFunctionMap.find(child->name());
	NodeList& secs = secondaryFunctionMap[child->name()];
	if (*prim == child) {
	    if (secs.isEmpty()) {
		primaryFunctionMap.remove(child->name());
	    }
            else {
		primaryFunctionMap.insert(child->name(), secs.takeFirst());
	    }
	}
        else {
	    secs.removeAll(child);
	}
        QMap<QString, Node *>::Iterator ent = childMap.find( child->name() );
        if ( *ent == child )
            childMap.erase( ent );
    }
    else {
	QMap<QString, Node *>::Iterator ent = childMap.find(child->name());
	if (*ent == child)
	    childMap.erase(ent);
    }
}

/*!
  Find the module (QtCore, QtGui, etc.) to which the class belongs.
  We do this by obtaining the full path to the header file's location
  and examine everything between "src/" and the filename.  This is
  semi-dirty because we are assuming a particular directory structure.

  This function is only really useful if the class's module has not
  been defined in the header file with a QT_MODULE macro or with an
  \inmodule command in the documentation.
*/
QString Node::moduleName() const
{
    if (!mod.isEmpty())
        return mod;

    QString path = location().filePath();
    QString pattern = QString("src") + QDir::separator();
    int start = path.lastIndexOf(pattern);

    if (start == -1)
        return "";

    QString moduleDir = path.mid(start + pattern.size());
    int finish = moduleDir.indexOf(QDir::separator());

    if (finish == -1)
        return "";

    QString moduleName = moduleDir.left(finish);

    if (moduleName == "corelib")
        return "QtCore";
    else if (moduleName == "uitools")
        return "QtUiTools";
    else if (moduleName == "gui")
        return "QtGui";
    else if (moduleName == "network")
        return "QtNetwork";
    else if (moduleName == "opengl")
        return "QtOpenGL";
    else if (moduleName == "qt3support")
        return "Qt3Support";
    else if (moduleName == "svg")
        return "QtSvg";
    else if (moduleName == "sql")
        return "QtSql";
    else if (moduleName == "qtestlib")
        return "QtTest";
    else if (moduleDir.contains("webkit"))
        return "QtWebKit";
    else if (moduleName == "xml")
        return "QtXml";
    else
        return "";
}

/*!
 */
void InnerNode::removeRelated(Node *pseudoChild)
{
    related.removeAll(pseudoChild);
}

/*!
  \class LeafNode
 */

/*!
  Returns false because this is an InnerNode.
 */
bool LeafNode::isInnerNode() const
{
    return false;
}

/*!
 */
LeafNode::LeafNode(Type type, InnerNode *parent, const QString& name)
    : Node(type, parent, name)
{
}

/*!
  \class NamespaceNode
 */

/*!
 */
NamespaceNode::NamespaceNode(InnerNode *parent, const QString& name)
    : InnerNode(Namespace, parent, name)
{
}

/*!
  \class ClassNode
 */

/*!
 */
ClassNode::ClassNode(InnerNode *parent, const QString& name)
    : InnerNode(Class, parent, name)
{
    hidden = false;
}

/*!
 */
void ClassNode::addBaseClass(Access access,
                             ClassNode *node,
                             const QString &dataTypeWithTemplateArgs)
{
    bas.append(RelatedClass(access, node, dataTypeWithTemplateArgs));
    node->der.append(RelatedClass(access, this));
}

/*!
 */
void ClassNode::fixBaseClasses()
{
    int i;

    i = 0;
    while (i < bas.size()) {
        ClassNode *baseClass = bas.at(i).node;
        if (baseClass->access() == Node::Private) {
            bas.removeAt(i);

            const QList<RelatedClass> &basesBases = baseClass->baseClasses();
            for (int j = basesBases.size() - 1; j >= 0; --j)
                bas.insert(i, basesBases.at(j));
        }
        else {
            ++i;
        }
    }

    i = 0;
    while (i < der.size()) {
        ClassNode *derivedClass = der.at(i).node;
        if (derivedClass->access() == Node::Private) {
            der.removeAt(i);

            const QList<RelatedClass> &dersDers =
                derivedClass->derivedClasses();
            for (int j = dersDers.size() - 1; j >= 0; --j)
                der.insert(i, dersDers.at(j));
        }
        else {
            ++i;
        }
    }
}

/*!
  \class FakeNode
 */

/*!
 */
FakeNode::FakeNode(InnerNode *parent, const QString& name, SubType subType)
    : InnerNode(Fake, parent, name), sub(subType)
{
}

/*!
 */
QString FakeNode::fullTitle() const
{
    if (sub == File) {
        if (title().isEmpty())
            return name().mid(name().lastIndexOf('/') + 1) + " Example File";
        else
            return title();
    }
    else if (sub == HeaderFile) {
        if (title().isEmpty())
            return name();
        else
            return name() + " - " + title();
    }
    else {
        return title();
    }
}

/*!
 */
QString FakeNode::subTitle() const
{
    if (!stle.isEmpty())
        return stle;

    if (sub == File) {
        if (title().isEmpty() && name().contains("/"))
            return name();
    }
    return QString();
}

/*!
  \class EnumNode
 */

/*!
 */
EnumNode::EnumNode(InnerNode *parent, const QString& name)
    : LeafNode(Enum, parent, name), ft(0)
{
}

/*!
 */
void EnumNode::addItem(const EnumItem& item)
{
    itms.append(item);
    names.insert(item.name());
}

/*!
 */
Node::Access EnumNode::itemAccess(const QString &name) const
{
    if (doc().omitEnumItemNames().contains(name)) {
        return Private;
    }
    else {
        return Public;
    }
}

/*!
  Returns the enum value associated with the enum \a name.
 */
QString EnumNode::itemValue(const QString &name) const
{
    foreach (const EnumItem &item, itms) {
        if (item.name() == name)
            return item.value();
    }
    return QString();
}

/*!
  \class TypedefNode
 */

/*!
 */
TypedefNode::TypedefNode(InnerNode *parent, const QString& name)
    : LeafNode(Typedef, parent, name), ae(0)
{
}

/*!
 */
void TypedefNode::setAssociatedEnum(const EnumNode *enume)
{
    ae = enume;
}

/*!
  \class Parameter
 */

/*!
 */
Parameter::Parameter(const QString& leftType,
                     const QString& rightType,
		     const QString& name,
                     const QString& defaultValue)
    : lef(leftType), rig(rightType), nam(name), def(defaultValue)
{
}

/*!
 */
Parameter::Parameter(const Parameter& p)
    : lef(p.lef), rig(p.rig), nam(p.nam), def(p.def)
{
}

/*!
 */
Parameter& Parameter::operator=(const Parameter& p)
{
    lef = p.lef;
    rig = p.rig;
    nam = p.nam;
    def = p.def;
    return *this;
}

/*!
  \class FunctionNode
 */

/*!
 */
FunctionNode::FunctionNode(InnerNode *parent, const QString& name)
    : LeafNode(Function, parent, name), met(Plain), vir(NonVirtual),
      con(false), sta(false), ove(false), rf(0), ap(0)
{
}

/*!
 */
void FunctionNode::setOverload(bool overlode)
{
    parent()->setOverload(this, overlode);
    ove = overlode;
}

/*!
 */
void FunctionNode::addParameter(const Parameter& parameter)
{
    params.append(parameter);
}

/*!
 */
void FunctionNode::borrowParameterNames(const FunctionNode *source)
{
    QList<Parameter>::Iterator t = params.begin();
    QList<Parameter>::ConstIterator s = source->params.begin();
    while (s != source->params.end() && t != params.end()) {
	if (!(*s).name().isEmpty())
	    (*t).setName((*s).name());
	++s;
	++t;
    }
}

/*!
 */
void FunctionNode::setReimplementedFrom(FunctionNode *from)
{
    rf = from;
    from->rb.append(this);
}

/*!
 */
void FunctionNode::setAssociatedProperty(PropertyNode *property)
{
    ap = property;
}

/*!
 */
int FunctionNode::overloadNumber() const
{
    return parent()->overloadNumber(this);
}

/*!
 */
int FunctionNode::numOverloads() const
{
    return parent()->numOverloads(name());
}

/*!
 */
QStringList FunctionNode::parameterNames() const
{
    QStringList names;
    QList<Parameter>::ConstIterator p = parameters().begin();
    while (p != parameters().end()) {
        names << (*p).name();
        ++p;
    }
    return names;
}

/*!
  \class PropertyNode
 */

/*!
 */
PropertyNode::PropertyNode(InnerNode *parent, const QString& name)
    : LeafNode(Property, parent, name),
      sto(Trool_Default),
      des(Trool_Default),
      overrides(0)
{
}

/*!
 */
void PropertyNode::setOverriddenFrom(const PropertyNode *baseProperty)
{
    for (int i = 0; i < NumFunctionRoles; ++i) {
        if (funcs[i].isEmpty())
            funcs[i] = baseProperty->funcs[i];
    }
    if (sto == Trool_Default)
        sto = baseProperty->sto;
    if (des == Trool_Default)
        des = baseProperty->des;
    overrides = baseProperty;
}

/*!
 */
QString PropertyNode::qualifiedDataType() const
{
    if (setters().isEmpty() && resetters().isEmpty()) {
        if (dt.contains("*") || dt.contains("&")) {
            // 'QWidget *' becomes 'QWidget *' const
            return dt + " const";
        }
        else {
            /*
              'int' becomes 'const int' ('int const' is
              correct C++, but looks wrong)
            */
            return "const " + dt;
        }
    }
    else {
        return dt;
    }
}

/*!
 */
PropertyNode::Trool PropertyNode::toTrool(bool boolean)
{
    return boolean ? Trool_True : Trool_False;
}

/*!
 */
bool PropertyNode::fromTrool(Trool troolean, bool defaultValue)
{
    switch (troolean) {
    case Trool_True:
        return true;
    case Trool_False:
        return false;
    default:
        return defaultValue;
    }
}

/*!
  \class TargetNode
 */

/*!
 */
TargetNode::TargetNode(InnerNode *parent, const QString& name)
    : LeafNode(Target, parent, name)
{
}

/*!
  Returns false because this is a TargetNode.
 */
bool TargetNode::isInnerNode() const
{
    return false;
}

QT_END_NAMESPACE
