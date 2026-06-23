#ifndef BOOKDIALOG_H
#define BOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "book.h"

class BookDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BookDialog(QWidget *parent = nullptr);
    void setBook(const Book &book);
    Book book() const;

private:
    void buildUi();
    void connectSignals();
    void validateInput();

    QLineEdit        *m_titleEdit;
    QLineEdit        *m_authorEdit;
    QLineEdit        *m_genreEdit;
    QSpinBox         *m_yearSpin;
    QLineEdit        *m_isbnEdit;
    QTextEdit        *m_descEdit;
    QDialogButtonBox *m_buttons;
    int m_bookId = -1;
};

#endif // BOOKDIALOG_H
