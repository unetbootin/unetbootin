#ifndef MESSAGEEDITORWIDGETS_H
#define MESSAGEEDITORWIDGETS_H

#include <QImage>
#include <QLabel>
#include <QMap>
#include <QTextEdit>
#include <QUrl>
#include <QWidget>

QT_BEGIN_NAMESPACE

class QAction;
class QContextMenuEvent;
class QKeyEvent;
class QMenu;
class QSizeF;
class QString;
class QVariant;

class MessageHighlighter;

/*
  Automatically adapt height to document contents
 */
class ExpandingTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    ExpandingTextEdit(QWidget *parent = 0);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private slots:
    void updateHeight(const QSizeF &documentSize);
    void reallyEnsureCursorVisible();

private:
    int m_minimumHeight;
};

/*
  Format markup & control characters
*/
class FormatTextEdit : public ExpandingTextEdit
{
    Q_OBJECT
public:
    FormatTextEdit(QWidget *parent = 0);
    QString getPlainText();
    void setEditable(bool editable);
    // QTextEdit
    virtual QVariant loadResource(int type, const QUrl &name);

public slots:
    void copySelection();
    void setPlainText(const QString & text, bool userAction);

protected:
    // QWidget
    virtual void contextMenuEvent(QContextMenuEvent * e);
    virtual void keyPressEvent(QKeyEvent *e);

private:
    static QString plainText(const QTextDocument *document);

    QMap<QUrl, QImage> m_backTabOmages;
    MessageHighlighter *m_highlighter;
};

/*
  Displays text field & associated label
*/
class FormWidget : public QWidget
{
    Q_OBJECT
public:
    FormWidget(const QString &label, bool isEditable, QWidget *parent = 0);
    void setLabel(const QString &label) { m_label->setText(label); }
    void setTranslation(const QString &text, bool userAction = false);
    void clearTranslation() { m_editor->clear(); }
    QString getTranslation() { return m_editor->getPlainText(); }
    void setEditingEnabled(bool enable);
    void setHideWhenEmpty(bool optional) { m_hideWhenEmpty = optional; }
    FormatTextEdit *getEditor() { return m_editor; }

signals:
    void textChanged();
    void selectionChanged();

private:
    QLabel *m_label;
    FormatTextEdit *m_editor;
    bool m_hideWhenEmpty;
};

QT_END_NAMESPACE

#endif // MESSAGEEDITORWIDGETS_H
