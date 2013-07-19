#ifndef TEXTFINDER_H
#define TEXTFINDER_H

#include <QtGui>

class TextFinder : public QWidget
 {
     Q_OBJECT

 public:
     TextFinder(QWidget *parent = 0);

 public slots:
     void showHideFind();
     void findNextOccurrence();

 private slots:
     void on_findButton_clicked();
     void on_lineEdit_textChanged(const QString & text);

 private:
     void highlightBlock(const QString &text);
     QTextEdit* getCurrentTextEdit() const;
     bool findKeyWord(const QString &searcString, QTextCursor highlightCursor);

     QPushButton *m_findButton;     
     QLineEdit *m_keywordLineEdit;
     bool isFirstTime;
     QTextCursor m_tempCursor;
     QString m_tempKeyword;
 };

#endif // TEXTFINDER_H
