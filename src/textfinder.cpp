#include "textfinder.h"
#include "traymemowindow.h"
#include "searchhighlighter.h"

#include <QtGui>

TextFinder::TextFinder(QWidget *parent)
     : QWidget(parent)
{
    QLabel *keywordLabel = new QLabel(this);
    keywordLabel->setText("Keyword(s): ");
    m_keywordLineEdit = new QLineEdit(this);
    m_findButton = new QPushButton(this);
    m_findButton->setText("Find");
    m_findButton->setEnabled(false);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(keywordLabel);
    layout->addWidget(m_keywordLineEdit);
    layout->addWidget(m_findButton);
    setLayout(layout);

    QObject::connect(m_keywordLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(on_lineEdit_textChanged(const QString &)));
    QObject::connect(m_findButton, SIGNAL(clicked()), this, SLOT(on_findButton_clicked()));
}

void TextFinder::on_lineEdit_textChanged(const QString & text)
{
    // Enabled/disable find button based on if there is text in find search box
    (text.isEmpty()) ? m_findButton->setDisabled(true) : m_findButton->setDisabled(false);
}

void TextFinder::on_findButton_clicked()
{
    QString searchString = m_keywordLineEdit->text();

    if (searchString.isEmpty())
    {
        QMessageBox::information(this, tr("Empty Search Field"),
             "The search field is empty. Please enter a word and click Find.");
        return;
    }

    QTextEdit *textEdit = getCurrentTextEdit();
    if (textEdit == NULL)
    {
        QMessageBox::information(this, tr("No Opened Documents"),
             "No documents opened. Please open document(s).");
        return;
    }

    if (searchString == m_tempKeyword)
    {
        findNextOccurrence();
        return;
    }

    QTextCursor highlightCursor(textEdit->document());
    highlightCursor.clearSelection();

    //highlightCursor.movePosition(QTextCursor::End);    
    highlightCursor.movePosition(QTextCursor::Start);

    bool found = false;
    found = findKeyWord(searchString, highlightCursor);    

    if (found == false)
    {
        QMessageBox::information(this, tr("Word Not Found"),
          "The word cannot be found.");
    }
}

void TextFinder::findNextOccurrence()
{
    bool found = false;
    found = findKeyWord(m_tempKeyword, m_tempCursor);
    if (!found)
    {
        QMessageBox::information(this, tr("Reached End Of The Document"),
             "Search reached end of the document.");

        //m_tempCursor.movePosition(QTextCursor::End);
        m_tempCursor.movePosition(QTextCursor::Start);
    }
}

bool TextFinder::findKeyWord(const QString &searchString, QTextCursor highlightCursor)
{
    bool found = false;
    QTextEdit *textEdit = getCurrentTextEdit();

    if (!highlightCursor.isNull() && !highlightCursor.atEnd())
    //    if (!highlightCursor.isNull() && !highlightCursor.atStart())
    {
        //QTextDocument::FindWholeWords | , QTextDocument::FindBackward
        highlightCursor = textEdit->document()->find(searchString, highlightCursor);

        if (!highlightCursor.isNull())
        {
            found = highlightCursor.movePosition(QTextCursor::StartOfWord);
            highlightCursor.select(QTextCursor::WordUnderCursor);
            textEdit->setTextCursor(highlightCursor);
            SearchHighlighter *highLighter = new SearchHighlighter(textEdit->document());
            highLighter->setPattern(searchString);
            m_tempCursor = highlightCursor;
            m_tempKeyword = searchString;
        }
    }

    return found;
}

QTextEdit* TextFinder::getCurrentTextEdit() const
{
    QTextEdit *textEdit = NULL;
    QWidget* currentTextEdit = 0;

    currentTextEdit = dynamic_cast<TrayMemoWindow*>(this->parent())->getCurrentTextEdit();

    if (currentTextEdit)
        textEdit = dynamic_cast<QTextEdit*>(currentTextEdit);

    return textEdit;
}

void TextFinder::showHideFind()
{
    if (this->isHidden())
    {
        this->show();
        this->m_keywordLineEdit->setFocus();
    }
    else
    {
        this->hide();
        QTextEdit *currentTextEdit = getCurrentTextEdit();

        if (currentTextEdit)
            currentTextEdit->setFocus();

        foreach(QSyntaxHighlighter* highlighter,
                currentTextEdit->findChildren<QSyntaxHighlighter*>()) {
            delete highlighter;
        }
    }
}
