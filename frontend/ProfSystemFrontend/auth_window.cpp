#include "auth_window.h"
#include "./ui_auth_window.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

namespace StyleConstants {
const QString FONT_FAMILY = "Arial";
const int DEFAULT_FONT_SIZE = 12;
const int SMALL_FONT_SIZE = 10;

const QString PRIMARY_COLOR = "#3498db";
const QString HOVER_COLOR = "#2980b9";
const QString PRESSED_COLOR = "#21618c";
const QString ERROR_COLOR = "#d32f2f";
const QString BORDER_COLOR = "#bdc3c7";
const QString TEXT_COLOR = "#7f8c8d";

const QString BUTTON_STYLE =
    "QPushButton {"
    "background: %1;"
    "color: white;"
    "border: none;"
    "padding: 12px 30px;"
    "border-radius: 5px;"
    "font-size: 12pt;"
    "font-weight: bold;"
    "min-width: 120px;"
    "outline: none;"
    "}"
    "QPushButton:hover { background: %2; }"
    "QPushButton:pressed { background: %3; }";

const QString LINE_EDIT_STYLE =
    "QLineEdit {"
    "padding: 8px;"
    "border: 2px solid %1;"
    "border-radius: 5px;"
    "font-size: 12pt;"
    "}"
    "QLineEdit:focus { border-color: %2; }";

const QString CHECKBOX_STYLE =
    "QCheckBox {"
    "font-size: 10pt;"
    "color: %1;"
    "outline: none;"
    "}"
    "QCheckBox::indicator {"
    "width: 16px;"
    "height: 16px;"
    "}"
    "QCheckBox::indicator:unchecked {"
    "border: 2px solid %2;"
    "border-radius: 3px;"
    "}"
    "QCheckBox::indicator:checked {"
    "background: %3;"
    "border: 2px solid %3;"
    "}"
    "QCheckBox:focus {"
    "outline: none;"
    "border: none;"
    "}";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeWindow();
    setupStyles();
    setupLayout();
    setupConnections();
}

void MainWindow::initializeWindow()
{
    setWindowTitle("Прософт - Авторизация");
    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));

    ui->errorLabel->setMinimumHeight(0);
    ui->errorLabel->setMaximumHeight(30);
    ui->errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->errorLabel->setStyleSheet(QString("color: %1;").arg(StyleConstants::ERROR_COLOR));
    ui->errorLabel->setVisible(false);
}

void MainWindow::setupStyles()
{
    QFont defaultFont(StyleConstants::FONT_FAMILY, StyleConstants::DEFAULT_FONT_SIZE);
    QFont smallFont(StyleConstants::FONT_FAMILY, StyleConstants::SMALL_FONT_SIZE);

    ui->usernameLabel->setFont(defaultFont);
    ui->passwordLabel->setFont(defaultFont);
    ui->usernameEdit->setFont(defaultFont);
    ui->passwordEdit->setFont(defaultFont);
    ui->usernameEdit->setPlaceholderText("Введите логин...");
    ui->passwordEdit->setPlaceholderText("Введите пароль...");
    ui->loginButton->setFont(defaultFont);
    ui->showPasswordCheckbox->setFont(smallFont);
    ui->errorLabel->setFont(smallFont);

    ui->loginButton->setStyleSheet(
        StyleConstants::BUTTON_STYLE
            .arg(StyleConstants::PRIMARY_COLOR)
            .arg(StyleConstants::HOVER_COLOR)
            .arg(StyleConstants::PRESSED_COLOR)
        );

    ui->showPasswordCheckbox->setStyleSheet(
        StyleConstants::CHECKBOX_STYLE
            .arg(StyleConstants::TEXT_COLOR)
            .arg(StyleConstants::BORDER_COLOR)
            .arg(StyleConstants::PRIMARY_COLOR)
        );

    ui->usernameEdit->setStyleSheet(
        StyleConstants::LINE_EDIT_STYLE
            .arg(StyleConstants::BORDER_COLOR)
            .arg(StyleConstants::PRIMARY_COLOR)
        );

    ui->passwordEdit->setStyleSheet(
        StyleConstants::LINE_EDIT_STYLE
            .arg(StyleConstants::BORDER_COLOR)
            .arg(StyleConstants::PRIMARY_COLOR)
        );

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

void MainWindow::setupLayout()
{
    QHBoxLayout *horizontalLayout = new QHBoxLayout(ui->centralwidget);
    QVBoxLayout *verticalLayout = new QVBoxLayout();

    horizontalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addStretch();

    verticalLayout->addStretch();
    verticalLayout->addWidget(ui->groupBox);
    verticalLayout->addStretch();
}

void MainWindow::setupConnections()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &MainWindow::onShowPasswordToggled);
}

void MainWindow::onShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void MainWindow::onLoginClicked()
{
    const QString username = ui->usernameEdit->text();
    const QString password = ui->passwordEdit->text();

    if (username.isEmpty()) {
        ui->errorLabel->setText("Введите логин");
        ui->errorLabel->setVisible(true);
        ui->usernameEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        ui->errorLabel->setText("Введите пароль");
        ui->errorLabel->setVisible(true);
        ui->passwordEdit->setFocus();
        return;
    }

    bool authSuccess = authenticateUser(username, password);

    password.fill('0');

    if (authSuccess) {
        handleSuccessfulLogin();
    } else {
        handleFailedLogin();
        addLoginAttempt(username);
    }
}

void MainWindow::handleSuccessfulLogin()
{
    qDebug() << "Успешный вход!";
    ui->errorLabel->setVisible(false);
    // TODO: Переход к главному окну
}

void MainWindow::handleFailedLogin()
{
    ui->errorLabel->setText("Неверный логин или пароль");
    ui->errorLabel->setVisible(true);
}

void MainWindow::addLoginAttempt(const QString& username)
{
    failedAttempts[username]++;
    if (failedAttempts[username] >= 5) {
        ui->errorLabel->setText("Слишком много попыток. Попробуйте через 5 минут");
        ui->loginButton->setEnabled(false);
        QTimer::singleShot(300000, this, [this]() { // 5 минут
            ui->loginButton->setEnabled(true);
            failedAttempts.clear();
        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
