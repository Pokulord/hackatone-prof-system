#include "usersInfo_window.h"
#include "ui_usersInfo_window.h"
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
    // Временные тестовые данные
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
    bool ok;
    QString username = QInputDialog::getText(this, "Добавить пользователя", "Логин:", QLineEdit::Normal, "", &ok);
    if (ok && !username.isEmpty()) {
        // TODO: Вызов API для создания пользователя
        statusBar()->showMessage("Пользователь " + username + " добавлен");
        loadUsers(); // Обновляем таблицу
    }
}

void UsersInfoWindow::onEditUserClicked()
{
    QTableWidget *table = ui->usersTable;
    int row = table->currentRow();

    if (row == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя для редактирования");
        return;
    }

    QString currentUser = table->item(row, 0)->text();
    statusBar()->showMessage("Редактирование: " + currentUser);

    // TODO: Открыть диалог редактирования
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
