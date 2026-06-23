#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::init(const QString &dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "catalog_conn");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    QSqlQuery q(m_db);
    q.exec("PRAGMA case_sensitive_like = OFF");
    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery q(m_db);
    const bool ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS books (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            title       TEXT NOT NULL,
            author      TEXT NOT NULL,
            genre       TEXT,
            year        INTEGER,
            isbn        TEXT,
            description TEXT
        )
    )");
    if (!ok) m_lastError = q.lastError().text();
    return ok;
}

bool DatabaseManager::addBook(Book &book)
{
    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO books (title, author, genre, year, isbn, description)
        VALUES (:title, :author, :genre, :year, :isbn, :desc)
    )");
    q.bindValue(":title",  book.title);
    q.bindValue(":author", book.author);
    q.bindValue(":genre",  book.genre);
    q.bindValue(":year",   book.year > 0 ? QVariant(book.year) : QVariant());
    q.bindValue(":isbn",   book.isbn);
    q.bindValue(":desc",   book.description);

    if (!q.exec()) { m_lastError = q.lastError().text(); return false; }
    book.id = q.lastInsertId().toInt();
    return true;
}

bool DatabaseManager::updateBook(const Book &book)
{
    QSqlQuery q(m_db);
    q.prepare(R"(
        UPDATE books
        SET title       = :title,
            author      = :author,
            genre       = :genre,
            year        = :year,
            isbn        = :isbn,
            description = :desc
        WHERE id = :id
    )");
    q.bindValue(":title",  book.title);
    q.bindValue(":author", book.author);
    q.bindValue(":genre",  book.genre);
    q.bindValue(":year",   book.year > 0 ? QVariant(book.year) : QVariant());
    q.bindValue(":isbn",   book.isbn);
    q.bindValue(":desc",   book.description);
    q.bindValue(":id",     book.id);

    if (!q.exec()) { m_lastError = q.lastError().text(); return false; }
    return true;
}

bool DatabaseManager::deleteBook(int id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM books WHERE id = :id");
    q.bindValue(":id", id);
    if (!q.exec()) { m_lastError = q.lastError().text(); return false; }
    return true;
}

QList<Book> DatabaseManager::getAllBooks() const
{
    QSqlQuery q(m_db);
    q.exec("SELECT * FROM books ORDER BY title");
    return booksFromQuery(q);
}

QList<Book> DatabaseManager::searchBooks(const QString &query) const
{
    QSqlQuery q(m_db);
    const QString like = "%" + query.toLower() + "%";
    q.prepare(R"(
        SELECT * FROM books
        WHERE LOWER(title)          LIKE :q
           OR LOWER(author)         LIKE :q
           OR LOWER(genre)          LIKE :q
           OR LOWER(isbn)           LIKE :q
           OR CAST(year AS TEXT)    LIKE :q
        ORDER BY title
    )");
    q.bindValue(":q", like);
    q.exec();
    return booksFromQuery(q);
}

QList<Book> DatabaseManager::filterBooks(const QString &author,
                                         const QString &genre,
                                         int yearFrom, int yearTo) const
{
    QString sql = "SELECT * FROM books WHERE 1=1";
    if (!author.isEmpty()) sql += " AND author LIKE :author";
    if (!genre.isEmpty())  sql += " AND genre  LIKE :genre";
    if (yearFrom > 0)      sql += " AND year >= :yearFrom";
    if (yearTo   > 0)      sql += " AND year <= :yearTo";
    sql += " ORDER BY title";

    QSqlQuery q(m_db);
    q.prepare(sql);
    if (!author.isEmpty()) q.bindValue(":author",   author);
    if (!genre.isEmpty())  q.bindValue(":genre",    genre);
    if (yearFrom > 0)      q.bindValue(":yearFrom", yearFrom);
    if (yearTo   > 0)      q.bindValue(":yearTo",   yearTo);

    if (!q.exec()) {
        qDebug() << "filterBooks error:" << q.lastError().text();
    }
    return booksFromQuery(q);
}

QStringList DatabaseManager::distinctAuthors() const
{
    QStringList list;
    QSqlQuery q("SELECT DISTINCT author FROM books ORDER BY author", m_db);
    while (q.next()) list << q.value(0).toString();
    return list;
}

QStringList DatabaseManager::distinctGenres() const
{
    QStringList list;
    QSqlQuery q("SELECT DISTINCT genre FROM books WHERE genre != '' ORDER BY genre", m_db);
    while (q.next()) list << q.value(0).toString();
    return list;
}

QList<Book> DatabaseManager::booksFromQuery(QSqlQuery &q)
{
    QList<Book> result;
    while (q.next()) {
        Book b;
        b.id          = q.value("id").toInt();
        b.title       = q.value("title").toString();
        b.author      = q.value("author").toString();
        b.genre       = q.value("genre").toString();
        b.year        = q.value("year").toInt();
        b.isbn        = q.value("isbn").toString();
        b.description = q.value("description").toString();
        result << b;
    }
    return result;
}