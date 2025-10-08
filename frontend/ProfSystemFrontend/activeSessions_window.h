#ifndef ACTIVESESSIONS_WINDOW_H
#define ACTIVESESSIONS_WINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class ActiveSessionsWindow;
}

class ActiveSessionsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActiveSessionsWindow(QWidget *parent = nullptr);
    ~ActiveSessionsWindow();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onRefreshClicked();
    void onKillSessionClicked();

private:
    Ui::ActiveSessionsWindow *ui;
    void loadSessions();
    void setupUI();
};

#endif // ACTIVESESSIONS_WINDOW_H
