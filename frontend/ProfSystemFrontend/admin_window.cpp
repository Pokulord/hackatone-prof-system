#include "activeSessions_window.h"
#include "admin_window.h"
#include "auditLog_window.h"
#include "statistics_window.h"
#include "ui_admin_window.h"
#include "usersInfo_window.h"
#include <QStatusBar>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    setWindowTitle("Прософт - Панель администратора");
    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));

    centerContent();

    connect(ui->statsButton, &QPushButton::clicked, this, &AdminWindow::onStatisticsClicked);
    connect(ui->usersButton, &QPushButton::clicked, this, &AdminWindow::onUsersClicked);
    connect(ui->sessionsButton, &QPushButton::clicked, this, &AdminWindow::onSessionsClicked);
    connect(ui->auditButton, &QPushButton::clicked, this, &AdminWindow::onLogsClicked);
}

void AdminWindow::centerContent()
{
    QGridLayout *grid = ui->gridLayout;
    grid->setAlignment(Qt::AlignCenter);

    ui->centralwidget->setLayout(new QVBoxLayout());
    ui->centralwidget->layout()->setAlignment(Qt::AlignCenter);
    ui->centralwidget->layout()->addWidget(ui->centralwidget_2);
}

void AdminWindow::onStatisticsClicked() {
    StatisticsWindow *statsWindow = new StatisticsWindow();
    statsWindow->setAttribute(Qt::WA_DeleteOnClose);
    statsWindow->show();
    this->hide();

    connect(statsWindow, &StatisticsWindow::windowClosed, this, [this]() {
        this->show();
        this->activateWindow();
    });
}

void AdminWindow::onUsersClicked() {
    UsersInfoWindow *usersInfoWindow = new UsersInfoWindow();
    usersInfoWindow->show();
    this->hide();

    connect(usersInfoWindow, &UsersInfoWindow::windowClosed, this, [this]() {
        this->show();
        this->activateWindow();
    });
}

void AdminWindow::onSessionsClicked() {
    ActiveSessionsWindow *sessionsWindow = new ActiveSessionsWindow();
    sessionsWindow->setAttribute(Qt::WA_DeleteOnClose);
    sessionsWindow->show();
    this->hide();

    connect(sessionsWindow, &ActiveSessionsWindow::windowClosed, this, [this]() {
        this->show();
        this->activateWindow();
    });
}

void AdminWindow::onLogsClicked() {
    AuditLogWindow *sessionsWindow = new AuditLogWindow();
    sessionsWindow->setAttribute(Qt::WA_DeleteOnClose);
    sessionsWindow->show();
    this->hide();

    connect(sessionsWindow, &AuditLogWindow::windowClosed, this, [this]() {
        this->show();
        this->activateWindow();
    });
}

AdminWindow::~AdminWindow()
{
    delete ui;
}
