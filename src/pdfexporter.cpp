#include "pdfexporter.h"
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QFontMetrics>
#include <QDateTime>
#include <QVector>

PdfExporter::PdfExporter(QObject *parent) : QObject(parent) {}

static QStringList wrapText(const QFontMetrics &fm,
                            const QString &text, int maxWidth)
{
    if (text.isEmpty()) return {""};
    QStringList lines;
    QString current;
    for (const QString &word : text.split(' ', Qt::SkipEmptyParts)) {
        const QString candidate = current.isEmpty() ? word : current + ' ' + word;
        if (fm.horizontalAdvance(candidate) <= maxWidth) {
            current = candidate;
        } else {
            if (!current.isEmpty()) lines << current;
            current = word;
        }
    }
    if (!current.isEmpty()) lines << current;
    return lines.isEmpty() ? QStringList{""} : lines;
}

bool PdfExporter::exportToPdf(const QList<Book> &books,
                              const QString &title,
                              QWidget *parent)
{
    if (books.isEmpty()) {
        m_lastError = "Список книг порожній";
        return false;
    }

    const QString path = QFileDialog::getSaveFileName(
        parent, "Зберегти каталог як PDF", "catalog.pdf", "PDF-файли (*.pdf)");
    if (path.isEmpty()) return false;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Portrait);

    QPainter p;
    if (!p.begin(&printer)) {
        m_lastError = "Не вдалося ініціалізувати QPainter";
        return false;
    }

    const QRect vp  = p.viewport();
    const int   L   = vp.left()   + 60;
    const int   R   = vp.right()  - 60;
    const int   TOP = vp.top()    + 60;
    const int   BOT = vp.bottom() - 50;
    const int   W   = R - L;
    int y = TOP;

    QFont fTitle("Arial", 16, QFont::Bold);
    QFont fHeader("Arial", 9,  QFont::Bold);
    QFont fRow("Arial", 8);
    QFont fFoot("Arial", 8);

    // заголовок
    p.setFont(fTitle);
    p.drawText(QRect(L, y, W, 36), Qt::AlignHCenter | Qt::AlignVCenter, title);
    y += 44;

    p.setFont(fFoot);
    p.setPen(Qt::gray);
    p.drawText(QRect(L, y, W, 18), Qt::AlignRight,
               "Дата: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
    p.setPen(Qt::black);
    y += 28;

    // колонки
    const int c0 = 28;            // №
    const int c4 = 44;            // Рік
    const int c5 = W * 17 / 100; // ISBN
    const int c3 = W * 18 / 100; // Жанр
    const int c2 = W * 23 / 100; // Автор
    const int c1 = W - c0 - c2 - c3 - c4 - c5; // Назва

    const QVector<int> cw = {c0, c1, c2, c3, c4, c5};
    auto cx = [&](int col) {
        int x = L;
        for (int i = 0; i < col; ++i) x += cw[i];
        return x;
    };

    const QStringList headers = {"№", "Назва", "Автор", "Жанр", "Рік", "ISBN"};
    const int headerH = 22;

    auto drawHeader = [&]() {
        p.setFont(fHeader);
        for (int c = 0; c < headers.size(); ++c) {
            p.fillRect(cx(c), y, cw[c], headerH, QColor(100, 60, 200));
            p.setPen(Qt::white);
            p.drawText(QRect(cx(c) + 4, y, cw[c] - 8, headerH),
                       Qt::AlignVCenter, headers[c]);
        }
        p.setPen(Qt::black);
        y += headerH;
    };

    drawHeader();

    // рядки 
    p.setFont(fRow);
    const QFontMetrics fm(fRow);
    bool shade = false;
    const int padX = 4, padY = 3;

    for (int i = 0; i < books.size(); ++i) {
        const Book &b = books[i];

        const QVector<QStringList> wrapped = {
                                              {QString::number(i + 1)},
                                              wrapText(fm, b.title,  cw[1] - padX * 2),
                                              wrapText(fm, b.author, cw[2] - padX * 2),
                                              wrapText(fm, b.genre,  cw[3] - padX * 2),
                                              {b.year > 0 ? QString::number(b.year) : "—"},
                                              wrapText(fm, b.isbn,   cw[5] - padX * 2),
                                              };

        int maxLines = 1;
        for (const auto &wl : wrapped)
            maxLines = qMax(maxLines, wl.size());
        const int rowH = maxLines * (fm.height() + 1) + padY * 2;

        if (y + rowH > BOT) {
            printer.newPage();
            y = TOP;
            drawHeader();
        }

        if (shade)
            p.fillRect(L, y, W, rowH, QColor(240, 235, 255));
        shade = !shade;

        for (int c = 0; c < wrapped.size(); ++c) {
            const int align = (c == 0 || c == 4)
            ? (Qt::AlignTop | Qt::AlignHCenter)
            : (Qt::AlignTop | Qt::AlignLeft);
            int ty = y + padY;
            for (const QString &ln : wrapped[c]) {
                p.drawText(QRect(cx(c) + padX, ty,
                                 cw[c] - padX * 2, fm.height() + 1),
                           align, ln);
                ty += fm.height() + 1;
            }
        }
        y += rowH;
    }

    // підсумок
    y += 12;
    if (y + 18 <= BOT) {
        p.setFont(fFoot);
        p.setPen(Qt::gray);
        p.drawText(QRect(L, y, W, 18), Qt::AlignRight,
                   QString("Усього книг: %1").arg(books.size()));
    }

    p.end();
    return true;
}
