#ifndef STATISTICS_WINDOW_H
#define STATISTICS_WINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class StatisticsWindow;
}
QT_END_NAMESPACE

class StatisticsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatisticsWindow(QWidget *parent = nullptr);
    ~StatisticsWindow();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onRefreshClicked();

private:
    Ui::StatisticsWindow *ui;
    void loadStatistics();
    void setupUI();
};

#endif // STATISTICS_WINDOW_H
