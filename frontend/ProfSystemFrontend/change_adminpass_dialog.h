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
    void onPasswordChangedInDB();

private:
    void setupCenteredLayout();
    void setupStyles();
    void changeUserPassword(const QString& username, const QString& newPassword);
    QString loadJwtToken(const QString& username);

    Ui::ChangeAdminPassDialog *ui;
    QNetworkAccessManager* networkManager;
};

#endif // CHANGE_ADMINPASS_DIALOG_H
