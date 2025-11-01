# Cliente CLI do Simulador SHA - Guia de Uso

## Compilação e Execução

```bash
# Compilar
make clean && make

# Executar
./hidrometro_sim
```

## Funcionalidades Disponíveis

O Cliente CLI fornece uma interface completa para operar o Sistema de Simulação SHA através das 5 funcionalidades principais da facade:

### 1. Configuração do Simulador SHA (`configSimulatorSHA`)

**Opções disponíveis:**
- **Carregar de arquivo**: Carrega configurações do arquivo padrão (config.json)
- **Configuração personalizada**: Define tick_ms, fluxo inicial, mínimo e máximo
- **Configurar tempo**: Modifica apenas o tempo de tick
- **Configurar fluxo**: Modifica apenas parâmetros de fluxo

**Exemplo de uso:**
```
1. Configurar Simulador SHA
2. Configuração personalizada
Tempo de tick (ms): 500
Fluxo inicial (L/min): 25.0
Fluxo mínimo (L/min): 5.0
Fluxo máximo (L/min): 100.0
```

### 2. Criação de Instância SHA (`criaSHA`)

**Funcionalidade:**
- Cria uma nova instância do SHA com configurações padrão
- Retorna um ID único para a instância
- Permite múltiplas instâncias simultâneas

**Exemplo de uso:**
```
2. Criar Nova Instância SHA
✓ SHA criado com sucesso!
ID da Instância: 0
Total de SHAs ativos: 1
```

### 3. Finalização de Instância SHA (`finalizaSHA`)

**Funcionalidade:**
- Lista todas as instâncias ativas
- Permite escolher qual instância finalizar
- Remove a instância da lista de ativos

**Exemplo de uso:**
```
3. Finalizar Instância SHA
=== INSTÂNCIAS SHA ATIVAS ===
Total de instâncias: 2
IDs das instâncias: 0, 1
Digite o ID da instância SHA para finalizar: 0
✓ SHA 0 finalizado com sucesso!
```

### 4. Modificação de Vazão SHA (`modificaVazaoSHA`)

**Funcionalidade:**
- Lista instâncias ativas
- Permite alterar a vazão de uma instância específica
- Validação de limites de vazão

**Exemplo de uso:**
```
4. Modificar Vazão SHA
IDs das instâncias: 0, 1
Digite o ID da instância SHA: 0
Digite a nova vazão (L/min): 35.5
✓ Vazão do SHA 0 alterada para 35.5 L/min!
```

### 5. Habilitação de Geração de Imagem (`habilitaGeracaoImagemSHA`)

**Funcionalidade:**
- Lista instâncias ativas
- Habilita a interface gráfica para uma instância específica
- Permite visualização em tempo real do hidrômetro

**Exemplo de uso:**
```
5. Habilitar Geração de Imagem
IDs das instâncias: 0
Digite o ID da instância SHA: 0
✓ Geração de imagem habilitada para SHA 0!
A interface gráfica deve aparecer agora.
```

## Demonstrações Automáticas

### Demo: Múltiplas Instâncias
Demonstra o gerenciamento de várias instâncias SHA simultaneamente:
1. Cria 3 instâncias SHA
2. Configura vazões diferentes (10, 25, 40 L/min)
3. Habilita geração de imagem para todas
4. Aguarda 10 segundos
5. Finaliza todas as instâncias

### Demo: Execução Automática
Demonstra o ciclo completo de vida de um SHA:
1. Configuração personalizada (tick=500ms, fluxos específicos)
2. Criação de SHA
3. Habilitação de visualização
4. Ciclo automático de modificações de vazão
5. Finalização

## Menu Principal

```
========================================
           MENU PRINCIPAL
========================================
SHAs Ativos: 0
----------------------------------------
1. Configurar Simulador SHA
2. Criar Nova Instância SHA
3. Finalizar Instância SHA
4. Modificar Vazão SHA
5. Habilitar Geração de Imagem
6. Listar SHAs Ativos
7. Demo - Múltiplas Instâncias
8. Demo - Execução Automática
0. Sair
========================================
```

## Exemplo de Uso Completo

```bash
# 1. Executar o cliente
./hidrometro_sim

# 2. Configurar o simulador (Opção 1)
# Escolher "1. Carregar configuração de arquivo (padrão)"

# 3. Criar um SHA (Opção 2)
# Sistema retorna: "SHA criado com sucesso! ID da Instância: 0"

# 4. Modificar vazão (Opção 4)
# Escolher ID: 0, Nova vazão: 30.0

# 5. Habilitar visualização (Opção 5)
# Escolher ID: 0
# Interface gráfica será exibida

# 6. Finalizar quando necessário (Opção 3)
# Escolher ID: 0

# 7. Sair (Opção 0)
```

## Validações e Tratamento de Erros

- **IDs inválidos**: Sistema valida se o ID da instância SHA existe
- **Limites de vazão**: Valores são validados dentro dos limites configurados
- **Instâncias vazias**: Sistema informa quando não há instâncias ativas
- **Erros de configuração**: Mensagens detalhadas sobre problemas de configuração
- **Entradas inválidas**: Sistema solicita nova entrada em caso de dados inválidos

## Arquitetura

O Cliente CLI atua como um proxy para a SystemFacade, demonstrando como um cliente externo pode operar completamente o Sistema SHA através de uma interface simplificada e intuitiva.

```
Cliente CLI → SystemFacade → Subsistemas SHA
```

Isso demonstra a eficácia do padrão Facade em fornecer uma interface unificada e simples para um sistema complexo.