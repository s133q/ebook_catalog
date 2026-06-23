#ifndef BOOK_H
#define BOOK_H

#include <QString>

struct Book {
    int     id     = -1;
    QString title;
    QString author;
    QString genre;
    int     year   = 0;
    QString isbn;
    QString description;
};

#endif // BOOK_H
