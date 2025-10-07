#include "usersInfo_window.h"
#include "ui_usersInfo_window.h"
#include "adduser_dialog.h"
#include "edituser_dialog.h"
#include <QToolBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>

UsersInfoWindow::UsersInfoWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UsersInfoWindow)
{
    ui->setupUi(this);

    setWindowTitle("Управление пользователями");
    setWindowIcon(QIcon("B:/hackatone-prof-system/frontend/ProfSystemFrontend/resources/logo.jpg"));
    setMinimumSize(1000, 600);

    setupToolbar();
    setupTable();
    loadUsers();

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(ui->usersTable);
    layout->setContentsMargins(10, 10, 10, 10); // Небольшие отступы
    setCentralWidget(centralWidget);
}

void UsersInfoWindow::setupToolbar()
{
    QToolBar *toolbar = addToolBar("Основные действия");

    QAction *addAction = toolbar->addAction("➕ Добавить");
    QAction *editAction = toolbar->addAction("✏️ Редактировать");
    QAction *deleteAction = toolbar->addAction("🗑️ Удалить");
    QAction *refreshAction = toolbar->addAction("🔄 Обновить");

    connect(addAction, &QAction::triggered, this, &UsersInfoWindow::onAddUserClicked);
    connect(editAction, &QAction::triggered, this, &UsersInfoWindow::onEditUserClicked);
    connect(deleteAction, &QAction::triggered, this, &UsersInfoWindow::onDeleteUserClicked);
    connect(refreshAction, &QAction::triggered, this, &UsersInfoWindow::onRefreshClicked);
}

void UsersInfoWindow::setupTable()
{
    QTableWidget *table = ui->usersTable;

    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Логин", "Роль", "Дата создания", "Статус"});

    table->setStyleSheet(
        "QTableWidget {"
        "   background: white;"
        "   border: 1px solid #bdc3c7;"
        "   border-radius: 5px;"
        "}"
        "QHeaderView::section {"
        "   background: #3498db;"
        "   color: white;"
        "   padding: 8px;"
        "   border: none;"
        "}"
        );

    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UsersInfoWindow::loadUsers()
{
    // TODO: Заменить на загрузку из API
    QTableWidget *table = ui->usersTable;
    table->setRowCount(3);

    QStringList users = {
        "admin|Администратор|2024-01-15|Активен",
        "user1|Пользователь|2024-01-20|Активен",
        "operator|Оператор|2024-01-25|Заблокирован"
    };

    for (int i = 0; i < users.size(); ++i) {
        QStringList fields = users[i].split('|');
        for (int j = 0; j < fields.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(fields[j]);

            if (j == 3) {
                item->setForeground(fields[j] == "Активен" ? Qt::darkGreen : Qt::red);
            }

            table->setItem(i, j, item);
        }
    }
}

void UsersInfoWindow::onAddUserClicked()
{
    AddUserDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        // Получаем данные из диалога
        QString username = dialog.username();
        QString role = dialog.role();

        // TODO: Вызов API для создания пользователя
        statusBar()->showMessage("Пользователь " + username + " добавлен");
        loadUsers();

        qDebug() << "Создан пользователь:" << username << role;
    }
}

#include "edituser_dialog.h"

void UsersInfoWindow::onEditUserClicked()
{
    QTableWidget *table = ui->usersTable;
    int row = table->currentRow();

    if (row == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя для редактирования");
        return;
    }

    // Получаем данные из таблицы
    QString username = table->item(row, 0)->text();
    QString currentRole = table->item(row, 1)->text();
    QString currentStatus = table->item(row, 3)->text();

    EditUserDialog dialog(this);
    dialog.setUserData(username, UserRoles::getRoleFromName(currentRole), currentStatus);

    if (dialog.exec() == QDialog::Accepted) {
        UserRoles::Role newRole = dialog.role();
        QString newStatus = dialog.status();

        if (dialog.shouldResetPassword()) {
            // TODO: Вызов API для обновления пользователя
        }

        // Обновляем таблицу
        table->item(row, 1)->setText(UserRoles::getRoleName(newRole));
        // TODO: Обновить другие поля когда будут реальные данные

        statusBar()->showMessage("Пользователь " + username + " обновлен");

        qDebug() << "Обновлен пользователь:" << username
                 << "Роль:" << UserRoles::getRoleName(newRole)
                 << "Статус:" << newStatus
                 << "Сброс пароля:" << dialog.shouldResetPassword();
    }
}

void UsersInfoWindow::onDeleteUserClicked()
{
    QTableWidget *table = ui->usersTable;
    int row = table->currentRow();

    if (row == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя для удаления");
        return;
    }

    QString userToDelete = table->item(row, 0)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
        "Удалить пользователя " + userToDelete + "?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // TODO: Вызов API для удаления
        table->removeRow(row);
        statusBar()->showMessage("Пользователь " + userToDelete + " удален");
    }
}

void UsersInfoWindow::onRefreshClicked()
{
    loadUsers();
    statusBar()->showMessage("Список пользователей обновлен");
}

UsersInfoWindow::~UsersInfoWindow()
{
    delete ui;
}
