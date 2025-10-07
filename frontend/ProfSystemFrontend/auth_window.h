#ifndef AUTH_WINDOW_H
#define AUTH_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AuthWindow;
}
QT_END_NAMESPACE

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

private slots:
    void onLoginClicked();
    void onShowPasswordToggled(bool checked);

private:
    Ui::AuthWindow *ui;
    void initializeWindow();
    void setupStyles();
    void setupLayout();
    void setupConnections();
    void handleSuccessfulLogin();
    void showPasswordChangeDialog();
    void openAdminWindow();
    void handleFailedLogin();
};
#endif //AUTH_WINDOW_H
