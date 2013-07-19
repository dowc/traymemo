#include "searchhighlighter.h"
#include <QtGui>

SearchHighlighter::SearchHighlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
{
}

void SearchHighlighter::setPattern(const QString &pattern)
{
    m_pattern = pattern;
}

void SearchHighlighter::highlightBlock(const QString &text)
{
    if (m_pattern.isEmpty())
        return;

    QTextCharFormat colorFormat;
    colorFormat.setFontWeight(QFont::Bold);
    colorFormat.setForeground(Qt::red);

    QRegExp expression(m_pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, colorFormat);
        index = text.indexOf(expression, index + length);
    }
}
