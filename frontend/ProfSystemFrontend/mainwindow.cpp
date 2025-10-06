#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

// В конструкторе MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->errorLabel->setVisible(false);

    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);

    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, [this](bool checked) {
        ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
}

void MainWindow::onLoginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    if (username == "admin" && password == "admin") {
        qDebug() << "Успешный вход!";
    } else {
        ui->errorLabel->setText("Неверный логин или пароль");
        ui->errorLabel->setVisible(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
