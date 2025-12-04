# 🚨 SOLUÇÃO IMEDIATA - Erro de Upload ESP32

## ⚠️ PROBLEMA IDENTIFICADO

A porta USB serial **NÃO está sendo detectada** pelo sistema. Isso significa que:

1. ❌ O ESP32 pode estar desconectado
2. ❌ O driver USB pode não estar instalado
3. ❌ O cabo USB pode estar com problema
4. ❌ A porta pode ter mudado após reconexão

## ✅ SOLUÇÃO PASSO A PASSO

### PASSO 1: Verificar Conexão Física

1. **Olhe o ESP32** - Há algum LED aceso/piscando?
2. **Verifique o cabo USB** - Está bem conectado em ambos os lados?
3. **Tente outra porta USB** - Conecte em outra porta do computador

### PASSO 2: Desconectar e Reconectar

```bash
# 1. Desconecte o ESP32 completamente
# 2. Aguarde 10 segundos
# 3. Reconecte o ESP32
# 4. Aguarde 15 segundos para o sistema detectar
```

### PASSO 3: Verificar se a Porta Apareceu

Execute no terminal:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/Projeto
ls -la /dev/cu.* | grep -v Bluetooth
```

**Se aparecer uma porta** (ex: `/dev/cu.usbserial-XXXXX`):
- Anote o nome completo da porta
- Vá para o PASSO 4

**Se NÃO aparecer nenhuma porta**:
- Vá para o PASSO 5 (Instalar Driver)

### PASSO 4: Atualizar platformio.ini

Se a porta mudou, edite o arquivo `platformio.ini`:

```ini
upload_port = /dev/cu.NOVA_PORTA_AQUI
monitor_port = /dev/cu.NOVA_PORTA_AQUI
```

### PASSO 5: Instalar Driver USB (se necessário)

**Identifique o chip USB do seu ESP32:**

Execute:
```bash
system_profiler SPUSBDataType | grep -A 5 -i "serial\|ch340\|cp210\|ft232"
```

**Para CH340 (mais comum em ESP32 baratos):**
```bash
brew install --cask wch-ch34x-usb-serial-driver
```

Depois de instalar, **reinicie o computador**.

**Para CP210x:**
- Baixe: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- Instale e reinicie

**Para FT232:**
- Baixe: https://ftdichip.com/drivers/vcp-drivers/
- Instale e reinicie

### PASSO 6: Tentar Upload com Botão BOOT

1. **Abra o PlatformIO** e prepare para fazer upload
2. **Mantenha pressionado** o botão **BOOT** (ou **GPIO0**) no ESP32
3. **Clique em Upload** no PlatformIO
4. **Quando aparecer "Connecting..."**, solte o botão BOOT
5. Aguarde o upload completar

### PASSO 7: Se Nada Funcionar - Modo Alternativo

**Opção A: Remover configuração de porta**

Edite `platformio.ini` e **comente** as linhas de porta:

```ini
; upload_port = /dev/cu.usbserial-56AB0008831
; monitor_port = /dev/cu.usbserial-56AB0008831
```

Isso fará o PlatformIO tentar detectar automaticamente (mas pode pegar Bluetooth).

**Opção B: Usar Arduino IDE temporariamente**

1. Instale Arduino IDE
2. Faça upload pelo Arduino IDE
3. Depois volte para PlatformIO apenas para desenvolvimento

## 🔍 VERIFICAÇÃO FINAL

Após seguir os passos, execute:

```bash
# Ver portas disponíveis
ls -la /dev/cu.* | grep -v Bluetooth

# Tentar upload com logs detalhados
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/Projeto
pio run --target upload -v
```

## 📞 O QUE FAZER AGORA

1. ✅ **Desconecte e reconecte o ESP32**
2. ✅ **Aguarde 15 segundos**
3. ✅ **Execute:** `ls -la /dev/cu.* | grep -v Bluetooth`
4. ✅ **Se aparecer porta**, atualize `platformio.ini`
5. ✅ **Tente fazer upload novamente**

---

**💡 Dica:** O erro `termios.error` quase sempre é resolvido desconectando e reconectando o dispositivo, ou instalando o driver USB correto.

