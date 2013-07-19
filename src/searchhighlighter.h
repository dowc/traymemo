#ifndef SEARCHHIGHLIGHTER_H
#define SEARCHHIGHLIGHTER_H

#include <QtGui>
#include <QSyntaxHighlighter>

class SearchHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    public:
        SearchHighlighter(QTextDocument *parent = 0);
        void setPattern(const QString &pattern);
    protected:
        void highlightBlock(const QString &text);

    private:
        QString m_pattern;
};

#endif // SEARCHHIGHLIGHTER_H
