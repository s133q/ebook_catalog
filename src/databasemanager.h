#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "book.h"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool init(const QString &dbPath = "ebook_catalog.db");

    bool        addBook(Book &book);
    bool        updateBook(const Book &book);
    bool        deleteBook(int id);
    QList<Book> getAllBooks() const;
    QList<Book> searchBooks(const QString &query) const;
    QList<Book> filterBooks(const QString &author,
                            const QString &genre,
                            int yearFrom, int yearTo) const;
    QStringList distinctAuthors() const;
    QStringList distinctGenres()  const;

    QString lastError() const { return m_lastError; }

private:
    bool createTables();
    static QList<Book> booksFromQuery(QSqlQuery &q);

    QSqlDatabase m_db;
    QString      m_lastError;
};

#endif // DATABASEMANAGER_H
