
## Simulador de Hidrometro
![Programa em Ação](Aplicação-SHA.png) 

Projeto para a disciplina de Padrões de Projeto do curso de Engenharia de Computação do IFPB Campus Campina Grande.

O projeto simula um hidrômetro, desenvolvido em C++, onde simula o consumo de água tentando se aproximar da realidade. O usuário pode realizar a configuração do simulador para tentar se aproximar de uma situação desejada.

## Diagrama de Classe
```mermaid
classDiagram
    class Configuracao {
        +tick_ms
        +fluxo_inicial_mm
        +fluxo_minimo_mm
        +fluxo_maximo_mm
        +probabilidade_falha_aleatoria_por_min
        +fail_duration_sec_min
        +fail_duration_sec_max
        +simulate_pressure_variation
        +log_interval_sec
        +simulate_air_volume
        +air_volume_rate
        +load_from_file(filename)
        +save_to_file(filename)
    }

    class Controladora {
        -cfg: Configuracao
        -entrada: Entrada
        -display: Display
        -relogio: Relogio
        -hidrometro: Hidrometro
        -running: atomic<bool>
        -sim_thread: thread
        -rng: mt19937
        -per_tick_fail_prob: double
        +Controladora(cfg)
        +start()
        +stop()
        +set_flow(v)
        +toggle_fail_forced()
        +save_config(fn)
        +set_minmax(mn, mx)
        +getVolumeTotal()
        +getFluxoAtual()
        +isCurrentlyFailed()
    }

    class Display {
        +show(total_volume_l, fluxoAtual, tsec, fail_duration, fail_remaining)
        +clearScreen()
    }

    class Entrada {
        -flow_mm: double
        -min_flow: double
        -max_flow: double
        -forced_fail: bool
        +Entrada(initial_flow, min_flow, max_flow)
        +set_flow_mm(flow)
        +get_flow_mm()
        +set_min_max(min, max)
        +force_fail(fail)
        +is_forced_fail()
        +simulate_variation(simulate)
    }

    class Hidrometro {
        -entrada: Entrada&
        -relogio: Relogio&
        -total_volume_l: double
        +Hidrometro(entrada, relogio)
        +update(tick_sec, water_available, air_volume_rate)
        +get_total_volume_l()
    }

    class Relogio {
        -seconds_elapsed: atomic<long long>
        +Relogio()
        +tick(sec)
        +now()
    }

    class HidrometroUI {
        -fundo: QPixmap
        -ponteiro: QPixmap
        -currentVolume: double
        -litrosPositions: PointerPosition[10]
        -decimosPositions: PointerPosition[10]
        +HidrometroUI(initialFlow, parent)
        +setVolume(volume)
        +savePointerConfig(filename)
        +loadPointerConfig(filename)
        +setFaltaAgua(falta)
        +drawDigits(painter, total_volume_l)
        +drawPointers(painter, total_volume_l)
        +drawPointer(painter, value, isDecimos)
        +paintEvent(event)
    }

    class PointerPosition {
        +x: int
        +y: int
        +angle: double
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
```