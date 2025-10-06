#include "auth_window.h"
#include "./ui_auth_window.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Прософт - Авторизация");

    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));

    ui->errorLabel->setMinimumHeight(0);
    ui->errorLabel->setMaximumHeight(30);
    ui->errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QFont defaultFont("Arial", 12);
    QFont smallFont("Arial", 10);

    ui->usernameLabel->setFont(defaultFont);
    ui->passwordLabel->setFont(defaultFont);
    ui->usernameEdit->setFont(defaultFont);
    ui->passwordEdit->setFont(defaultFont);
    ui->loginButton->setFont(defaultFont);
    ui->showPasswordCheckbox->setFont(smallFont);
    ui->errorLabel->setFont(smallFont);

    ui->errorLabel->setStyleSheet("color: #d32f2f;");
    ui->errorLabel->setVisible(false);

    ui->loginButton->setStyleSheet(
        "QPushButton {"
            "background: #3498db;"
            "color: white;"
            "border: none;"
            "padding: 12px 30px;"
            "border-radius: 5px;"
            "font-size: 12pt;"
            "font-weight: bold;"
            "min-width: 120px;"
            "outline: none;"
        "}"
        "QPushButton:hover { background: #2980b9; }"
        "QPushButton:pressed { background: #21618c; }"
    );

    ui->showPasswordCheckbox->setStyleSheet(
        "QCheckBox {"
            "font-size: 10pt;"
            "color: #7f8c8d;"
            "outline: none;"
        "}"
        "QCheckBox::indicator {"
            "width: 16px;"
            "height: 16px;"
        "}"
        "QCheckBox::indicator:unchecked {"
            "border: 2px solid #bdc3c7;"
            "border-radius: 3px;"
        "}"
        "QCheckBox::indicator:checked {"
            "background: #3498db;"
            "border: 2px solid #3498db;"
        "}"
        "QCheckBox:focus {"
            "outline: none;"
            "border: none;"
        "}"
        );

    ui->usernameEdit->setStyleSheet(
        "QLineEdit {"
            "padding: 8px;"
            "border: 2px solid #bdc3c7;"
            "border-radius: 5px;"
            "font-size: 12pt;"
        "}"
        "QLineEdit:focus { border-color: #3498db; }"
    );

    ui->passwordEdit->setStyleSheet(
        "QLineEdit {"
            "padding: 8px;"
            "border: 2px solid #bdc3c7;"
            "border-radius: 5px;"
            "font-size: 12pt;"
        "}"
        "QLineEdit:focus { border-color: #3498db; }"
    );

    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(ui->centralwidget);
    QVBoxLayout *verticalLayout = new QVBoxLayout();

    horizontalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addStretch();

    verticalLayout->addStretch();
    verticalLayout->addWidget(ui->groupBox);
    verticalLayout->addStretch();

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
        ui->errorLabel->setVisible(false);
    }
    else {
        ui->errorLabel->setText("Неверный логин или пароль");
        ui->errorLabel->setVisible(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
