#ifndef CHANGE_ADMINPASS_DIALOG_H
#define CHANGE_ADMINPASS_DIALOG_H

#include <QDialog>

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
        void setupCenteredLayout();
        void setupUI();
        void setupStyles();

    private:
        Ui::ChangeAdminPassDialog *ui;
};

#endif // CHANGE_ADMINPASS_DIALOG_H
