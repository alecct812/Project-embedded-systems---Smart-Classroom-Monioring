#!/bin/bash
# Script para detectar portas USB serial disponíveis

echo "🔍 Procurando portas USB serial disponíveis..."
echo ""

# Lista todas as portas cu.*
echo "📡 Portas disponíveis:"
ls -la /dev/cu.* 2>/dev/null | grep -v "Bluetooth" | while read line; do
    port=$(echo $line | awk '{print $NF}')
    if [ -n "$port" ]; then
        echo "  ✅ $port"
    fi
done

echo ""
echo "🔌 Portas USB Serial (recomendadas):"
ls -la /dev/cu.* 2>/dev/null | grep -E "usb|serial|SLAB|CH340|CP210|FT232" | while read line; do
    port=$(echo $line | awk '{print $NF}')
    if [ -n "$port" ]; then
        echo "  ✅ $port"
    fi
done

echo ""
echo "💡 Para usar uma porta específica, edite platformio.ini e adicione:"
echo "   upload_port = /dev/cu.SUA_PORTA_AQUI"
echo "   monitor_port = /dev/cu.SUA_PORTA_AQUI"

