#include "auditLog_window.h"
#include "ui_auditLog_window.h"
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

AuditLogWindow::AuditLogWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AuditLogWindow)
{
    ui->setupUi(this);
    setWindowTitle("Журнал аудита");
    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));

    setMinimumSize(1200, 700);

    setupUI();
    loadAuditLog();
}

AuditLogWindow::~AuditLogWindow()
{
    delete ui;
}

void AuditLogWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}

void AuditLogWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel("Журнал аудита");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    QPushButton *exportButton = new QPushButton("Экспорт");
    exportButton->setStyleSheet(
        "QPushButton {"
        "    background: #27ae60;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #219a52;"
        "}"
        );

    QPushButton *refreshButton = new QPushButton("Обновить");
    refreshButton->setStyleSheet(
        "QPushButton {"
        "    background: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #2980b9;"
        "}"
        );

    connect(exportButton, &QPushButton::clicked, this, &AuditLogWindow::onExportClicked);
    connect(refreshButton, &QPushButton::clicked, this, &AuditLogWindow::onRefreshClicked);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(exportButton);
    headerLayout->addWidget(refreshButton);

    QGroupBox *filtersGroup = new QGroupBox("Фильтры");
    QHBoxLayout *filtersLayout = new QHBoxLayout(filtersGroup);

    QLabel *dateFromLabel = new QLabel("С:");
    QDateEdit *dateFromEdit = new QDateEdit();
    dateFromEdit->setDate(QDate::currentDate().addDays(-7));
    dateFromEdit->setCalendarPopup(true);

    QLabel *dateToLabel = new QLabel("По:");
    QDateEdit *dateToEdit = new QDateEdit();
    dateToEdit->setDate(QDate::currentDate());
    dateToEdit->setCalendarPopup(true);

    QLabel *eventTypeLabel = new QLabel("Тип события:");
    QComboBox *eventTypeCombo = new QComboBox();
    eventTypeCombo->addItems({"Все", "Вход в систему", "Выход", "Создание", "Редактирование", "Удаление", "Блокировка"});

    QLabel *userLabel = new QLabel("Пользователь:");
    QComboBox *userCombo = new QComboBox();
    userCombo->addItems({"Все", "admin", "user1", "operator", "moderator"});

    QLabel *statusLabel = new QLabel("Статус:");
    QComboBox *statusCombo = new QComboBox();
    statusCombo->addItems({"Все", "Успешно", "Ошибка", "Предупреждение"});

    QLabel *searchLabel = new QLabel("Поиск:");
    QLineEdit *searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Поиск по описанию...");
    QPushButton *searchButton = new QPushButton("Найти");

    connect(searchButton, &QPushButton::clicked, this, &AuditLogWindow::onSearchClicked);

    filtersLayout->addWidget(dateFromLabel);
    filtersLayout->addWidget(dateFromEdit);
    filtersLayout->addWidget(dateToLabel);
    filtersLayout->addWidget(dateToEdit);
    filtersLayout->addWidget(eventTypeLabel);
    filtersLayout->addWidget(eventTypeCombo);
    filtersLayout->addWidget(userLabel);
    filtersLayout->addWidget(userCombo);
    filtersLayout->addWidget(statusLabel);
    filtersLayout->addWidget(statusCombo);
    filtersLayout->addWidget(searchLabel);
    filtersLayout->addWidget(searchEdit);
    filtersLayout->addWidget(searchButton);

    QGroupBox *statsGroup = new QGroupBox("Статистика журнала");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    QLabel *totalEventsLabel = new QLabel("1,247\nВсего событий");
    QLabel *todayEventsLabel = new QLabel("45\nСегодня");
    QLabel *errorsLabel = new QLabel("23\nОшибок");
    QLabel *securityLabel = new QLabel("12\nБезопасность");
    QLabel *usersActivityLabel = new QLabel("18\nАктивных пользователей");

    QString statStyle = "QLabel { font-size: 14px; text-align: center; }";
    totalEventsLabel->setStyleSheet(statStyle + "color: #2c3e50;");
    todayEventsLabel->setStyleSheet(statStyle + "color: #3498db;");
    errorsLabel->setStyleSheet(statStyle + "color: #e74c3c;");
    securityLabel->setStyleSheet(statStyle + "color: #f39c12;");
    usersActivityLabel->setStyleSheet(statStyle + "color: #27ae60;");

    totalEventsLabel->setAlignment(Qt::AlignCenter);
    todayEventsLabel->setAlignment(Qt::AlignCenter);
    errorsLabel->setAlignment(Qt::AlignCenter);
    securityLabel->setAlignment(Qt::AlignCenter);
    usersActivityLabel->setAlignment(Qt::AlignCenter);

    statsLayout->addWidget(totalEventsLabel);
    statsLayout->addWidget(todayEventsLabel);
    statsLayout->addWidget(errorsLabel);
    statsLayout->addWidget(securityLabel);
    statsLayout->addWidget(usersActivityLabel);

    QGroupBox *logGroup = new QGroupBox("Журнал событий");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);

    QTableWidget *auditTable = new QTableWidget(0, 6);
    auditTable->setHorizontalHeaderLabels({
        "Время",
        "Пользователь",
        "Событие",
        "Объект",
        "Статус",
        "Детали"
    });

    auditTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    auditTable->setSelectionMode(QAbstractItemView::SingleSelection);
    auditTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auditTable->horizontalHeader()->setStretchLastSection(true);
    auditTable->setSortingEnabled(true);

    logLayout->addWidget(auditTable);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(filtersGroup);
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(logGroup);

    setCentralWidget(centralWidget);
}

void AuditLogWindow::loadAuditLog()
{
    // TODO: Загрузка реальных данных из API
    // Временные тестовые данные

    QTableWidget *table = findChild<QTableWidget*>();
    if (!table) return;

    table->setRowCount(0);

    QVector<QStringList> auditLog = {
        {"2024-03-20 14:30:15", "admin", "Вход в систему", "Система", "Успешно", "IP: 192.168.1.100"},
        {"2024-03-20 14:25:30", "user1", "Смена пароля", "user1", "Успешно", "-"},
        {"2024-03-20 14:20:45", "moderator", "Блокировка пользователя", "user456", "Успешно", "Причина: нарушение правил"},
        {"2024-03-20 14:15:20", "admin", "Создание пользователя", "newuser", "Успешно", "Роль: Оператор"},
        {"2024-03-20 14:10:35", "user456", "Попытка входа", "Система", "Ошибка", "Неверный пароль"},
        {"2024-03-20 14:05:50", "operator", "Редактирование профиля", "user789", "Успешно", "Изменен email"},
        {"2024-03-20 14:00:25", "admin", "Просмотр статистики", "Статистика", "Успешно", "-"},
        {"2024-03-20 13:55:40", "user789", "Вход в систему", "Система", "Успешно", "IP: 192.168.1.105"},
        {"2024-03-20 13:50:15", "moderator", "Разблокировка", "user123", "Успешно", "-"},
        {"2024-03-20 13:45:30", "admin", "Назначение роли", "user001", "Успешно", "Роль: Пользователь → Оператор"},
        {"2024-03-20 13:40:20", "user123", "Попытка доступа", "Админ панель", "Ошибка", "Недостаточно прав"},
        {"2024-03-20 13:35:10", "admin", "Экспорт данных", "Пользователи", "Успешно", "Формат: CSV"}
    };

    table->setRowCount(auditLog.size());

    for (int i = 0; i < auditLog.size(); ++i) {
        for (int j = 0; j < auditLog[i].size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(auditLog[i][j]);

            if (j == 4) {
                if (auditLog[i][j] == "Успешно") {
                    item->setForeground(Qt::darkGreen);
                    item->setBackground(QColor(230, 245, 230));
                } else if (auditLog[i][j] == "Ошибка") {
                    item->setForeground(Qt::darkRed);
                    item->setBackground(QColor(255, 230, 230));
                } else if (auditLog[i][j] == "Предупреждение") {
                    item->setForeground(QColor(255, 140, 0));
                    item->setBackground(QColor(255, 245, 230));
                }
            }

            if (j == 2 && (auditLog[i][j].contains("Блокировка") ||
                           auditLog[i][j].contains("Попытка доступа") ||
                           auditLog[i][j].contains("Недостаточно прав"))) {
                item->setForeground(QColor(192, 57, 43));
                item->setFont(QFont("", -1, QFont::Bold));
            }

            table->setItem(i, j, item);
        }
    }
}

void AuditLogWindow::onRefreshClicked()
{
    loadAuditLog();
}

void AuditLogWindow::onFilterChanged()
{
    applyFilters();
}

void AuditLogWindow::onSearchClicked()
{
    applyFilters();
}

void AuditLogWindow::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Экспорт журнала аудита",
        QDir::homePath() + "/Журнал_аудита_" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".csv",
        "CSV Files (*.csv)"
        );

    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл");
        return;
    }

    QTextStream stream(&file);
    stream.setGenerateByteOrderMark(true);

    QTableWidget *table = findChild<QTableWidget*>();
    if (!table) return;

    QStringList headers;
    for (int i = 0; i < table->columnCount(); ++i) {
        headers << table->horizontalHeaderItem(i)->text();
    }
    stream << headers.join(";") << "\n";

    for (int row = 0; row < table->rowCount(); ++row) {
        QStringList rowData;

        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);
            QString text = item ? item->text() : "";

            if (col == 0 && !text.isEmpty()) {
                text = "=\"" + text + "\"";
            }

            rowData << text;
        }
        stream << rowData.join(";") << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", "Журнал аудита экспортирован");
}

void AuditLogWindow::applyFilters()
{
    // TODO: Реализация фильтрации данныхs
    loadAuditLog();
}
