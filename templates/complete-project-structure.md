# 🚀 COMPLETE PROJECT STRUCTURE - CLEAN ARCHITECTURE VERSION

## 📋 UPDATED TEAM DISTRIBUTION

### 🔹 БЭКЕНДЕР #1 - CORE AUTHENTICATION & USE CASES
**Ответственность:** Clean Architecture core - entities, use cases, authentication logic

**Ваши папки:**
- `domain/entities/` - бизнес-сущности 
- `domain/repositories/` - интерфейсы репозиториев
- `domain/services/` - интерфейсы доменных сервисов
- `application/use_cases/` - бизнес-логика приложения
- `application/ports/` - порты для внешних адаптеров
- `infrastructure/adapters/` - конкретные реализации сервисов
- `infrastructure/web/` - HTTP API контроллеры

### 🔸 БЭКЕНДЕР #2 - DATA LAYER & INFRASTRUCTURE  
**Ответственность:** PostgreSQL, конфигурации, деплой, инфраструктура

**Ваши папки:**
- `infrastructure/data/` - работа с PostgreSQL
- `infrastructure/config/` - конфигурация системы
- `infrastructure/logging/` - система логирования
- `infrastructure/migration/` - миграции базы данных
- `presentation/dto/` - DTO для HTTP API
- `tests/` - тестовая инфраструктура
- `docker/` - контейнеризация
- `scripts/` - скрипты сборки и деплоя

---

## 📂 ПОЛНАЯ СТРУКТУРА ПРОЕКТА

```
AuthServer/
├── main.cpp                                    // DI setup (Бэкендер #1)
├── CMakeLists.txt                             // Build config (Бэкендер #2)
├── Dockerfile                                 // Container (Бэкендер #2)
├── docker-compose.yml                         // Orchestration (Бэкендер #2)
├── README.md                                  // Documentation
├── .gitignore                                 // Git config
├── 
├── domain/                                    // 🔹 БЭКЕНДЕР #1
│   ├── entities/
│   │   ├── User.hpp                          // Core user entity
│   │   ├── Session.hpp                       // User session entity  
│   │   ├── AuditLog.hpp                      // Audit log entity
│   │   └── UserRole.hpp                      // User role enum
│   ├── repositories/
│   │   ├── IUserRepository.hpp               // User repository interface
│   │   └── IAuditRepository.hpp              // Audit repository interface
│   ├── services/
│   │   ├── IPasswordHasher.hpp               // Password hashing interface
│   │   ├── ITokenGenerator.hpp               // Token generation interface
│   │   └── IDateTimeProvider.hpp             // DateTime service interface
│   └── exceptions/
│       ├── AuthenticationException.hpp        // Auth exceptions
│       ├── UserNotFoundException.hpp          // User not found
│       └── ValidationException.hpp           // Validation errors
├── 
├── application/                               // 🔹 БЭКЕНДЕР #1
│   ├── use_cases/
│   │   ├── AuthenticateUserUseCase.hpp/.cpp  // Login use case
│   │   ├── CreateUserUseCase.hpp/.cpp        // Create user use case
│   │   ├── DeleteUserUseCase.hpp/.cpp        // Delete user use case
│   │   ├── ChangePasswordUseCase.hpp/.cpp    // Change password use case
│   │   ├── ForcePasswordChangeUseCase.hpp/.cpp // Force password change
│   │   ├── GetUsersUseCase.hpp/.cpp          // Get users list use case
│   │   └── GetAuditLogsUseCase.hpp/.cpp      // Get audit logs use case
│   ├── ports/
│   │   ├── IAuthenticationService.hpp        // Auth service port
│   │   └── IUserManagementService.hpp        // User management port
│   └── dto/
│       ├── AuthenticationRequest.hpp         // Auth request DTO
│       ├── AuthenticationResult.hpp          // Auth result DTO
│       ├── CreateUserRequest.hpp             // Create user request DTO
│       └── ChangePasswordRequest.hpp         // Change password request DTO
├── 
├── infrastructure/
│   ├── adapters/                             // 🔹 БЭКЕНДЕР #1
│   │   ├── BcryptPasswordHasher.hpp/.cpp     // Bcrypt implementation
│   │   ├── JwtTokenGenerator.hpp/.cpp        // JWT token generation
│   │   └── SystemDateTimeProvider.hpp/.cpp  // System datetime provider
│   ├── web/                                  // 🔹 БЭКЕНДЕР #1
│   │   ├── RestApiServer.hpp/.cpp            // HTTP server
│   │   ├── AuthController.hpp/.cpp           // Auth endpoints
│   │   ├── UserController.hpp/.cpp           // User management endpoints
│   │   ├── AuditController.hpp/.cpp          // Audit endpoints
│   │   └── HttpErrorHandler.hpp/.cpp         // Error handling
│   ├── data/                                 // 🔸 БЭКЕНДЕР #2
│   │   ├── DatabaseManager.hpp/.cpp          // PostgreSQL connection
│   │   ├── PostgreSqlUserRepository.hpp/.cpp // User repository impl
│   │   ├── PostgreSqlAuditRepository.hpp/.cpp// Audit repository impl
│   │   └── ConnectionPool.hpp/.cpp           // DB connection pool
│   ├── config/                               // 🔸 БЭКЕНДЕР #2
│   │   ├── ConfigManager.hpp/.cpp            // Config management
│   │   ├── DatabaseConfig.hpp                // DB configuration
│   │   ├── ServerConfig.hpp                  // Server configuration
│   │   └── SecurityConfig.hpp                // Security settings
│   ├── logging/                              // 🔸 БЭКЕНДЕР #2
│   │   ├── Logger.hpp/.cpp                   // Logger interface
│   │   ├── FileLogger.hpp/.cpp               // File logging impl
│   │   └── ConsoleLogger.hpp/.cpp            // Console logging impl
│   └── migration/                            // 🔸 БЭКЕНДЕР #2
│       ├── MigrationRunner.hpp/.cpp          // Migration runner
│       ├── Migration.hpp                     // Base migration class
│       ├── 001_CreateUsersTable.hpp/.cpp     // Users table migration
│       ├── 002_CreateAuditTable.hpp/.cpp     // Audit table migration
│       └── 003_AddIndexes.hpp/.cpp           // Indexes migration
├── 
├── presentation/                              // 🔸 БЭКЕНДЕР #2
│   └── dto/
│       ├── AuthRequestDto.hpp                // HTTP auth request DTO
│       ├── AuthResponseDto.hpp               // HTTP auth response DTO
│       ├── UserDto.hpp                       // User DTO for HTTP
│       ├── CreateUserDto.hpp                 // Create user HTTP DTO
│       ├── ChangePasswordDto.hpp             // Change password HTTP DTO
│       ├── ErrorDto.hpp                      // Error response DTO
│       └── AuditLogDto.hpp                   // Audit log HTTP DTO
├── 
├── tests/                                     // 🔸 БЭКЕНДЕР #2
│   ├── unit/                                 // Unit tests
│   ├── integration/                          // Integration tests
│   ├── e2e/                                  // End-to-end tests
│   ├── mocks/                                // Mock objects
│   └── fixtures/                             // Test fixtures
├── 
├── config/                                    // 🔸 БЭКЕНДЕР #2
│   ├── auth_server.json                      // Main config file
│   ├── database.json                         // Database config
│   └── logging.json                          // Logging config
├── 
├── scripts/                                   // 🔸 БЭКЕНДЕР #2
│   ├── build.sh                              // Build script
│   ├── run.sh                                // Run script
│   ├── test.sh                               // Test script
│   └── setup_db.sh                           // Database setup
├── 
├── docker/                                    // 🔸 БЭКЕНДЕР #2
│   ├── Dockerfile.backend                    // Backend container
│   ├── Dockerfile.postgres                   // PostgreSQL container
│   └── init.sql                              // DB initialization
└── 
└── sql/                                       // 🔸 БЭКЕНДЕР #2
    ├── 001_initial_schema.sql                // Initial schema
    ├── 002_add_failed_attempts.sql           // Failed attempts feature
    ├── 003_add_indexes.sql                   // Performance indexes
    └── seed_admin.sql                        // Default admin user
```

---

## 🎯 КОНКРЕТНЫЕ ЗАДАЧИ ПО ФАЙЛАМ

### 🔹 БЭКЕНДЕР #1 - Приоритетные файлы (в порядке важности):

1. **domain/entities/User.hpp** - Core entity with all user fields
2. **domain/repositories/IUserRepository.hpp** - Repository interface 
3. **domain/services/IPasswordHasher.hpp** - Password hashing interface
4. **application/use_cases/AuthenticateUserUseCase.hpp/.cpp** - Login logic
5. **application/use_cases/CreateUserUseCase.hpp/.cpp** - User creation
6. **infrastructure/adapters/BcryptPasswordHasher.hpp/.cpp** - Bcrypt impl
7. **infrastructure/web/RestApiServer.hpp/.cpp** - HTTP server
8. **infrastructure/web/AuthController.hpp/.cpp** - Auth endpoints
9. **main.cpp** - Dependency injection setup

### 🔸 БЭКЕНДЕР #2 - Приоритетные файлы (в порядке важности):

1. **infrastructure/data/DatabaseManager.hpp/.cpp** - PostgreSQL connection
2. **infrastructure/data/PostgreSqlUserRepository.hpp/.cpp** - User repo impl
3. **infrastructure/config/ConfigManager.hpp/.cpp** - Configuration system
4. **infrastructure/migration/MigrationRunner.hpp/.cpp** - DB migrations
5. **sql/001_initial_schema.sql** - Database schema
6. **CMakeLists.txt** - Build configuration
7. **Dockerfile** - Container configuration
8. **docker-compose.yml** - Full stack orchestration
9. **presentation/dto/** - HTTP DTO classes

---

## ⚡ CRITICAL PATH - Порядок разработки:

### День 1 (сегодня, осталось ~2 часа):
**Бэкендер #1:**
- Создать базовые entities и interfaces
- Начать первый use case (AuthenticateUser)

**Бэкендер #2:**
- Настроить PostgreSQL подключение
- Создать базовую схему БД
- Настроить CMakeLists.txt

### День 2 (завтра):
**Бэкендер #1:**
- Доделать все use cases
- Реализовать REST API endpoints
- Интеграция с data layer

**Бэкендер #2:**
- Реализовать все repositories
- Система конфигурации и логирования
- Docker контейнеризация

### День 3 (финал):
**Общая интеграция, тестирование, полировка**

---

## 📊 Статистика проекта:

- **Всего файлов:** ~80
- **Бэкендер #1:** ~35 файлов (core logic)
- **Бэкендер #2:** ~35 файлов (infrastructure)
- **Общие:** ~10 файлов (документация, конфиги)

**Это серьезный enterprise-level проект! Clean Architecture + PostgreSQL + Docker = максимум баллов за техническое качество! 🚀**