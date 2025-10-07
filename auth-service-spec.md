# Auth Service - Техническая спецификация

## Архитектура

**Модульный монолит** с Clean Architecture:
```
Клиент -> API Endpoints -> Use Cases -> Repository -> PostgreSQL
```

### Структура проекта
```
src/
├── main.cpp                    # Точка входа
├── app.cpp                     # Сборка приложения
├── core/                       # Бизнес-логика (Clean Architecture)
│   ├── entities/              # Сущности (User, Role, Permission, Session)
│   ├── usecases/              # Варианты использования
│   └── interfaces/            # Интерфейсы репозиториев
├── infrastructure/            # Реализация репозиториев и внешние сервисы
│   ├── database/              # PostgreSQL (libpq)
│   └── security/              # JWT, хэширование паролей
├── api/                       # HTTP API (Crow framework)
│   ├── controllers/           # Контроллеры эндпоинтов
│   ├── middleware/            # Авторизация, валидация
│   └── dto/                   # Data Transfer Objects
└── shared/                    # Логирование, утилиты, ошибки
```

## API Эндпоинты

### 1. Авторизация `/api/auth`

| Эндпоинт | Метод | Назначение |
|----------|-------|------------|
| `/register` | POST | Регистрация пользователя (создание + хэширование пароля) |
| `/login` | POST | Вход в систему (проверка логина/пароля, выдача токенов) |
| `/logout` | POST | Выход из системы (аннулирование токенов) |
| `/refresh` | POST | Обновление access токена по refresh токену |
| `/validate` | GET | Проверка действительности access токена |
| `/revoke` | POST | Принудительное завершение сессии (админ функция) |

### 2. Управление пользователями `/api/users`

| Эндпоинт | Метод | Назначение | Доступ |
|----------|-------|------------|--------|
| `/` | GET | Список всех пользователей | Admin only |
| `/{id}` | GET | Информация о конкретном пользователе | Admin/Owner |
| `/me` | GET | Профиль текущего пользователя | Authenticated |
| `/{id}` | PUT | Обновление профиля пользователя | Admin/Owner |
| `/{id}` | DELETE | Удаление пользователя | Admin/Owner |
| `/{id}/roles` | PATCH | Изменение ролей пользователя | Admin only |

### 3. Права и роли `/api/roles`

| Эндпоинт | Метод | Назначение | Доступ |
|----------|-------|------------|--------|
| `/` | GET | Получить список всех ролей | Admin only |
| `/` | POST | Создать новую роль | Admin only |
| `/{id}` | PUT | Обновить роль | Admin only |
| `/{id}` | DELETE | Удалить роль | Admin only |
| `/{id}/permissions` | PATCH | Управление правами роли | Admin only |

### 4. Мониторинг системы `/api`

| Эндпоинт | Метод | Назначение | Доступ |
|----------|-------|------------|--------|
| `/health` | GET | Проверка состояния API и БД | Public |
| `/version` | GET | Версия API | Public |

## Технологии

- **HTTP Framework**: Crow
- **Database**: PostgreSQL + libpq
- **Authentication**: JWT токены
- **Logging**: spdlog
- **Config**: .env файлы
- **Build**: CMake
- **C++ Standard**: C++20

## Use Cases (бизнес-логика)

### AuthUseCase
- RegisterUser
- LoginUser  
- RefreshToken
- ValidateToken
- RevokeToken

### UserUseCase
- GetUserProfile
- UpdateUserProfile
- DeleteUser
- GetUserList (admin)
- ManageUserRoles (admin)

### RoleUseCase  
- CreateRole (admin)
- UpdateRole (admin)
- DeleteRole (admin)
- AssignPermissions (admin)

## Основные сущности

### User
```cpp
struct User {
    int id;
    std::string email;
    std::string password_hash;
    std::string first_name;
    std::string last_name;
    bool is_active;
    std::vector<Role> roles;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
};
```

### Role & Permission
```cpp
struct Role {
    int id;
    std::string name;
    std::string description;
    std::vector<Permission> permissions;
};

struct Permission {
    int id;
    std::string name;
    std::string resource;
    std::string action;
};
```

### Session (JWT)
```cpp
struct Session {
    std::string access_token;
    std::string refresh_token;
    int user_id;
    std::chrono::system_clock::time_point expires_at;
    std::string ip_address;
    std::string user_agent;
};
```

## Подготовка к микросервисной архитектуре

Все интерфейсы (IUserRepository, IAuthService и т.д.) проектируются так, чтобы модуль можно было легко вынести в отдельный сервис с минимальными изменениями кода.

**Принцип**: Чистые слои + чёткие контракты = простая миграция в микросервисы