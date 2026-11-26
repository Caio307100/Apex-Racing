#!/usr/bin/env bash
set -Eeuo pipefail
REPO="Caio307100/Apex-Racing"
APP_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/apex-racing"
ARCHIVE="$APP_DIR/apex-racing-linux-x86_64.tar.gz"
BIN="$APP_DIR/apex-racing"
if ! command -v python3 >/dev/null 2>&1; then
  printf 'Erro: Python 3 é necessário.\n'
  read -r -p 'Pressione Enter para sair...'
  exit 1
fi
mkdir -p "$APP_DIR"
if [[ ! -x "$BIN" ]]; then
  printf 'Baixando Apex Racing...\n'
  python3 - "$REPO" "$ARCHIVE" <<'PY'
import json, pathlib, sys, urllib.request
repo, archive = sys.argv[1], pathlib.Path(sys.argv[2])
headers = {"User-Agent": "Apex-Racing-Launcher"}
req = urllib.request.Request(f"https://api.github.com/repos/{repo}/releases/latest", headers=headers)
with urllib.request.urlopen(req, timeout=30) as response:
    release = json.load(response)
assets = [a for a in release.get("assets", []) if a["name"] == "apex-racing-linux-x86_64.tar.gz"]
if not assets:
    raise SystemExit("Não encontrei o pacote Linux no último release.")
req = urllib.request.Request(assets[0]["browser_download_url"], headers=headers)
with urllib.request.urlopen(req, timeout=120) as response:
    archive.write_bytes(response.read())
print("Download concluído:", release["tag_name"])
PY
  python3 - "$ARCHIVE" "$APP_DIR" <<'PY'
import pathlib, sys, tarfile
with tarfile.open(sys.argv[1], "r:gz") as archive:
    archive.extractall(sys.argv[2], filter="data")
PY
  chmod +x "$BIN"
fi
"$BIN"
