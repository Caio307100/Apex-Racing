# Apex Racing Engine

**Versão 0.2.0 — engine nativa em C++**

A Apex Racing Engine agora é um projeto nativo em **C++20**, usando **SDL2** para janela, eventos e entrada e **OpenGL** para renderização 3D. O motor do jogo, a física do veículo, a câmera e o circuito são implementados no próprio projeto.

## Requisitos

- C++20 (GCC 13+ ou Clang 16+)
- CMake 3.20+
- SDL2
- OpenGL

No Arch Linux:

```bash
sudo pacman -S --needed base-devel cmake sdl2 mesa
```

## Compilar

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/apex-racing
```


## Executar com dois cliques (Linux)

1. Baixe `Apex-Racing.sh` na página do projeto.
2. Nas propriedades do arquivo, marque **Permitir executar como programa** (se o gerenciador de arquivos solicitar).
3. Dê dois cliques no arquivo e escolha **Executar**.

Na primeira abertura, o script baixa automaticamente o pacote do último release e inicia o jogo. Depois, ele reutiliza o executável instalado em `~/.local/share/apex-racing`.

O launcher precisa de Python 3 e internet na primeira execução. O jogo requer SDL2 e OpenGL disponíveis no sistema. No Arch Linux, instale com:

```bash
sudo pacman -S --needed sdl2 mesa
```

## Releases

As versões marcadas com uma tag `v*` geram automaticamente um pacote Linux x86_64 e publicam o arquivo `apex-racing-linux-x86_64.tar.gz` na página de Releases.

## Controles

| Tecla | Ação |
|---|---|
| W / S ou ↑ / ↓ | Acelerar / frear e ré |
| A / D ou ← / → | Virar |
| P | Pausar |
| R | Reiniciar posição e corrida |
| Esc | Sair |

## O que existe nesta versão

- Loop principal em C++ com delta time.
- Janela e eventos com SDL2.
- Renderização 3D OpenGL.
- Circuito oval procedural, asfalto, zebras e marcação central.
- Carro 3D construído por geometria.
- Física arcade com aceleração, frenagem, ré, direção, inércia e perda de velocidade fora da pista.
- Câmera chase que acompanha o veículo.
- Velocidade, voltas e estado da corrida no título da janela.

## Arquitetura

- `engine/main.cpp`: inicialização SDL/OpenGL, loop, renderização e integração dos sistemas.
- `CMakeLists.txt`: configuração e build nativo.

A engine não depende de navegador, Node.js, Vite ou Three.js para executar o jogo.
