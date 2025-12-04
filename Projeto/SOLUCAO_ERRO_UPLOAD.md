# 🔧 Solução para Erro de Upload do ESP32

## ❌ Erro Atual
```
termios.error: (22, 'Invalid argument')
Failed to connect to ESP32: No serial data received.
```

## 🔍 Diagnóstico

O problema pode ser causado por:

1. **Porta USB não detectada** - O dispositivo pode ter sido desconectado
2. **Porta errada sendo usada** - PlatformIO detectou Bluetooth em vez de USB
3. **Driver USB não instalado** - Falta driver para o chip USB do ESP32
4. **Porta em uso** - Outro processo está usando a porta

## ✅ Soluções (Tente em ordem)

### 1️⃣ Verificar se o ESP32 está conectado

```bash
# Execute este comando para ver portas disponíveis
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/Projeto
./detectar_porta.sh
```

**Ou manualmente:**
```bash
ls -la /dev/cu.* | grep -v Bluetooth
```

### 2️⃣ Desconectar e Reconectar o ESP32

1. **Desconecte** o cabo USB do ESP32
2. **Aguarde 5 segundos**
3. **Reconecte** o cabo USB
4. **Aguarde 10 segundos** para o sistema detectar
5. Execute novamente: `./detectar_porta.sh`

### 3️⃣ Verificar se a porta está correta no platformio.ini

O arquivo já está configurado para usar:
```
upload_port = /dev/cu.usbserial-56AB0008831
monitor_port = /dev/cu.usbserial-56AB0008831
```

**Se a porta mudou após reconectar**, atualize o `platformio.ini` com a nova porta.

### 4️⃣ Instalar Driver USB (se necessário)

Dependendo do chip USB do seu ESP32:

**Para CH340:**
```bash
# macOS
brew install --cask wch-ch34x-usb-serial-driver
```

**Para CP210x:**
- Baixe em: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

**Para FT232:**
- Baixe em: https://ftdichip.com/drivers/vcp-drivers/

### 5️⃣ Fechar processos que podem estar usando a porta

```bash
# Ver processos usando portas seriais
lsof | grep cu.usb

# Se encontrar algum processo, mate-o:
kill -9 <PID>
```

### 6️⃣ Tentar modo de upload manual (Boot Mode)

1. **Mantenha pressionado** o botão **BOOT** (ou **GPIO0**) no ESP32
2. **Enquanto pressiona**, clique em **Upload** no PlatformIO
3. **Solte o botão** quando o upload começar (aparecer "Connecting...")

### 7️⃣ Verificar permissões da porta

```bash
# Dar permissão de leitura/escrita
sudo chmod 666 /dev/cu.usbserial-56AB0008831
```

### 8️⃣ Tentar outra porta USB

Se possível, tente conectar o ESP32 em outra porta USB do computador.

## 🎯 Passos Recomendados Agora

1. **Execute o script de detecção:**
   ```bash
   cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/Projeto
   ./detectar_porta.sh
   ```

2. **Se a porta aparecer**, verifique se está correta no `platformio.ini`

3. **Se não aparecer**, desconecte e reconecte o ESP32

4. **Tente fazer upload novamente**

## 📝 Verificar Logs Detalhados

Para ver mais detalhes do erro:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/Projeto
pio run --target upload -v
```

O `-v` mostra logs detalhados que podem ajudar a identificar o problema.

## 🔄 Alternativa: Usar outra ferramenta

Se nada funcionar, tente usar o **Arduino IDE** para fazer upload e depois volte para o PlatformIO apenas para desenvolvimento.

## ⚠️ Checklist Final

- [ ] ESP32 está conectado via USB?
- [ ] LED do ESP32 está piscando/aceso?
- [ ] Porta aparece quando executo `./detectar_porta.sh`?
- [ ] Porta no `platformio.ini` está correta?
- [ ] Nenhum monitor serial está aberto?
- [ ] Tentei desconectar e reconectar?
- [ ] Tentei pressionar o botão BOOT durante upload?

---

**💡 Dica:** O erro `termios.error` geralmente indica problema de comunicação serial. A solução mais comum é desconectar/reconectar o dispositivo.

