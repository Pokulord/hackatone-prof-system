#ifndef ADDUSER_DIALOG_H
#define ADDUSER_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class AddUserDialog;
}
QT_END_NAMESPACE

class AddUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUserDialog(QWidget *parent = nullptr);
    ~AddUserDialog();
    QString username() const;
    QString password() const;
    QString role() const;

private slots:
    void on_showPasswordCheckbox_toggled(bool checked);
    void on_createButton_clicked();
    void on_cancelButton_clicked();
    bool validateForm();

private:
    Ui::AddUserDialog *ui;
    bool validatePassword(const QString &password);
};

#endif // ADDUSER_DIALOG_H
