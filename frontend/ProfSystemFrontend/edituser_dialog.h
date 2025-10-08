#ifndef EDITUSER_DIALOG_H
#define EDITUSER_DIALOG_H

#include <QDialog>
#include "user_roles.h"

namespace Ui {
class EditUserDialog;
}

class EditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditUserDialog(QWidget *parent = nullptr);
    ~EditUserDialog();

    void setUserData(const QString &username, UserRoles::Role role, const QString &status);

    UserRoles::Role role() const;
    QString status() const;
    QString password() const;
    bool shouldResetPassword() const;

private slots:
    void on_showPasswordCheckbox_toggled(bool checked);
    void on_resetPasswordCheckbox_toggled(bool checked);
    void on_saveButton_clicked();
    void on_cancelButton_clicked();
    bool validateForm();

private:
    Ui::EditUserDialog *ui;
    bool validatePassword(const QString &password);
    void updatePasswordCheckboxStyle(bool checked);
};

#endif // EDITUSER_DIALOG_H
