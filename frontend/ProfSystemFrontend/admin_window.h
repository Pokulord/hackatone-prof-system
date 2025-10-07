#ifndef ADMIN_WINDOW_H
#define ADMIN_WINDOW_H

#include <QGridLayout>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class AdminWindow;
}
QT_END_NAMESPACE

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    void onStatisticsClicked();
    void onUsersClicked();
    void onSessionsClicked();
    void onLogsClicked();
    void centerContent();

private:
    Ui::AdminWindow *ui;
};

#endif // ADMIN_WINDOW_H
