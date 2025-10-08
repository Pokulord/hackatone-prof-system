#ifndef AUDITLOG_WINDOW_H
#define AUDITLOG_WINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class AuditLogWindow;
}
QT_END_NAMESPACE

class AuditLogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuditLogWindow(QWidget *parent = nullptr);
    ~AuditLogWindow();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onRefreshClicked();
    void onFilterChanged();
    void onExportClicked();
    void onSearchClicked();

private:
    Ui::AuditLogWindow *ui;
    void loadAuditLog();
    void setupUI();
    void applyFilters();
};

#endif // AUDITLOG_WINDOW_H
