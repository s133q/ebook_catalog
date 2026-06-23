#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QList>
#include <QWidget>
#include "book.h"

class PdfExporter : public QObject
{
    Q_OBJECT
public:
    explicit PdfExporter(QObject *parent = nullptr);

    bool exportToPdf(const QList<Book> &books,
                     const QString &title,
                     QWidget *parent = nullptr);

    QString lastError() const { return m_lastError; }

private:
    QString m_lastError;
};

#endif // PDFEXPORTER_H
