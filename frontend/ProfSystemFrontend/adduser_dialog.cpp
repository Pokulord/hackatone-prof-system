#include "adduser_dialog.h"
#include "ui_adduser_dialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>

AddUserDialog::AddUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUserDialog)
{
    ui->setupUi(this);

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
            )
        );

    QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    ui->usernameEdit->setValidator(new QRegularExpressionValidator(usernameRegex, this));

    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &AddUserDialog::validateForm);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &AddUserDialog::validateForm);

    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &AddUserDialog::on_showPasswordCheckbox_toggled);
    connect(ui->createButton, &QPushButton::clicked, this, &AddUserDialog::on_createButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddUserDialog::on_cancelButton_clicked);

    ui->usernameEdit->setFocus();
    ui->errorLabel->setStyleSheet("color: #d32f2f;");
}

AddUserDialog::~AddUserDialog()
{
    ui->passwordEdit->setText(QString().fill('*', 10));
    delete ui;
}

void AddUserDialog::on_showPasswordCheckbox_toggled(bool checked)
{
    QString style = QString(
                        "QCheckBox {"
                        "    background: white;"
                        "    spacing: 8px;"
                        "}"
                        "QCheckBox::indicator {"
                        "    width: 18px;"
                        "    height: 18px;"
                        "    border: 2px solid %1;"
                        "    border-radius: 3px;"
                        "    background: %2;"
                        "}"
                        "QCheckBox::indicator:checked {"
                        "    background: %3;"
                        "    border-color: %3;"
                        "}"
                        ).arg(
                            checked ? "#3498db" : "#bdc3c7",
                            checked ? "#3498db" : "white",
                            "#3498db"
                            );

    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    ui->showPasswordCheckbox->setStyleSheet(style);
}

void AddUserDialog::on_createButton_clicked()
{
    if (validateForm()) {
        accept();
    }
}

void AddUserDialog::on_cancelButton_clicked()
{
    reject();
}

bool AddUserDialog::validateForm()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->errorLabel->setText("Все поля обязательны для заполнения");
        ui->createButton->setEnabled(false);
        return false;
    }

    if (username.length() < 3) {
        ui->errorLabel->setText("Логин должен быть не менее 3 символов");
        ui->createButton->setEnabled(false);
        return false;
    }

    if (password.length() < 8) {
        ui->errorLabel->setText("Пароль должен быть не менее 8 символов");
        ui->createButton->setEnabled(false);
        return false;
    }

    if (!ui->usernameEdit->hasAcceptableInput()) {
        ui->errorLabel->setText("Логин может содержать только буквы, цифры и подчеркивание");
        ui->createButton->setEnabled(false);
        return false;
    }

    if (password.length() < 6) {
        ui->errorLabel->setText("Пароль должен быть не менее 6 символов");
        ui->createButton->setEnabled(false);
        return false;
    }

    ui->errorLabel->clear();
    ui->createButton->setEnabled(true);
    return true;
}

bool AddUserDialog::validatePassword(const QString &password)
{
    return password.length() >= 6;
}

QString AddUserDialog::username() const { return ui->usernameEdit->text().trimmed(); }
QString AddUserDialog::password() const { return ui->passwordEdit->text(); }
QString AddUserDialog::role() const { return ui->roleCombo->currentText(); }
