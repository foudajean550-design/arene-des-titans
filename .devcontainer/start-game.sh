#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

# Expose the graphical desktop only through the Codespaces-forwarded web port.
Xvfb :0 -screen 0 1280x720x24 +extension GLX +render -noreset >/tmp/arene-xvfb.log 2>&1 &
export DISPLAY=:0
export LIBGL_ALWAYS_SOFTWARE=1
openbox >/tmp/arene-openbox.log 2>&1 &
x11vnc -display :0 -forever -shared -nopw -localhost -rfbport 5900 >/tmp/arene-x11vnc.log 2>&1 &

# Make the forwarded port open directly into a connected noVNC session.
NOVNC_ROOT="$(mktemp -d)"
ln -s /usr/share/novnc/* "$NOVNC_ROOT/"
cat >"$NOVNC_ROOT/index.html" <<'HTML'
<!doctype html>
<meta charset="utf-8">
<meta http-equiv="refresh" content="0; url=/vnc.html?autoconnect=true&amp;resize=scale">
<a href="/vnc.html?autoconnect=true&amp;resize=scale">Ouvrir le jeu</a>
HTML
websockify --web="$NOVNC_ROOT" 6080 127.0.0.1:5900 >/tmp/arene-websockify.log 2>&1 &

sleep 2
./build/arene-des-titans >/tmp/arene-game.log 2>&1 &
