#include "change_adminpass_window.h"
#include "./ui_change_adminpass_window.h"
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

ChangeAdminPassWindow::ChangeAdminPassWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChangeAdminPassWindow)
{
    ui->setupUi(this);

    setWindowTitle("Смена пароля администратора");
    setFixedSize(500, 400); // УВЕЛИЧИЛ РАЗМЕР

    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    setupStyles();

    setupCenteredLayout();

    connect(ui->pushButton, &QPushButton::clicked, this, &ChangeAdminPassWindow::onChangePasswordClicked);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &ChangeAdminPassWindow::onPasswordTextChanged);
    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &ChangeAdminPassWindow::onShowPasswordToggled);

    ui->errorLabel->setVisible(false);
    ui->passwordEdit->setFocus();
}

void ChangeAdminPassWindow::setupStyles()
{
    QFont defaultFont("Arial", 12);
    QFont smallFont("Arial", 10);
    QFont titleFont("Arial", 16, QFont::Bold);

    ui->titleLabel->setFont(titleFont);
    ui->passwordEdit->setFont(defaultFont);
    ui->pushButton->setFont(defaultFont);
    ui->errorLabel->setFont(smallFont);
    ui->showPasswordCheckbox->setFont(smallFont);

    ui->pushButton->setStyleSheet(
        "QPushButton {"
        "   background: #3498db;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px 30px;"
        "   border-radius: 5px;"
        "   font-size: 12pt;"
        "   font-weight: bold;"
        "   min-width: 120px;"
        "   outline: none;"
        "}"
        "QPushButton:hover { background: #2980b9; }"
        "QPushButton:pressed { background: #21618c; }"
        "QPushButton:disabled { background: #bdc3c7; }");

    ui->passwordEdit->setStyleSheet(
        "QLineEdit {"
        "   padding: 8px;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 5px;"
        "   font-size: 12pt;"
        "}"
        "QLineEdit:focus { border-color: #3498db; }");

    ui->showPasswordCheckbox->setStyleSheet(
        "QCheckBox {"
        "   font-size: 10pt;"
        "   color: #7f8c8d;"
        "   outline: none;"
        "}"
        "QCheckBox::indicator {"
        "   width: 16px;"
        "   height: 16px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 3px;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background: #3498db;"
        "   border: 2px solid #3498db;"
        "}"
        "QCheckBox:focus {"
        "   outline: none;"
        "   border: none;"
        "}");

    ui->errorLabel->setStyleSheet("color: #d32f2f;");

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setPlaceholderText("Введите новый пароль...");
}

void ChangeAdminPassWindow::setupCenteredLayout()
{
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout();

    horizontalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addStretch();

    verticalLayout->addStretch();
    verticalLayout->addWidget(ui->groupBox);
    verticalLayout->addStretch();
}

void ChangeAdminPassWindow::setupUI()
{
    ui->groupBox->setMinimumSize(0, 0);
    ui->groupBox->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->groupBox->setStyleSheet(
        "QGroupBox {"
        "   background: white;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding: 20px;"  // ← ДОБАВЬ ОТСТУПЫ ВНУТРИ
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   padding: 0 5px;"
        "}");

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setPlaceholderText("Введите новый пароль...");

    ui->pushButton->setStyleSheet(
        "QPushButton {"
        "   background: #3498db;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background: #2980b9; }"
        "QPushButton:disabled { background: #bdc3c7; }"
        );

    ui->errorLabel->setStyleSheet("color: #d32f2f; font-size: 10pt;");
    ui->errorLabel->setWordWrap(true);
}

void ChangeAdminPassWindow::onPasswordTextChanged()
{
    QString password = ui->passwordEdit->text();

    // TODO: Изменить валидацию пароля
    if (password.length() < 8 && !password.isEmpty()) {
        ui->errorLabel->setText("Пароль должен содержать не менее 8 символов");
        ui->errorLabel->setVisible(true);
        ui->pushButton->setEnabled(false);
    } else {
        ui->errorLabel->setVisible(false);
        ui->pushButton->setEnabled(!password.isEmpty());
    }

    password.fill('0');
}

void ChangeAdminPassWindow::onChangePasswordClicked()
{
    QString newPassword = ui->passwordEdit->text();

    if (newPassword.length() < 8) {
        ui->errorLabel->setText("Пароль должен содержать не менее 8 символов");
        ui->errorLabel->setVisible(true);
        return;
    }

    // TODO: Отправить новый пароль в бэкенд
    QMessageBox msgBox;
    msgBox.setStyleSheet(
        "QMessageBox {"
        "   background-color: white;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 8px;"
        "   padding: 20px;"
        "}"
        "QMessageBox QLabel {"
        "   color: #2c3e50;"
        "   font-size: 12pt;"
        "   font-family: Arial;"
        "}"
        "QMessageBox QPushButton {"
        "   background: #3498db;"
        "   color: white;"
        "   border: none;"
        "   padding: 10px 20px;"
        "   border-radius: 5px;"
        "   font-size: 11pt;"
        "   font-weight: bold;"
        "   min-width: 100px;"
        "}"
        "QMessageBox QPushButton:hover {"
        "   background: #2980b9;"
        "}"
        );
    msgBox.setWindowTitle("Успех");
    msgBox.setText("Пароль успешно изменен!");
    msgBox.exec();

    accept();
}

void ChangeAdminPassWindow::onShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

ChangeAdminPassWindow::~ChangeAdminPassWindow()
{
    delete ui;
}
