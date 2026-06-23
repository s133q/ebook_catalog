#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QApplication>
#include <QSettings>
#include <QString>

class ThemeManager
{
public:
    enum Theme { Light, Dark };

    static ThemeManager &instance() {
        static ThemeManager mgr;
        return mgr;
    }

    void apply(Theme t) {
        m_current = t;
        QSettings s;
        s.setValue("ui/theme", t == Dark ? "dark" : "light");
        applyQss(t);
    }

    void load() {
        QSettings s;
        apply(s.value("ui/theme", "light").toString() == "dark" ? Dark : Light);
    }

    void toggle() { apply(m_current == Light ? Dark : Light); }
    Theme current() const { return m_current; }
    bool  isDark()  const { return m_current == Dark; }

private:
    ThemeManager() = default;
    Theme m_current = Light;

    static void applyQss(Theme t)
    {
        const QString accent      = "#7C4DFF";
        const QString accentHover = "#651FFF";
        const QString accentPress = "#6200EA";

        // Другорядна кнопка та неактивні пункти меню:
        // світла тема — фіолетовий; темна — пастельний бузковий
        const QString secondaryColor = (t == Light) ? accent : "#B39DDB";
        // Неактивні пункти меню
        const QString disabledMenuColor = (t == Light) ? "#BDBDBD" : "#585B70";

        QString qss;

        if (t == Light) {
            qss = QString(R"(
QMainWindow, QDialog {
    background-color: #F5F5F5;
}
QWidget {
    font-family: "Segoe UI", "Arial", sans-serif;
    font-size: 13px;
    color: #212121;
}
QTableWidget {
    background-color: #FFFFFF;
    alternate-background-color: #F3EFFF;
    gridline-color: #E0E0E0;
    border: 1px solid #E0E0E0;
    border-radius: 6px;
    selection-background-color: %1;
    selection-color: #FFFFFF;
}
QHeaderView::section {
    background-color: %1;
    color: #FFFFFF;
    font-weight: bold;
    padding: 6px 4px;
    border: none;
}
QHeaderView::section:hover {
    background-color: %2;
}
QPushButton {
    background-color: %1;
    color: #FFFFFF;
    border: none;
    border-radius: 6px;
    padding: 7px 16px;
    font-weight: 600;
    min-width: 100px;
}
QPushButton:hover    { background-color: %2; }
QPushButton:pressed  { background-color: %3; }
QPushButton:disabled { background-color: #BDBDBD; color: #757575; }

/* Другорядна кнопка — прозорий фон, кольоровий текст */
QPushButton#secondaryBtn {
    background-color: transparent;
    color: %4;                /* фіолетовий для світлої теми */
    border: none;
    min-width: 0;
    padding: 6px 10px;
    font-weight: normal;
}
QPushButton#secondaryBtn:hover { background-color: #EDE7F6; border-radius: 6px; }
QPushButton#secondaryBtn:pressed { background-color: #D1C4E9; }

/* Іконка-кнопка теми — квадратна, без рамки; кольором збігається з secondaryBtn */
QPushButton#themeIconBtn {
    background-color: transparent;
    border: none;
    min-width: 0;
    min-height: 0;
    padding: 4px 8px;
    font-size: 18px;
    color: %4;                 /* робимо значок того ж кольору, що і secondaryBtn */
}
QPushButton#themeIconBtn:hover { background-color: #EDE7F6; border-radius: 6px; }

QLineEdit, QTextEdit, QComboBox {
    background-color: #FFFFFF;
    border: 1px solid #BDBDBD;
    border-radius: 5px;
    padding: 5px 8px;
    selection-background-color: %1;
}
QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
    border: 2px solid %1;
}

QComboBox::drop-down { border: none; width: 24px; }
QComboBox QAbstractItemView {
    background: #FFFFFF;
    selection-background-color: %1;
    selection-color: #FFFFFF;
}
QGroupBox {
    border: 1px solid #E0E0E0;
    border-radius: 6px;
    margin-top: 6px;
    padding-top: 4px;
    font-weight: bold;
    color: #616161;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QMenuBar {
    background-color: #FFFFFF;
    border-bottom: 1px solid #E0E0E0;
}
QMenuBar::item:selected {
    background-color: %1;
    color: #FFFFFF;
    border-radius: 4px;
}
QMenu {
    background-color: #FFFFFF;
    border: 1px solid #E0E0E0;
    border-radius: 6px;
}
QMenu::item {
    padding: 5px 20px 5px 20px;
    color: #212121;
}
QMenu::item:selected {
    background-color: #EDE7F6;
    color: %1;
}
QMenu::item:disabled {
    color: %5;
}
QStatusBar {
    background-color: #EEEEEE;
    border-top: 1px solid #E0E0E0;
    color: #616161;
}
QScrollBar:vertical {
    width: 8px;
    background: transparent;
}
QScrollBar::handle:vertical {
    background: #BDBDBD;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::handle:vertical:hover { background: %1; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
)").arg(accent, accentHover, accentPress, secondaryColor, disabledMenuColor);

        } else {
            // ── Темна тема ─────────────────────────────────────────────
            qss = QString(R"(
QMainWindow, QDialog {
    background-color: #1E1E2E;
}
QWidget {
    font-family: "Segoe UI", "Arial", sans-serif;
    font-size: 13px;
    color: #CDD6F4;
    background-color: #1E1E2E;
}
QTableWidget {
    background-color: #181825;
    alternate-background-color: #2A2640;
    gridline-color: #313244;
    border: 1px solid #313244;
    border-radius: 6px;
    selection-background-color: %1;
    selection-color: #FFFFFF;
}
QHeaderView::section {
    background-color: %3;
    color: #FFFFFF;
    font-weight: bold;
    padding: 6px 4px;
    border: none;
}
QHeaderView::section:hover {
    background-color: %2;
}
QPushButton {
    background-color: %1;
    color: #FFFFFF;
    border: none;
    border-radius: 6px;
    padding: 7px 16px;
    font-weight: 600;
    min-width: 100px;
}
QPushButton:hover    { background-color: %2; }
QPushButton:pressed  { background-color: %3; }
QPushButton:disabled { background-color: #45475A; color: #585B70; }

/* У темній темі: робимо secondaryBtn білим, щоб контрастувати з темним фоном */
QPushButton#secondaryBtn {
    background-color: transparent;
    color: #FFFFFF;           /* БІЛИЙ для теми */
    border: none;
    min-width: 0;
    padding: 6px 10px;
    font-weight: normal;
}
QPushButton#secondaryBtn:hover   { background-color: #2A2640; border-radius: 6px; }
QPushButton#secondaryBtn:pressed { background-color: #313244; }

/* Іконка теми теж біла у темній темі, щоб бути читабельною */
QPushButton#themeIconBtn {
    background-color: transparent;
    border: none;
    min-width: 0;
    min-height: 0;
    padding: 4px 8px;
    font-size: 18px;
    color: #FFFFFF;           /* БІЛИЙ для теми */
}
QPushButton#themeIconBtn:hover { background-color: #2A2640; border-radius: 6px; }

QLineEdit, QTextEdit, QComboBox {
    background-color: #313244;
    border: 1px solid #45475A;
    border-radius: 5px;
    padding: 5px 8px;
    color: #CDD6F4;
    selection-background-color: %1;
}
QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
    border: 2px solid %1;
}

QComboBox::drop-down { border: none; width: 24px; }
QComboBox QAbstractItemView {
    background: #313244;
    color: #CDD6F4;
    selection-background-color: %1;
    selection-color: #FFFFFF;
}
QGroupBox {
    border: 1px solid #313244;
    border-radius: 6px;
    margin-top: 6px;
    padding-top: 4px;
    font-weight: bold;
    color: #7F849C;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QMenuBar {
    background-color: #181825;
    border-bottom: 1px solid #313244;
}
QMenuBar::item:selected {
    background-color: %1;
    color: #FFFFFF;
    border-radius: 4px;
}
QMenu {
    background-color: #181825;
    border: 1px solid #313244;
    border-radius: 6px;
}
QMenu::item {
    padding: 5px 20px 5px 20px;
    color: #CDD6F4;
}
QMenu::item:selected {
    background-color: #2A2640;
    color: %4;
}
QMenu::item:disabled {
    color: %5;
}
QStatusBar {
    background-color: #181825;
    border-top: 1px solid #313244;
    color: #7F849C;
}
QScrollBar:vertical {
    width: 8px;
    background: transparent;
}
QScrollBar::handle:vertical {
    background: #45475A;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::handle:vertical:hover { background: %1; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QLabel { background: transparent; }
)").arg(accent, accentHover, accentPress, secondaryColor, disabledMenuColor);
        }

        qApp->setStyleSheet(qss);
    }
};

#endif // THEMEMANAGER_H