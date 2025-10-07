#ifndef USERS_WINDOW_H
#define USERS_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class UsersInfoWindow;
}
QT_END_NAMESPACE

class UsersInfoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UsersInfoWindow(QWidget *parent = nullptr);
    ~UsersInfoWindow();

private slots:
    void onAddUserClicked();
    void onDeleteUserClicked();
    void onEditUserClicked();
    void onRefreshClicked();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupToolbar();
    void setupTable();
    void loadUsers();

    Ui::UsersInfoWindow *ui;
};

#endif // USERS_WINDOW_H
