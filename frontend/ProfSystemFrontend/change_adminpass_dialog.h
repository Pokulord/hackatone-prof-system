#ifndef CHANGE_ADMINPASS_DIALOG_H
#define CHANGE_ADMINPASS_DIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChangeAdminPassDialog;
}
QT_END_NAMESPACE

class ChangeAdminPassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeAdminPassDialog(QWidget *parent = nullptr);
    ~ChangeAdminPassDialog();

private slots:
    void onShowPasswordToggled(bool checked);
    void onChangePasswordClicked();
    void onPasswordTextChanged();
    void onLoginFinished();
    void onPasswordChangedInDB();
    void onMustChangePasswordUpdated();

private:
    void setupCenteredLayout();
    void setupUI();
    void setupStyles();
    void login(const QString& username, const QString& password);
    bool saveJwtTokenToFile(const QString& token);
    void changeAdminPassword(const QString& newPassword); // отправка нового пароля в БД
    void updateMustChangePasswordFlag(const QString& username, bool mustChange); // универсальная функция для mustChangePassword
    QString loadJwtToken(); // для подстановки токена из файла для авторизации

    Ui::ChangeAdminPassDialog *ui;
    QNetworkAccessManager* networkManager;
};

#endif // CHANGE_ADMINPASS_DIALOG_H
