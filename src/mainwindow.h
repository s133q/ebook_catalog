#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QAction>
#include <QList>
#include "book.h"
#include "databasemanager.h"
#include "pdfexporter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddBook();
    void onEditBook();
    void onDeleteBook();
    void onSearch();
    void onFilter();
    void onResetFilter();
    void onExportPdf();
    void onToggleTheme();
    void onTableSelectionChanged();
    void onTableDoubleClicked(int row, int col);

private:
    void buildUi();
    void buildMenu();
    void connectSignals();
    void refreshTable(const QList<Book> &books);
    void loadAllBooks();
    void updateFilterComboBoxes();
    int  selectedId() const;

    QLineEdit   *m_searchEdit;
    QPushButton *m_searchBtn;
    QPushButton *m_resetBtn;
    QComboBox   *m_authorCombo;
    QComboBox   *m_genreCombo;
    QSpinBox    *m_yearFromSpin;
    QSpinBox    *m_yearToSpin;
    QPushButton *m_filterBtn;

    QTableWidget *m_table;

    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_exportBtn;
    QPushButton *m_themeBtn;

    // Пункти меню для керування станом enabled
    QAction *m_editAction;
    QAction *m_deleteAction;

    QLabel *m_statusLabel;

    DatabaseManager *m_db;
    PdfExporter     *m_pdf;
    QList<Book>      m_currentBooks;
};

#endif // MAINWINDOW_H
