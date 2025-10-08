#include "statistics_window.h"
#include "ui_statistics_window.h"
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>

StatisticsWindow::StatisticsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatisticsWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));
    setWindowTitle("Статистика системы");

    setMinimumSize(900, 700);

    setupUI();
    loadStatistics();
}

StatisticsWindow::~StatisticsWindow()
{
    delete ui;
}

void StatisticsWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}

void StatisticsWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel("Статистика системы");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

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

    connect(refreshButton, &QPushButton::clicked, this, &StatisticsWindow::onRefreshClicked);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(refreshButton);

    QGroupBox *statsGroup = new QGroupBox("Общая статистика системы");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    QLabel *totalUsersLabel = new QLabel("150\nВсего пользователей");
    QLabel *activeSessionsLabel = new QLabel("23\nАктивных сессий");
    QLabel *blockedUsersLabel = new QLabel("5\nЗаблокированных");
    QLabel *newTodayLabel = new QLabel("2\nНовых сегодня");

    QString statStyle = "QLabel { font-size: 16px; text-align: center; }";
    totalUsersLabel->setStyleSheet(statStyle + "color: #2c3e50;");
    activeSessionsLabel->setStyleSheet(statStyle + "color: #27ae60;");
    blockedUsersLabel->setStyleSheet(statStyle + "color: #e74c3c;");
    newTodayLabel->setStyleSheet(statStyle + "color: #3498db;");

    totalUsersLabel->setAlignment(Qt::AlignCenter);
    activeSessionsLabel->setAlignment(Qt::AlignCenter);
    blockedUsersLabel->setAlignment(Qt::AlignCenter);
    newTodayLabel->setAlignment(Qt::AlignCenter);

    statsLayout->addWidget(totalUsersLabel);
    statsLayout->addWidget(activeSessionsLabel);
    statsLayout->addWidget(blockedUsersLabel);
    statsLayout->addWidget(newTodayLabel);

    QGroupBox *activityGroup = new QGroupBox("Активность за период");
    QVBoxLayout *activityLayout = new QVBoxLayout(activityGroup);

    QHBoxLayout *periodLayout = new QHBoxLayout();
    QLabel *periodLabel = new QLabel("Период:");
    QComboBox *periodCombo = new QComboBox();
    periodCombo->addItems({"За сегодня", "За неделю", "За месяц"});

    periodLayout->addWidget(periodLabel);
    periodLayout->addWidget(periodCombo);
    periodLayout->addStretch();

    QTableWidget *activityTable = new QTableWidget(7, 3);
    activityTable->setHorizontalHeaderLabels({"Дата", "Количество входов", "Пиковая активность"});
    activityTable->horizontalHeader()->setStretchLastSection(true);

    activityLayout->addLayout(periodLayout);
    activityLayout->addWidget(activityTable);

    QGroupBox *recentGroup = new QGroupBox("Последние действия");
    QVBoxLayout *recentLayout = new QVBoxLayout(recentGroup);

    QTableWidget *recentTable = new QTableWidget(10, 4);
    recentTable->setHorizontalHeaderLabels({"Время", "Пользователь", "Действие", "Детали"});
    recentTable->horizontalHeader()->setStretchLastSection(true);

    recentLayout->addWidget(recentTable);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(activityGroup);
    mainLayout->addWidget(recentGroup);

    setCentralWidget(centralWidget);
}

void StatisticsWindow::loadStatistics()
{
    // TODO: Загрузка реальных данных
    // Пока заглушки
}

void StatisticsWindow::onRefreshClicked()
{
    loadStatistics();
}
