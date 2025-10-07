#include "auth_window.h"
#include "ui_auth_window.h"
#include "change_adminpass_window.h"
#include "admin_window.h"
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

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AuthWindow)
{
    ui->setupUi(this);
    initializeWindow();
    setupStyles();
    setupLayout();
    setupConnections();
}

void AuthWindow::initializeWindow()
{
    setWindowTitle("Прософт - Авторизация");
    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));
}

void AuthWindow::setupStyles()
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
    ui->errorLabel->setMinimumHeight(0);
    ui->errorLabel->setMaximumHeight(30);
    ui->errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->errorLabel->setStyleSheet(QString("color: %1;").arg(StyleConstants::ERROR_COLOR));
    ui->errorLabel->setVisible(false);
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

void AuthWindow::setupLayout()
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

void AuthWindow::setupConnections()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &AuthWindow::onLoginClicked);
    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &AuthWindow::onShowPasswordToggled);
}

void AuthWindow::onShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void AuthWindow::onLoginClicked()
{
    const QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

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

    // TODO: Заменить хардкод на вызов API бэкенда
    if (username == "admin" && password == "admin"){
        handleSuccessfulLogin();
    } else {
        handleFailedLogin();
    }

    password.fill('0');
}

void AuthWindow::handleSuccessfulLogin()
{
    qDebug() << "Успешный вход!";
    ui->errorLabel->setVisible(false);

    // TODO: Получить от бэкенда флаг необходимости смены пароля
    bool needPasswordChange = true;

    if (needPasswordChange) {
        showPasswordChangeDialog();
    } else {
        openAdminWindow();
    }
}

void AuthWindow::showPasswordChangeDialog()
{
    ChangeAdminPassWindow dialog(this);

    int result = dialog.exec();

    if (result == QDialog::Accepted) {
        qDebug() << "Пароль успешно изменен, открываем админку";
        openAdminWindow();
    }
}

void AuthWindow::openAdminWindow()
{
    AdminWindow *adminWindow = new AdminWindow();
    adminWindow->show();

    this->close();
}

void AuthWindow::handleFailedLogin()
{
    ui->errorLabel->setText("Неверный логин или пароль");
    ui->errorLabel->setVisible(true);
}

AuthWindow::~AuthWindow()
{
    delete ui;
}
