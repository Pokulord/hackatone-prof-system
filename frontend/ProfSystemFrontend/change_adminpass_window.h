#ifndef CHANGE_ADMINPASS_WINDOW_H
#define CHANGE_ADMINPASS_WINDOW_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChangeAdminPassWindow;
}
QT_END_NAMESPACE

class ChangeAdminPassWindow : public QDialog
{
    Q_OBJECT

    public:
        explicit ChangeAdminPassWindow(QWidget *parent = nullptr);
        ~ChangeAdminPassWindow();

    private slots:
        void onShowPasswordToggled(bool checked);
        void onChangePasswordClicked();
        void onPasswordTextChanged();
        void setupCenteredLayout();
        void setupUI();
        void setupStyles();

    private:
        Ui::ChangeAdminPassWindow *ui;
};

#endif // CHANGE_ADMINPASS_WINDOW_H
