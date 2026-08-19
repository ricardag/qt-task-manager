# TaskManager

Um clone visual e funcional do Gerenciador de Tarefas do Windows 11 para Linux, escrito em C++20 com Qt Widgets. Roda nativamente em x64 e arm64.

Veja [PLANO.md](PLANO.md) para o contexto completo de arquitetura e decisões de projeto.

## Abas

- **Processos** — lista de processos com uso de CPU/memória atualizado a cada segundo.
- **Desempenho** — gráficos em tempo real de CPU, Memória, Disco, Rede e GPU (quando disponível via sysfs).
- **Histórico de apps** — Linux não mantém esse histórico nativamente; a aba documenta essa diferença em vez de simular dados falsos.
- **Inicializar** — entradas de autostart (`~/.config/autostart`, `/etc/xdg/autostart`).
- **Usuários** — sessões logadas via utmpx.
- **Detalhes** — visão por processo (equivalente à aba "Detalhes" do Windows).
- **Serviços** — unidades systemd via D-Bus, com fallback para `systemctl`.

## Build

Dependências (Fedora):

```bash
sudo dnf install gcc-c++ cmake ninja-build git qt6-qtbase-devel qt6-qtcharts-devel qt6-qtsvg-devel qt6-qttools-devel qt6-qtbase-private-devel
```

Dependências (Ubuntu/Debian):

```bash
sudo apt install build-essential cmake ninja-build git qt6-base-dev qt6-base-dev-tools libqt6charts6-dev libqt6svg6-dev qt6-tools-dev qt6-tools-dev-tools
```

> O nome do pacote do Qt Charts pode variar entre versões de distro (`libqt6charts6-dev` vs `qt6-charts-dev`); use `apt-cache search` / `dnf search qt6` se o build falhar por dependência não encontrada.

Compilar:

```bash
cmake -S . -B build -G Ninja
cmake --build build --parallel
./build/src/taskmanager
```

## Testes

```bash
ctest --test-dir build --output-on-failure
```

Os testes da camada `datasource/` usam fixtures em `tests/fixtures/proc/` (snapshots estáticos de `/proc`), então rodam de forma determinística em qualquer máquina, incluindo os runners de CI.

## CI

`.github/workflows/ci.yml` builda e testa em `ubuntu-latest` (x64) e `ubuntu-24.04-arm` (arm64), ambos nativos — sem emulação.

## Licença

GPLv3 — veja [LICENSE](LICENSE). O Qt é usado via linkagem dinâmica (LGPLv3), sem modificações à biblioteca.
