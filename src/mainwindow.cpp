#include "mainwindow.h"
#include "bookdialog.h"
#include "thememanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_db(new DatabaseManager(this))
    , m_pdf(new PdfExporter(this))
{
    setWindowTitle("Каталог електронних книг");
    setMinimumSize(900, 600);
    resize(1140, 700);

    if (!m_db->init()) {
        QMessageBox::critical(this, "Помилка БД",
            "Не вдалося відкрити базу даних:\n" + m_db->lastError());
    }

    buildUi();
    buildMenu();
    connectSignals();
    updateFilterComboBoxes(); // заповнюємо фільтри одразу при старті
    loadAllBooks();
}

MainWindow::~MainWindow() {}

// ─── Побудова UI ──────────────────────────────────────────────────────────

void MainWindow::buildUi()
{
    // ── Рядок пошуку ─────────────────────────────────────────────────────
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Пошук за назвою, автором, жанром, ISBN...");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumWidth(300);

    m_searchBtn = new QPushButton("⌕ Знайти", this);

    m_resetBtn = new QPushButton("× Скинути", this);
    m_resetBtn->setObjectName("secondaryBtn");

    // Кнопка теми — тільки іконка, без тексту і рамки
    m_themeBtn = new QPushButton(ThemeManager::instance().isDark() ? "☀" : "☾", this);
    m_themeBtn->setObjectName("themeIconBtn");
    m_themeBtn->setToolTip("Перемкнути тему");

    auto *searchRow = new QHBoxLayout;
    searchRow->addWidget(new QLabel("Пошук:", this));
    searchRow->addWidget(m_searchEdit, 1);
    searchRow->addWidget(m_searchBtn);
    searchRow->addWidget(m_resetBtn);
    searchRow->addStretch();
    searchRow->addWidget(m_themeBtn);

    // ── Панель фільтрів ───────────────────────────────────────────────────
    m_authorCombo = new QComboBox(this);
    m_authorCombo->addItem("Усі автори");
    m_authorCombo->setMinimumWidth(170);
    m_authorCombo->setToolTip("Фільтр за автором");

    m_genreCombo = new QComboBox(this);
    m_genreCombo->addItem("Усі жанри");
    m_genreCombo->setMinimumWidth(150);
    m_genreCombo->setToolTip("Фільтр за жанром");

    m_yearFromSpin = new QSpinBox(this);
    m_yearFromSpin->setRange(0, 2100);
    m_yearFromSpin->setSpecialValueText("—");
    m_yearFromSpin->setFixedWidth(76);

    m_yearToSpin = new QSpinBox(this);
    m_yearToSpin->setRange(0, 2100);
    m_yearToSpin->setSpecialValueText("—");
    m_yearToSpin->setFixedWidth(76);

    m_filterBtn = new QPushButton("Фільтрувати", this);

    auto *filterRow = new QHBoxLayout;
    filterRow->addWidget(new QLabel("Автор:", this));
    filterRow->addWidget(m_authorCombo);
    filterRow->addWidget(new QLabel("Жанр:", this));
    filterRow->addWidget(m_genreCombo);
    filterRow->addWidget(new QLabel("Рік від:", this));
    filterRow->addWidget(m_yearFromSpin);
    filterRow->addWidget(new QLabel("до:", this));
    filterRow->addWidget(m_yearToSpin);
    filterRow->addWidget(m_filterBtn);
    filterRow->addStretch();

    auto *filterBox = new QGroupBox("Фільтр", this);
    filterBox->setLayout(filterRow);

    // ── Таблиця ───────────────────────────────────────────────────────────
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(
        {"ID", "Назва", "Автор", "Жанр", "Рік", "ISBN"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->setColumnWidth(0, 42);
    m_table->setColumnWidth(3, 160);
    m_table->setColumnWidth(4, 54);
    m_table->setColumnWidth(5, 130);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSortingEnabled(true);
    m_table->setWordWrap(false);

    // ── Кнопки CRUD ───────────────────────────────────────────────────────
    m_addBtn    = new QPushButton("+ Додати",     this);
    m_editBtn   = new QPushButton("✎ Редагувати", this);
    m_deleteBtn = new QPushButton("⌫ Видалити",   this);
    m_exportBtn = new QPushButton("🗁 Експорт PDF",this);

    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);

    auto *btnLayout = new QVBoxLayout;
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_editBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addSpacing(16);
    btnLayout->addWidget(m_exportBtn);
    btnLayout->addStretch();

    // ── Загальне компонування ─────────────────────────────────────────────
    auto *tableRow = new QHBoxLayout;
    tableRow->addWidget(m_table, 1);
    tableRow->addLayout(btnLayout);

    auto *topLayout = new QVBoxLayout;
    topLayout->setSpacing(8);
    topLayout->addLayout(searchRow);
    topLayout->addWidget(filterBox);
    topLayout->addLayout(tableRow, 1);

    auto *central = new QWidget(this);
    central->setLayout(topLayout);
    central->setContentsMargins(10, 8, 10, 8);
    setCentralWidget(central);

    m_statusLabel = new QLabel("Готово", this);
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::buildMenu()
{
    auto *fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction("Експорт PDF...", this, &MainWindow::onExportPdf);
    fileMenu->addSeparator();
    fileMenu->addAction("Вихід", qApp, &QApplication::quit);

    auto *bookMenu = menuBar()->addMenu("Книги");
    bookMenu->addAction("Додати", this, &MainWindow::onAddBook);

    m_editAction = bookMenu->addAction("Редагувати", this, &MainWindow::onEditBook);
    m_editAction->setEnabled(false);

    m_deleteAction = bookMenu->addAction("Видалити", this, &MainWindow::onDeleteBook);
    m_deleteAction->setEnabled(false);

    auto *viewMenu = menuBar()->addMenu("Вигляд");
    viewMenu->addAction("Перемкнути тему", this, &MainWindow::onToggleTheme);

    auto *helpMenu = menuBar()->addMenu("Довідка");
    helpMenu->addAction("Про програму", this, [this]() {
        QMessageBox::about(this, "Про програму",
            "<b>Каталог електронних книг</b><br>"
            "Курсова робота Оропай О.В. КІУКІу-25-1");
    });
}

void MainWindow::connectSignals()
{
    connect(m_searchBtn,  &QPushButton::clicked,     this, &MainWindow::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed,  this, &MainWindow::onSearch);
    connect(m_resetBtn,   &QPushButton::clicked,     this, &MainWindow::onResetFilter);
    connect(m_filterBtn,  &QPushButton::clicked,     this, &MainWindow::onFilter);
    connect(m_addBtn,     &QPushButton::clicked,     this, &MainWindow::onAddBook);
    connect(m_editBtn,    &QPushButton::clicked,     this, &MainWindow::onEditBook);
    connect(m_deleteBtn,  &QPushButton::clicked,     this, &MainWindow::onDeleteBook);
    connect(m_exportBtn,  &QPushButton::clicked,     this, &MainWindow::onExportPdf);
    connect(m_themeBtn,   &QPushButton::clicked,     this, &MainWindow::onToggleTheme);
    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onTableSelectionChanged);
    connect(m_table, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onTableDoubleClicked);
}

// ─── Слоти ────────────────────────────────────────────────────────────────

void MainWindow::onAddBook()
{
    BookDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Book b = dlg.book();
        if (m_db->addBook(b)) {
            updateFilterComboBoxes();
            loadAllBooks();
            m_statusLabel->setText(
                QString("Книгу «%1» успішно додано").arg(b.title));
        } else {
            QMessageBox::warning(this, "Помилка",
                "Не вдалося додати книгу:\n" + m_db->lastError());
        }
    }
}

void MainWindow::onEditBook()
{
    const int id = selectedId();
    if (id < 0) return;

    Book current;
    for (const auto &b : m_currentBooks)
        if (b.id == id) { current = b; break; }

    BookDialog dlg(this);
    dlg.setBook(current);
    if (dlg.exec() == QDialog::Accepted) {
        Book updated = dlg.book();
        updated.id = id;
        if (m_db->updateBook(updated)) {
            updateFilterComboBoxes();
            loadAllBooks();
            m_statusLabel->setText(
                QString("Книгу «%1» оновлено").arg(updated.title));
        } else {
            QMessageBox::warning(this, "Помилка",
                "Не вдалося оновити книгу:\n" + m_db->lastError());
        }
    }
}

void MainWindow::onDeleteBook()
{
    const int id = selectedId();
    if (id < 0) return;

    QString title;
    for (const auto &b : m_currentBooks)
        if (b.id == id) { title = b.title; break; }

    if (QMessageBox::question(this, "Підтвердження",
            QString("Видалити книгу «%1»?").arg(title),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        if (m_db->deleteBook(id)) {
            updateFilterComboBoxes();
            loadAllBooks();
            m_statusLabel->setText(QString("Книгу «%1» видалено").arg(title));
        } else {
            QMessageBox::warning(this, "Помилка",
                "Не вдалося видалити книгу:\n" + m_db->lastError());
        }
    }
}

void MainWindow::onSearch()
{
    const QString q = m_searchEdit->text().trimmed();
    if (q.isEmpty()) { loadAllBooks(); return; }
    const auto result = m_db->searchBooks(q);
    refreshTable(result);
    m_statusLabel->setText(QString("Знайдено: %1 кн.").arg(result.size()));
}

void MainWindow::onFilter()
{
    const QString author = (m_authorCombo->currentIndex() <= 0)
                           ? QString() : m_authorCombo->currentText();
    const QString genre  = (m_genreCombo->currentIndex() <= 0)
                           ? QString() : m_genreCombo->currentText();
    const int yearFrom = m_yearFromSpin->value();
    const int yearTo   = m_yearToSpin->value();

    const auto result = m_db->filterBooks(author, genre, yearFrom, yearTo);
    refreshTable(result);
    m_statusLabel->setText(QString("Відфільтровано: %1 кн.").arg(result.size()));
}

void MainWindow::onResetFilter()
{
    m_searchEdit->clear();
    m_authorCombo->setCurrentIndex(0);
    m_genreCombo->setCurrentIndex(0);
    m_yearFromSpin->setValue(0);
    m_yearToSpin->setValue(0);
    loadAllBooks();
}

void MainWindow::onExportPdf()
{
    if (m_currentBooks.isEmpty()) {
        QMessageBox::information(this, "Експорт PDF",
            "Список книг порожній — нічого експортувати.");
        return;
    }
    if (m_pdf->exportToPdf(m_currentBooks, "Каталог електронних книг", this))
        m_statusLabel->setText("PDF успішно збережено");
    else if (!m_pdf->lastError().isEmpty())
        QMessageBox::warning(this, "Помилка PDF", m_pdf->lastError());
}

void MainWindow::onToggleTheme()
{
    ThemeManager::instance().toggle();
    m_themeBtn->setText(ThemeManager::instance().isDark() ? "☀" : "☾");
}

void MainWindow::onTableSelectionChanged()
{
    const bool sel = !m_table->selectedItems().isEmpty();
    m_editBtn->setEnabled(sel);
    m_deleteBtn->setEnabled(sel);
    m_editAction->setEnabled(sel);
    m_deleteAction->setEnabled(sel);
}

void MainWindow::onTableDoubleClicked(int, int) { onEditBook(); }

// ─── Таблиця ──────────────────────────────────────────────────────────────

void MainWindow::refreshTable(const QList<Book> &books)
{
    m_currentBooks = books;
    m_table->setSortingEnabled(false);
    m_table->setRowCount(0);

    for (const Book &b : books) {
        const int row = m_table->rowCount();
        m_table->insertRow(row);

        // Qt::Alignment замість int — усуває deprecation warning у Qt 6
        auto makeItem = [](const QString &text,
                           Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft)
        {
            auto *it = new QTableWidgetItem(text);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            if (!text.isEmpty())
                it->setToolTip(text);
            return it;
        };

        const QString yearStr = b.year > 0 ? QString::number(b.year) : "";
        m_table->setItem(row, 0, makeItem(QString::number(b.id),
                                          Qt::AlignVCenter | Qt::AlignRight));
        m_table->setItem(row, 1, makeItem(b.title));
        m_table->setItem(row, 2, makeItem(b.author));
        m_table->setItem(row, 3, makeItem(b.genre));
        m_table->setItem(row, 4, makeItem(yearStr,
                                          Qt::AlignVCenter | Qt::AlignHCenter));
        m_table->setItem(row, 5, makeItem(b.isbn));

        m_table->setRowHeight(row, 28);
    }

    m_table->setSortingEnabled(true);
    m_statusLabel->setText(QString("Книг у списку: %1").arg(books.size()));
}

void MainWindow::loadAllBooks()      { refreshTable(m_db->getAllBooks()); }

void MainWindow::updateFilterComboBoxes()
{
    const QString ca = m_authorCombo->currentText();
    const QString cg = m_genreCombo->currentText();

    m_authorCombo->clear();
    m_authorCombo->addItem("Усі автори");
    m_authorCombo->addItems(m_db->distinctAuthors());

    m_genreCombo->clear();
    m_genreCombo->addItem("Усі жанри");
    m_genreCombo->addItems(m_db->distinctGenres());

    int i = m_authorCombo->findText(ca);
    if (i >= 0) m_authorCombo->setCurrentIndex(i);
    i = m_genreCombo->findText(cg);
    if (i >= 0) m_genreCombo->setCurrentIndex(i);
}

int MainWindow::selectedId() const
{
    const auto sel = m_table->selectedItems();
    if (sel.isEmpty()) return -1;
    auto *it = m_table->item(sel.first()->row(), 0);
    return it ? it->text().toInt() : -1;
}
