#include "activeSessions_window.h"
#include "ui_activeSessions_window.h"
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

ActiveSessionsWindow::ActiveSessionsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ActiveSessionsWindow)
{
    ui->setupUi(this);
    setWindowTitle("Активные сессии");
    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));

    setMinimumSize(1000, 600);

    setupUI();
    loadSessions();
}

ActiveSessionsWindow::~ActiveSessionsWindow()
{
    delete ui;
}

void ActiveSessionsWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}

void ActiveSessionsWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel("Активные сессии");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    QPushButton *killSessionButton = new QPushButton("Завершить сессию");
    killSessionButton->setStyleSheet(
        "QPushButton {"
        "    background: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #c0392b;"
        "}"
        "QPushButton:disabled {"
        "    background: #bdc3c7;"
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

    connect(killSessionButton, &QPushButton::clicked, this, &ActiveSessionsWindow::onKillSessionClicked);
    connect(refreshButton, &QPushButton::clicked, this, &ActiveSessionsWindow::onRefreshClicked);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(killSessionButton);
    headerLayout->addWidget(refreshButton);

    QGroupBox *statsGroup = new QGroupBox("Статистика сессий");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    QLabel *totalSessionsLabel = new QLabel("23\nВсего сессий");
    QLabel *activeNowLabel = new QLabel("18\nАктивных сейчас");
    QLabel *longSessionsLabel = new QLabel("5\nДолгих сессий (>2ч)");
    QLabel *suspiciousLabel = new QLabel("2\nПодозрительных");

    QString statStyle = "QLabel { font-size: 16px; text-align: center; }";
    totalSessionsLabel->setStyleSheet(statStyle + "color: #2c3e50;");
    activeNowLabel->setStyleSheet(statStyle + "color: #27ae60;");
    longSessionsLabel->setStyleSheet(statStyle + "color: #f39c12;");
    suspiciousLabel->setStyleSheet(statStyle + "color: #e74c3c;");

    totalSessionsLabel->setAlignment(Qt::AlignCenter);
    activeNowLabel->setAlignment(Qt::AlignCenter);
    longSessionsLabel->setAlignment(Qt::AlignCenter);
    suspiciousLabel->setAlignment(Qt::AlignCenter);

    statsLayout->addWidget(totalSessionsLabel);
    statsLayout->addWidget(activeNowLabel);
    statsLayout->addWidget(longSessionsLabel);
    statsLayout->addWidget(suspiciousLabel);

    QGroupBox *sessionsGroup = new QGroupBox("Активные сессии");
    QVBoxLayout *sessionsLayout = new QVBoxLayout(sessionsGroup);

    QTableWidget *sessionsTable = new QTableWidget(0, 6);
    sessionsTable->setHorizontalHeaderLabels({
        "Пользователь",
        "IP адрес",
        "Время входа",
        "Длительность",
        "Статус",
        "Устройство"
    });

    sessionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    sessionsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    sessionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sessionsTable->horizontalHeader()->setStretchLastSection(true);

    sessionsLayout->addWidget(sessionsTable);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(sessionsGroup);

    setCentralWidget(centralWidget);
}

void ActiveSessionsWindow::loadSessions()
{
    // TODO: Загрузка реальных данных из API
    // Временные тестовые данные

    QTableWidget *table = findChild<QTableWidget*>();
    if (!table) return;

    table->setRowCount(0);

    QVector<QStringList> sessions = {
        {"admin", "192.168.1.100", "14:30", "2ч 15мин", "Активна", "Chrome Windows"},
        {"user1", "192.168.1.101", "15:45", "45мин", "Активна", "Firefox Linux"},
        {"operator", "192.168.1.102", "13:20", "3ч 10мин", "Активна", "Safari MacOS"},
        {"moderator", "192.168.1.105", "16:00", "15мин", "Активна", "Mobile Android"},
        {"user2", "192.168.1.110", "12:00", "4ч 30мин", "Неактивна", "Chrome Windows"}
    };

    table->setRowCount(sessions.size());

    for (int i = 0; i < sessions.size(); ++i) {
        for (int j = 0; j < sessions[i].size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(sessions[i][j]);

            if (j == 4) {
                if (sessions[i][j] == "Активна") {
                    item->setForeground(Qt::darkGreen);
                    item->setBackground(QColor(230, 245, 230));
                } else {
                    item->setForeground(Qt::darkRed);
                    item->setBackground(QColor(255, 230, 230));
                }
            }

            if (j == 3 && sessions[i][j].contains("ч") && sessions[i][j].split("ч")[0].toInt() > 2) {
                item->setForeground(QColor(255, 140, 0));
            }

            table->setItem(i, j, item);
        }
    }
}

void ActiveSessionsWindow::onRefreshClicked()
{
    loadSessions();
}

void ActiveSessionsWindow::onKillSessionClicked()
{
    QTableWidget *table = findChild<QTableWidget*>();
    if (!table) return;

    int currentRow = table->currentRow();
    if (currentRow == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите сессию для завершения");
        return;
    }

    QString username = table->item(currentRow, 0)->text();
    QString ip = table->item(currentRow, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение",
        QString("Завершить сессию пользователя %1 с IP %2?").arg(username).arg(ip),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // TODO: Вызов API для завершения сессии
        table->removeRow(currentRow);

        QMessageBox::information(this, "Успех",
                                 QString("Сессия пользователя %1 завершена").arg(username));
    }
}
