#include "bookdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <qpushbutton.h>

BookDialog::BookDialog(QWidget *parent) : QDialog(parent)
{
    buildUi();
    connectSignals();
    validateInput();
}

void BookDialog::buildUi()
{
    setMinimumWidth(440);
    setWindowTitle("Додати книгу");

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("Обов'язкове поле");

    m_authorEdit = new QLineEdit(this);
    m_authorEdit->setPlaceholderText("Обов'язкове поле");

    m_genreEdit = new QLineEdit(this);
    m_genreEdit->setPlaceholderText("Наприклад: Фантастика, Детектив...");

    m_yearSpin = new QSpinBox(this);
    m_yearSpin->setRange(0, 2100);
    m_yearSpin->setSpecialValueText("—");
    m_yearSpin->setValue(0);

    m_isbnEdit = new QLineEdit(this);
    m_isbnEdit->setPlaceholderText("978-...");

    m_descEdit = new QTextEdit(this);
    m_descEdit->setPlaceholderText("Короткий опис книги...");
    m_descEdit->setMinimumHeight(70);
    m_descEdit->setMaximumHeight(110);

    auto *form = new QFormLayout;
    form->addRow("Назва *:",     m_titleEdit);
    form->addRow("Автор *:",     m_authorEdit);
    form->addRow("Жанр:",        m_genreEdit);
    form->addRow("Рік видання:", m_yearSpin);
    form->addRow("ISBN:",        m_isbnEdit);
    form->addRow("Опис:",        m_descEdit);

    m_buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto *main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addSpacing(6);
    main->addWidget(m_buttons);
    setLayout(main);
}

void BookDialog::connectSignals()
{
    connect(m_titleEdit,  &QLineEdit::textChanged, this, &BookDialog::validateInput);
    connect(m_authorEdit, &QLineEdit::textChanged, this, &BookDialog::validateInput);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void BookDialog::validateInput()
{
    const bool ok = !m_titleEdit->text().trimmed().isEmpty()
                 && !m_authorEdit->text().trimmed().isEmpty();
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

void BookDialog::setBook(const Book &book)
{
    m_bookId = book.id;
    m_titleEdit->setText(book.title);
    m_authorEdit->setText(book.author);
    m_genreEdit->setText(book.genre);
    m_yearSpin->setValue(book.year);
    m_isbnEdit->setText(book.isbn);
    m_descEdit->setPlainText(book.description);
    setWindowTitle("Редагування книги");
}

Book BookDialog::book() const
{
    Book b;
    b.id          = m_bookId;
    b.title       = m_titleEdit->text().trimmed();
    b.author      = m_authorEdit->text().trimmed();
    b.genre       = m_genreEdit->text().trimmed();
    b.year        = m_yearSpin->value();
    b.isbn        = m_isbnEdit->text().trimmed();
    b.description = m_descEdit->toPlainText().trimmed();
    return b;
}
