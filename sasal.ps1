param([string]$Root = "src")

$dirs = @(
  $Root,
  "$Root\domain\entities",
  "$Root\domain\repositories",
  "$Root\domain\services",
  "$Root\domain\exceptions",
  "$Root\application\use_cases",
  "$Root\application\ports",
  "$Root\application\dto",
  "$Root\infrastructure\adapters",
  "$Root\infrastructure\web",
  "$Root\infrastructure\data",
  "$Root\infrastructure\config",
  "$Root\infrastructure\logging",
  "$Root\infrastructure\migration",
  "$Root\presentation\dto",
  "$Root\tests",
  "$Root\config",
  "$Root\scripts",
  "$Root\sql"
)

# create directories
foreach ($d in $dirs) {
  if (-not (Test-Path $d)) { New-Item -ItemType Directory -Path $d | Out-Null }
}

# list of files to create (empty)
$files = @(
  "$Root\Dockerfile",
  "$Root\docker-compose.yml",

  "$Root\domain\entities\User.hpp",
  "$Root\domain\entities\Session.hpp",
  "$Root\domain\entities\AuditLog.hpp",
  "$Root\domain\repositories\IUserRepository.hpp",
  "$Root\domain\repositories\IAuditRepository.hpp",
  "$Root\domain\services\IPasswordHasher.hpp",
  "$Root\domain\services\ITokenGenerator.hpp",
  "$Root\domain\exceptions\AuthenticationException.hpp",

  "$Root\application\use_cases\AuthenticateUserUseCase.hpp",
  "$Root\application\use_cases\UseCase2.hpp",
  "$Root\application\use_cases\UseCase3.hpp",
  "$Root\application\use_cases\UseCase4.hpp",
  "$Root\application\use_cases\UseCase5.hpp",
  "$Root\application\use_cases\UseCase6.hpp",
  "$Root\application\use_cases\UseCase7.hpp",
  "$Root\application\ports\IAuthenticationService.hpp",
  "$Root\application\dto\AuthenticationRequest.hpp",

  "$Root\infrastructure\adapters\README.md",
  "$Root\infrastructure\web\README.md",
  "$Root\infrastructure\data\README.md",
  "$Root\infrastructure\config\README.md",
  "$Root\infrastructure\logging\README.md",
  "$Root\infrastructure\migration\README.md",

  "$Root\presentation\dto\README.md",
  "$Root\tests\README.md",

  "$Root\config\auth_server.json",
  "$Root\config\database.json",

  "$Root\scripts\build.sh",
  "$Root\scripts\run.sh",

  "$Root\sql\schema.sql",
  "$Root\sql\migrations.sql"
)

foreach ($f in $files) {
  if (-not (Test-Path $f)) {
    New-Item -ItemType File -Path $f | Out-Null
  }
}

Write-Output "Создана структура: .\$Root"
