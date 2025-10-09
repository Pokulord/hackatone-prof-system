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
#include <QNetworkAccessManager>

ChangeAdminPassDialog::ChangeAdminPassDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChangeAdminPassDialog)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    setWindowTitle("Смена пароля администратора");
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

void ChangeAdminPassDialog::onChangePasswordClicked()
{
    QString newPassword = ui->passwordEdit->text();
    if (newPassword.length() < 8) {
        ui->errorLabel->setText("Пароль должен содержать не менее 8 символов");
        ui->errorLabel->setVisible(true);
        return;
    }

    ui->errorLabel->setVisible(false);

    // Выполнить логин: получите логин/пароль админа из соответствующих полей/настроек
    QString adminUsername = "admin";     // Получите, например, из отдельного поля UI
    QString adminPassword = "adminpass"; // Получите безопасно

    login(adminUsername, adminPassword);
}

void ChangeAdminPassDialog::login(const QString& username, const QString& password)
{
    QUrl url("http://127.0.0.1:18080/api/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;
    QJsonDocument doc(json);

    QNetworkReply* reply = networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &ChangeAdminPassDialog::onLoginFinished);
}

void ChangeAdminPassDialog::onLoginFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
        if (!jsonResponse.isNull() && jsonResponse.isObject()) {
            QJsonObject obj = jsonResponse.object();
            if (obj.contains("accesstoken")) {
                QString token = obj["accesstoken"].toString();
                if (saveJwtTokenToFile(token)) {
                    QMessageBox::information(this, "Успех", "JWT сохранён.");
                } else {
                    QMessageBox::warning(this, "Внимание", "JWT получен, но сохранить не удалось.");
                }
                // Здесь далее можно отправлять запрос на смену пароля с этим JWT токеном
            }
        }
    } else {
        QMessageBox::critical(this, "Ошибка сети", reply->errorString());
    }
    reply->deleteLater();
}




void ChangeAdminPassDialog::onPasswordChangedInDB()
{
    // Реализуйте/допишите нужную логику тут
    qDebug() << "Слот: onPasswordChangedInDB вызван!";
}

void ChangeAdminPassDialog::onMustChangePasswordUpdated()
{
    // Реализуйте/допишите нужную логику тут
    qDebug() << "Слот: onMustChangePasswordUpdated вызван!";
}


bool ChangeAdminPassDialog::saveJwtTokenToFile(const QString& token)
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        qWarning() << "Не удалось получить директорию для конфигурационных файлов!";
        return false;
    }

    QDir dir(configDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Не удалось создать конфиг директорию:" << configDir;
        return false;
    }

    QString tokenFilePath = dir.filePath("admin_jwt.token");
    QFile tokenFile(tokenFilePath);
    if (!tokenFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Не удалось открыть файл для записи токена:" << tokenFilePath;
        return false;
    }

    QByteArray data = token.toUtf8();
    if (tokenFile.write(data) != data.size()) {
        tokenFile.close();
        qWarning() << "Не удалось записать токен полностью!" << tokenFilePath;
        return false;
    }
    tokenFile.close();

#if defined(Q_OS_UNIX)
    QFile::setPermissions(tokenFilePath, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
#endif

    qInfo() << "JWT успешно сохранён:" << tokenFilePath;
    return true;
}
