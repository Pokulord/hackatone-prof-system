#include "change_adminpass_dialog.h"
#include "ui_change_adminpass_dialog.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

ChangeAdminPassDialog::ChangeAdminPassDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChangeAdminPassDialog)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    setWindowTitle("Смена пароля пользователя");
    setFixedSize(500, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setupStyles();
    setupCenteredLayout();

    connect(ui->pushButton, &QPushButton::clicked, this, &ChangeAdminPassDialog::onChangePasswordClicked);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &ChangeAdminPassDialog::onPasswordTextChanged);
    connect(ui->showPasswordCheckbox, &QCheckBox::toggled, this, &ChangeAdminPassDialog::onShowPasswordToggled);

    ui->errorLabel->setVisible(false);
    ui->passwordEdit->setFocus();
}

ChangeAdminPassDialog::~ChangeAdminPassDialog()
{
    delete ui;
}

void ChangeAdminPassDialog::setupStyles()
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
        "QPushButton { background: #3498db; color: white; border: none; padding: 12px 30px; border-radius: 5px; font-size: 12pt; font-weight: bold; min-width: 120px; outline: none; }"
        "QPushButton:hover { background: #2980b9; }"
        "QPushButton:pressed { background: #21618c; }"
        "QPushButton:disabled { background: #bdc3c7; }"
        );

    ui->passwordEdit->setStyleSheet(
        "QLineEdit { padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; font-size: 12pt; }"
        "QLineEdit:focus { border-color: #3498db; }"
        );

    ui->showPasswordCheckbox->setStyleSheet(
        "QCheckBox { font-size: 10pt; color: #7f8c8d; outline: none; }"
        "QCheckBox::indicator { width: 16px; height: 16px; }"
        "QCheckBox::indicator:unchecked { border: 2px solid #bdc3c7; border-radius: 3px; }"
        "QCheckBox::indicator:checked { background: #3498db; border: 2px solid #3498db; }"
        "QCheckBox:focus { outline: none; border: none; }"
        );

    ui->errorLabel->setStyleSheet("color: #d32f2f;");
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setPlaceholderText("Введите новый пароль...");
}

void ChangeAdminPassDialog::setupCenteredLayout()
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

void ChangeAdminPassDialog::onShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void ChangeAdminPassDialog::onPasswordTextChanged()
{
    QString password = ui->passwordEdit->text();

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

QString ChangeAdminPassDialog::loadJwtToken(const QString& username)
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString tokenFilePath = QDir(homeDir).filePath(".user_jwt.token");
    QFile tokenFile(tokenFilePath);
    if (!tokenFile.open(QIODevice::ReadOnly))
        return QString();
    QByteArray data = tokenFile.readAll();
    tokenFile.close();
    return QString::fromUtf8(data);
}

void ChangeAdminPassDialog::onChangePasswordClicked()
{
    QString newPassword = ui->passwordEdit->text();
    if (newPassword.length() < 8) {
        ui->errorLabel->setText("Пароль должен содержать не менее 8 символов");
        ui->errorLabel->setVisible(true);
        return;
    }
    ui->errorLabel->setVisible(false);

    QString username = "admin"; // Либо возьмите из поля или из внешнего класса
    changeUserPassword(username, newPassword);
}

void ChangeAdminPassDialog::changeUserPassword(const QString& username, const QString& newPassword)
{
    QUrl url(QString("http://127.0.0.1:18080/api/users/%1").arg(username));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString jwtToken = loadJwtToken(username);
    request.setRawHeader("Authorization", ("Bearer " + jwtToken).toUtf8());

    QJsonObject payloadObj;
    payloadObj["password"] = newPassword;
    payloadObj["mustChangePassword"] = false;
    QJsonDocument doc(payloadObj);

    QNetworkReply* reply = networkManager->put(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &ChangeAdminPassDialog::onPasswordChangedInDB);
}

void ChangeAdminPassDialog::onPasswordChangedInDB()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(this, "Успех", "Пароль успешно изменён!");
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка изменения пароля: " + reply->errorString());
    }
    reply->deleteLater();
}
