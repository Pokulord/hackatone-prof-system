#include "admin_window.h"
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
    statusBar()->showMessage("Открыта статистика");
}

void AdminWindow::onUsersClicked() {
    UsersInfoWindow *usersInfoWindow = new UsersInfoWindow();
    usersInfoWindow->show();
    this->hide();
}

void AdminWindow::onSessionsClicked() {
    statusBar()->showMessage("Активные сессии");
}

void AdminWindow::onLogsClicked() {
    statusBar()->showMessage("Журнал аудита");
}

AdminWindow::~AdminWindow()
{
    delete ui;
}
