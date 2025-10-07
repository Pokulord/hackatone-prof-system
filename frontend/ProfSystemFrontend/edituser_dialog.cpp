#include "edituser_dialog.h"
#include "ui_edituser_dialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>

EditUserDialog::EditUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditUserDialog)
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

    ui->roleCombo->clear();
    ui->roleCombo->addItems(UserRoles::getRoleList());
    ui->errorLabel->setStyleSheet("color: red");

    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &EditUserDialog::validateForm);
    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &EditUserDialog::on_showPasswordCheckbox_toggled);
    connect(ui->resetPasswordCheckbox, &QCheckBox::toggled, this, &EditUserDialog::on_resetPasswordCheckbox_toggled);
    connect(ui->saveButton, &QPushButton::clicked, this, &EditUserDialog::on_saveButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EditUserDialog::on_cancelButton_clicked);

    ui->passwordEdit->setEnabled(false);
    ui->showPasswordCheckbox->setEnabled(false);
}

EditUserDialog::~EditUserDialog()
{
    ui->passwordEdit->setText(QString().fill('*', 10));
    delete ui;
}

void EditUserDialog::setUserData(const QString &username, UserRoles::Role role, const QString &status)
{
    ui->usernameLabel->setText(username);
    ui->roleCombo->setCurrentText(UserRoles::getRoleName(role));
    ui->statusCombo->setCurrentText(status);
}

void EditUserDialog::on_showPasswordCheckbox_toggled(bool checked)
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

void EditUserDialog::on_resetPasswordCheckbox_toggled(bool checked)
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

    ui->passwordEdit->setEnabled(checked);
    ui->resetPasswordCheckbox->setStyleSheet(style);
    ui->showPasswordCheckbox->setEnabled(checked);

    if (!checked) {
        ui->passwordEdit->clear();
        ui->showPasswordCheckbox->setChecked(false);
        on_showPasswordCheckbox_toggled(false);
    }
}

void EditUserDialog::on_saveButton_clicked()
{
    if (validateForm()) {
        accept();
    }
}

void EditUserDialog::on_cancelButton_clicked()
{
    reject();
}

bool EditUserDialog::validateForm()
{
    QString password = ui->passwordEdit->text();

    if (ui->resetPasswordCheckbox->isChecked() && !password.isEmpty()) {
        if (!validatePassword(password)) {
            ui->errorLabel->setText("Пароль должен быть не менее 8 символов");
            ui->saveButton->setEnabled(false);
            return false;
        }
    }

    ui->errorLabel->clear();
    ui->saveButton->setEnabled(true);
    return true;
}

bool EditUserDialog::validatePassword(const QString &password)
{
    return password.length() >= 8;
}

UserRoles::Role EditUserDialog::role() const {
    return UserRoles::getRoleFromName(ui->roleCombo->currentText());
}

QString EditUserDialog::status() const {
    return ui->statusCombo->currentText();
}

QString EditUserDialog::password() const {
    return ui->passwordEdit->text();
}

bool EditUserDialog::shouldResetPassword() const {
    return ui->resetPasswordCheckbox->isChecked() && !ui->passwordEdit->text().isEmpty();
}
