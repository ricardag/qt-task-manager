# Task Manager para Linux (clone do Windows 11 Task Manager) — Qt Widgets / C++

## Contexto

O usuário quer criar, do zero, um clone visual e funcional do Task Manager do Windows 11 para Linux, em C++ (GNU toolchain), com interface gráfica, rodando nativamente em x64 e arm64. O projeto será open-source. O diretório de trabalho (`/Volumes/Workspaces/data/desenv/Linux/TaskManager`) está vazio — não há código nem estrutura prévia para reaproveitar.

Decisões já tomadas em conversa com o usuário:
- **Toolkit**: Qt Widgets (não QML/GTK) — melhor fidelidade visual ao Win11 (QTableView/QTreeView com models, QSplitter, gráficos via Qt Charts, QSS para estilização), portabilidade x64/arm64 madura, e licença LGPLv3 compatível com projeto open-source (linkagem dinâmica, sem custo).
- **Ambiente de dev**: o usuário desenvolve em macOS (Apple Silicon), mas o app só roda em Linux (lê `/proc`/`/sys` diretamente). Fluxo: editar no macOS → build/run dentro de VM Linux via **VMware Fusion** (grátis para uso pessoal, virtualização nativa arm64 em Apple Silicon).
- **VM de desenvolvimento**: **Fedora KDE Plasma Desktop (Spin)**, versão atual estável. Escolhido em vez de Debian por preferência do usuário e por ser a distro "casa" do KDE (integração/polimento superiores, pacotes Qt6 mais recentes via `dnf`). A cobertura de compatibilidade mais conservadora (que motivou considerar Debian antes) fica garantida pelo CI, que roda sobre Ubuntu.
- **x64 real**: como Apple Silicon não virtualiza x64 nativamente (só emulação lenta via QEMU/TCG), a validação x64 fica a cargo do **CI**, não da VM local.
- **CI**: GitHub Actions com matriz de arquiteturas — `ubuntu-latest` (x64 nativo) e `ubuntu-24.04-arm` (arm64 nativo, sem emulação).

## Estrutura do projeto

```
TaskManager/
├── CMakeLists.txt
├── cmake/
│   ├── CompilerWarnings.cmake
│   └── Qt6Setup.cmake
├── src/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── app/
│   │   ├── MainWindow.h/.cpp          # QMainWindow raiz
│   │   ├── NavigationSidebar.h/.cpp   # QListWidget estilo NavigationView do Win11
│   │   └── RefreshController.h/.cpp
│   ├── views/                          # uma "página" por aba do Task Manager
│   │   ├── ProcessesView.h/.cpp
│   │   ├── PerformanceView.h/.cpp      # + CpuGraphWidget, MemoryGraphWidget, DiskGraphWidget, NetworkGraphWidget, GpuGraphWidget
│   │   ├── AppHistoryView.h/.cpp
│   │   ├── StartupView.h/.cpp
│   │   ├── UsersView.h/.cpp
│   │   ├── DetailsView.h/.cpp
│   │   └── ServicesView.h/.cpp
│   ├── models/                         # QAbstractTableModel por view tabular
│   │   ├── ProcessTableModel.h/.cpp
│   │   ├── ServiceTableModel.h/.cpp
│   │   ├── StartupTableModel.h/.cpp
│   │   └── UserTableModel.h/.cpp
│   ├── datasource/                     # STL puro, SEM dependência de QWidget — testável isoladamente
│   │   ├── ProcessInfo.h/.cpp
│   │   ├── ProcessReader.h/.cpp        # /proc/[pid]/{stat,status,cmdline,io}
│   │   ├── CpuStats.h/.cpp             # /proc/stat, /proc/cpuinfo
│   │   ├── MemoryStats.h/.cpp          # /proc/meminfo
│   │   ├── DiskStats.h/.cpp            # /proc/diskstats, statvfs
│   │   ├── NetworkStats.h/.cpp         # /proc/net/dev
│   │   ├── GpuStats.h/.cpp             # sysfs (amdgpu/i915); NVML via dlopen opcional
│   │   ├── UserSessions.h/.cpp         # utmpx, getpwuid
│   │   ├── SystemdServices.h/.cpp      # D-Bus org.freedesktop.systemd1
│   │   └── ProcFsPaths.h
│   ├── SystemMonitor.h/.cpp            # fachada QObject: orquestra datasource + QTimer, emite sinais Qt
│   └── util/
│       ├── Formatting.h/.cpp
│       └── ProcFileUtils.h/.cpp        # readFile() -> std::optional<std::string>, raiz injetável
├── resources/
│   ├── icons/ (SVG estilo Fluent)
│   ├── resources.qrc
│   └── style.qss                       # QSS aproximando visual Win11
├── tests/
│   ├── CMakeLists.txt
│   ├── fixtures/proc/                  # snapshots reais e anonimizados de /proc para testes determinísticos
│   └── test_*.cpp
├── .github/workflows/ci.yml
├── .gitignore
├── .clang-format
├── LICENSE (LGPLv3, ou GPLv3 para o app + LGPL só para o linking do Qt — a definir)
└── README.md
```

**Padrão de linguagem**: C++20, `CMAKE_CXX_EXTENSIONS OFF` (i.e. `-std=c++20`, não `-std=gnu++20`). O projeto usa apenas headers POSIX/Linux padrão (`unistd.h`, `sys/statvfs.h`, `pwd.h`, `utmpx.h`) — não há necessidade de sintaxe GNU-específica (trigraphs, `typeof`, statement expressions). C++20 dá acesso a `std::span`/`std::ranges` úteis na camada `datasource`.

## Arquitetura da aplicação

`MainWindow (QMainWindow)` contém:
- `NavigationSidebar` (QListWidget com ícones, estilo NavigationView do Win11: hover/seleção via QSS, indicador lateral)
- `QStackedWidget` central — uma página por item da sidebar
- `SystemMonitor` (QObject, não widget) — único ponto que possui o `QTimer` central (padrão 1000ms), delega a cada tick para os `*Stats`/`*Reader` de `datasource/`, e emite sinais (`processesUpdated`, `cpuSampleReady`, `memorySampleReady`, `diskSampleReady`, `networkSampleReady`, `gpuSampleReady`)

Regras de desacoplamento:
- **Views nunca leem `/proc` diretamente** — só se conectam aos sinais do `SystemMonitor` e atualizam seus `QAbstractTableModel`s ou widgets de gráfico (`QChartView`, Qt Charts).
- `ProcessTableModel` faz diff incremental por PID a cada tick (não recria a lista inteira) para evitar flicker de seleção/scroll.
- `ServicesView` usa D-Bus (`QDBusInterface` sobre `org.freedesktop.systemd1`) para listar serviços; fallback via `QProcess` + `systemctl` se D-Bus não estiver acessível.
- `StartupView` mapeia para entradas de autostart (`~/.config/autostart/*.desktop`, `/etc/xdg/autostart/*.desktop`) com toggle enable/disable.
- `AppHistoryView`: Linux não tem equivalente nativo ao histórico de apps do Windows — documentar no README como decisão de produto (reduzir a métricas cumulativas desde o boot, ou placeholder inicial), não tratar como funcionalidade faltando.
- Suporte a GPU (NVIDIA/NVML) deve ser **opcional via `dlopen` em runtime**, nunca linkado estaticamente nem condicionado por arquitetura no CMake — todo o resto do código de `datasource/` é agnóstico de arquitetura (caminhos e formatos de `/proc` são idênticos em x64/arm64).

## CMake

Raiz (`CMakeLists.txt`): `cmake_minimum_required(3.21)`, C++20 sem extensões GNU, `CMAKE_AUTOMOC/AUTORCC/AUTOUIC ON`, `find_package(Qt6 REQUIRED COMPONENTS Widgets Charts DBus)`, `qt_standard_project_setup()`, `add_subdirectory(src)`, testes via `option(TASKMANAGER_BUILD_TESTS ON)` + `add_subdirectory(tests)`.

`src/`: executável via `qt_add_executable`, linkando `Qt6::Widgets Qt6::Charts Qt6::DBus`. A camada `datasource/` deve ser extraída como **biblioteca estática separada** (`taskmanager_datasource`, linkando só `Qt6::Core` ou nada de Qt se ficar 100% STL) para ser testável sem recompilar toda a UI, e linkada tanto pelo executável quanto pelos testes.

Sem nenhum `if(CMAKE_SYSTEM_PROCESSOR ...)` — nada arch-specific hardcoded.

## CI — `.github/workflows/ci.yml`

Matriz `{ubuntu-latest (x64), ubuntu-24.04-arm (arm64)}`, cada job: `apt-get install build-essential cmake ninja-build qt6-base-dev qt6-base-dev-tools libqt6charts6-dev libqt6svg6-dev qt6-tools-dev qt6-tools-dev-tools libgl1-mesa-dev`, depois `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`, `cmake --build build --parallel`, `ctest --test-dir build --output-on-failure`. Documentar no README que o nome do pacote Qt Charts pode variar entre versões do Ubuntu (`libqt6charts6-dev` vs `qt6-charts-dev`).

## Setup local de dev (VM Fedora KDE Plasma Spin, VMware Fusion)

1. Baixar a imagem **Fedora KDE Plasma Desktop Spin** (já vem com KDE pré-instalado, sem etapa extra de seleção de desktop) e criar a VM no Fusion — disco ~50 GB thin-provisioned, "Accelerate 3D Graphics" habilitado.
2. `sudo dnf install open-vm-tools open-vm-tools-desktop`, reiniciar — habilita shared folders, clipboard, resolução dinâmica.
3. Configurar Shared Folder no VMware Fusion apontando para `/Volumes/Workspaces/data/desenv/Linux/TaskManager` no macOS.
4. Instalar toolchain: `sudo dnf install gcc-c++ cmake ninja-build git qt6-qtbase-devel qt6-qtcharts-devel qt6-qtsvg-devel qt6-qttools-devel qt6-qtbase-private-devel` (confirmar nomes exatos de pacote na versão do Fedora em uso — `dnf search qt6` ajuda a localizar variações).
5. Fluxo recomendado: editar no macOS, versionar via `git` (commit no host, `git pull` na VM) como ponte principal — mais robusto que depender só do shared folder HGFS para I/O de build. Shared folder fica reservado para troca avulsa de arquivos.
6. Build/run na VM: `cmake -S . -B build -G Ninja && cmake --build build && ./build/src/taskmanager`.
7. Rodar sob KDE Plasma nativo garante fontes/DPI/tema comparáveis ao ambiente-alvo real (QSS Fluent-like renderiza de forma mais previsível).

## Estratégia de testes

**Framework: Qt Test (`Qt6::Test`)** — já vem com Qt (sem dependência externa extra no CI), integra com o mesmo CMake/AUTOMOC do resto do projeto.

- Cada classe de `datasource/` (`CpuStats`, `MemoryStats`, `ProcessReader`, etc.) recebe no construtor uma raiz `std::filesystem::path` injetável (default `/proc`), permitindo testes apontarem para `tests/fixtures/proc/` — snapshots reais e anonimizados commitados no repo.
- `ProcFileUtils::readFile()` retorna `std::optional<std::string>`, nunca lança por arquivo ausente (processo pode morrer entre listagem e leitura).
- Casos de borda a cobrir em `ProcessReader`: `cmdline` vazio (kernel threads, aparecem como `[nome]`), nomes com parênteses/espaços em `comm` no `/proc/[pid]/stat`, PID que desaparece entre `readdir` e leitura (retorna `nullopt`, não crasha).
- Testes de UI/models ficam para uma fase 2 (via `QT_QPA_PLATFORM=offscreen` no CI) — não bloqueante para o MVP.

## Arquivos críticos para começar a implementação

- [CMakeLists.txt](CMakeLists.txt) (raiz)
- [src/SystemMonitor.h](src/SystemMonitor.h)
- [src/datasource/ProcessReader.h](src/datasource/ProcessReader.h)
- [src/app/MainWindow.h](src/app/MainWindow.h)
- [.github/workflows/ci.yml](.github/workflows/ci.yml)
- [tests/CMakeLists.txt](tests/CMakeLists.txt)

## Verificação

1. **Build local (VM Fedora KDE)**: `cmake -S . -B build -G Ninja && cmake --build build` deve compilar sem warnings novos; `./build/src/taskmanager` deve abrir a janela com a sidebar e pelo menos a aba Processos funcional mostrando processos reais da VM.
2. **Testes unitários**: `ctest --test-dir build --output-on-failure` — todos os testes de `datasource/` passando usando as fixtures de `/proc`, sem depender da máquina que roda o teste.
3. **CI**: push para o repo aciona `ci.yml`; os dois jobs da matriz (x64 e arm64) devem buildar e passar `ctest` com sucesso, confirmando portabilidade cross-arch sem emulação.
4. **Verificação visual manual**: comparar lado a lado com screenshots do Task Manager do Windows 11 (sidebar, tabela de processos, gráficos de desempenho) para validar fidelidade do QSS/layout.
