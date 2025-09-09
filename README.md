classDiagram
    class Configuracao {
        +int tick_ms
        +double fluxo_inicial_mm
        +double fluxo_minimo_mm
        +double fluxo_maximo_mm
        +double probabilidade_falha_aleatoria_por_min
        +int fail_duration_sec_min
        +int fail_duration_sec_max
        +bool simulate_pressure_variation
        +int log_interval_sec
        +bool simulate_air_volume
        +double air_volume_rate
        +bool load_from_file(string filename)
        +void save_to_file(string filename)
    }

    class Controladora {
        -Configuracao cfg
        -Entrada entrada
        -Display display
        -Relogio relogio
        -Hidrometro hidrometro
        -atomic~bool~ running
        -thread sim_thread
        -mt19937 rng
        -double per_tick_fail_prob
        +Controladora(Configuracao& cfg)
        +void start()
        +void stop()
        +void set_flow(double v)
        +void toggle_fail_forced()
        +void save_config(string fn)
        +void set_minmax(double mn, double mx)
        +void print_status()
        +double getVolumeTotal()
        +double getFluxoAtual()
        -void run_loop()
    }

    class Display {
        +void show(double total_volume_l, double fluxoAtual, long long tsec)
        +void clearScreen()
    }

    class Entrada {
        -double flow_mm
        -double min_flow
        -double max_flow
        -bool forced_fail
        +Entrada(double initial_flow, double min_flow, double max_flow)
        +void set_flow_mm(double flow)
        +double get_flow_mm()
        +void set_min_max(double min, double max)
        +void force_fail(bool fail)
        +bool is_forced_fail()
        +void simulate_variation(bool simulate)
    }

    class Hidrometro {
        -Entrada& entrada
        -Relogio& relogio
        -double total_volume_l
        +Hidrometro(Entrada& entrada, Relogio& relogio)
        +void update(int tick_sec, bool water_available, double air_volume_rate)
        +double get_total_volume_l()
    }

    class Relogio {
        -atomic~long long~ seconds_elapsed
        +Relogio()
        +void tick(int sec)
        +long long now()
    }

    class HidrometroUI {
        -QPixmap fundo
        -QPixmap ponteiro
        -double currentVolume
        -PointerPosition litrosPositions[10]
        -PointerPosition decimosPositions[10]
        +HidrometroUI(QWidget* parent)
        +void setVolume(double volume)
        +void savePointerConfig(string filename)
        +void loadPointerConfig(string filename)
        -void drawDigits(QPainter& painter, double total_volume_l)
        -void drawPointers(QPainter& painter, double total_volume_l)
        -void drawPointer(QPainter& painter, int value, bool isDecimos)
        -void paintEvent(QPaintEvent* event)
    }

    class PointerPosition {
        +int x
        +int y
        +double angle
    }

    %% Relacionamentos
    Controladora --> Configuracao : usa
    Controladora --> Entrada : contém
    Controladora --> Display : contém
    Controladora --> Relogio : contém
    Controladora --> Hidrometro : contém

    Hidrometro --> Entrada : referencia
    Hidrometro --> Relogio : referencia

    HidrometroUI --> PointerPosition : usa

    note for Controladora "Gerencia o loop de simulação\nAtualiza Hidrometro, Entrada, Relogio\nUsa Display para logs no terminal"

    note for HidrometroUI "Interface gráfica Qt\nAtualizada por QTimer (1s)\nRecebe volume via setVolume()"