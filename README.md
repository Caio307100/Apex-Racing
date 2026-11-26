# Apex Racing

Jogo de corrida 3D para navegador, construído com uma engine própria em JavaScript. Three.js é usado como camada de renderização; ciclo de jogo, sistemas, dinâmica arcade, controles, câmera e regras da corrida ficam no código do projeto.

## Executar
```bash
npm install
npm run dev
``

## Controles
- **W / ↑** acelerar
- **S / ↓** frear e dar ré
- **A / ←** virar à esquerda
- **D / →** virar à direita
- **R** reposicionar o carro

## Estrutura
- `src/engine/Engine.js`: cena, câmera, renderização e ciclo principal.
- `src/engine/VehiclePhysics.js`: aceleração, frenagem, arrasto e direção.
- `src/game/track.js`: circuito procedural e iluminação.
- `src/game/car.js`: carro 3D procedural.
- `src/main.js`: corrida, entradas, HUD e câmera.

## Roadmap
- [x] Engine base e renderização 3D
- [x] Circuito procedural e carro
- [x] Controles e física arcade inicial
- [x] HUD e cronômetro
- [ ] Colisões completas, áudio e adversários
- [ ] Seleção de carros e editor de pistas

**0.1.0 — First Track**