#ifndef AUTH_WINDOW_H
#define AUTH_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginClicked();
    void onShowPasswordToggled(bool checked);

private:
    Ui::MainWindow *ui;
    void initializeWindow();
    void setupStyles();
    void setupLayout();
    void setupConnections();
    void handleSuccessfulLogin();
    void handleFailedLogin();
};
#endif
