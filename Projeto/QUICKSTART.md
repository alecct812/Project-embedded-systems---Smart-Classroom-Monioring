# 🚀 Guia Rápido - Mosquitto Docker

## ⚡ Início Rápido (5 minutos)

### 1. Inicie o Mosquitto:
```powershell
docker run -d --name mosquitto -p 1883:1883 eclipse-mosquitto:latest
```

### 2. Descubra seu IP:
```powershell
ipconfig
```
Anote o **IPv4** (ex: `192.168.1.100`)

### 3. Configure no código:
```cpp
const char *mqtt_server = "192.168.1.100"; // Seu IP aqui
```

### 4. Upload:
```bash
pio run -t upload
pio device monitor
```

### 5. Monitore:
```powershell
docker exec -it mosquitto mosquitto_sub -t "sala/#" -v
```

---

## 📋 Comandos Essenciais

```powershell
# Ver containers
docker ps

# Parar
docker stop mosquitto

# Iniciar
docker start mosquitto

# Logs
docker logs mosquitto

# Remover
docker rm -f mosquitto

# Monitorar tópicos
docker exec -it mosquitto mosquitto_sub -t "sala/#" -v

# Publicar teste
docker exec -it mosquitto mosquitto_pub -t "sala/teste" -m "ok"
```

---

## ✅ Pronto!

Consulte o **README.md** para mais detalhes.
