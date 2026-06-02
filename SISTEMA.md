# 📚 SmartCampus UTA — Documentación del Sistema

> **Universidad Técnica de Ambato (UTA) · Campus Huachi**  
> Sistema de Gestión Universitaria con Navegación Georreferenciada

---

## 📋 Tabla de Contenidos

1. [¿Qué es SmartCampus UTA?](#qué-es-smartcampus-uta)
2. [¿Por qué se construyó así?](#por-qué-se-construyó-así)
3. [Arquitectura General](#arquitectura-general)
4. [Tecnologías Usadas](#tecnologías-usadas)
5. [Módulos del Sistema](#módulos-del-sistema)
6. [Estructuras de Datos Implementadas](#estructuras-de-datos-implementadas)
7. [El Mapa del Campus](#el-mapa-del-campus)
8. [Seguridad](#seguridad)
9. [Cómo Funciona Internamente](#cómo-funciona-internamente)
10. [Despliegue con Docker](#despliegue-con-docker)
11. [Guía Rápida de Uso](#guía-rápida-de-uso)

---

## ¿Qué es SmartCampus UTA?

**SmartCampus UTA** es una plataforma web universitaria diseñada para digitalizar y mejorar los procesos administrativos de la Universidad Técnica de Ambato. El sistema integra:

- 🎫 **Gestión de turnos** para atención en ventanilla (como una fila digital)
- 📋 **Seguimiento de trámites** académicos y administrativos
- 📁 **Repositorio de documentos** organizados en categorías
- 🏛️ **Árbol de dependencias** institucionales
- 🗺️ **Mapa interactivo del campus** con navegación inteligente (Dijkstra)
- 👤 **Sistema de roles** (Administrador, Secretaria, Estudiante)

La idea central es que un estudiante pueda entrar al sistema, ver el estado de su trámite, sacar un turno y además saber cómo llegar físicamente a la oficina que necesita, todo desde la misma aplicación.

---

## ¿Por qué se Construyó Así?

### El problema que resuelve

Antes de este sistema, los estudiantes de la UTA debían:
1. Ir físicamente a una ventanilla sin saber si había fila
2. No tenían forma de rastrear el estado de un trámite en línea
3. Los nuevos estudiantes no sabían ubicarse dentro del campus
4. La gestión de documentos era manual y desorganizada

### Las decisiones de diseño clave

**¿Por qué C++ en el backend?**  
C++ fue elegido como lenguaje del servidor porque el proyecto requería demostrar el uso real de **estructuras de datos avanzadas** implementadas desde cero: grafos, árboles, listas enlazadas, pilas. En Python o JavaScript estas estructuras ya están resueltas por librerías; en C++ hay que construirlas, lo que permite entender cómo funcionan realmente.

**¿Por qué un mapa real (OpenStreetMap) y no un dibujo?**  
Un mapa dibujado a mano quedaría desactualizado y no reflejaría la realidad geográfica. Al usar coordenadas GPS reales y tiles de OpenStreetMap, cualquier persona que conozca la UTA puede verificar que las ubicaciones son correctas, y el mapa se mantiene fiel al territorio real de Ambato.

**¿Por qué Dijkstra para las rutas?**  
Dijkstra es el algoritmo estándar para encontrar el camino más corto en un grafo con pesos. En el contexto del campus, cada sendero peatonal tiene una distancia real en metros (calculada con la fórmula Haversine sobre las coordenadas GPS). Dijkstra garantiza que la ruta sugerida siempre será la más corta entre las disponibles en el grafo que el administrador haya definido.

**¿Por qué PostgreSQL y no SQLite?**  
SQLite es excelente para proyectos pequeños, pero no soporta bien la **concurrencia**: si dos estudiantes sacan turno al mismo tiempo, SQLite puede bloquearse. PostgreSQL está diseñado para manejar múltiples conexiones simultáneas de forma segura, algo crítico en un sistema universitario con cientos de usuarios.

**¿Por qué Docker?**  
Para garantizar que el sistema funcione de forma idéntica en cualquier computadora, sin importar el sistema operativo. Un desarrollador en Windows, otro en Linux y el servidor de producción en la nube ejecutan exactamente el mismo entorno gracias a los contenedores.

---

## Arquitectura General

El sistema sigue una arquitectura de **tres capas separadas**, cada una en su propio contenedor Docker:

```
┌─────────────────────────────────────────────────────────┐
│                    NAVEGADOR WEB                        │
│           (Chrome, Firefox, Edge, etc.)                 │
└───────────────────────┬─────────────────────────────────┘
                        │ HTTP (puerto 8090)
                        ▼
┌─────────────────────────────────────────────────────────┐
│            FRONTEND · Nginx + HTML/CSS/JS               │
│  Sirve las páginas estáticas y actúa como proxy inverso │
│  hacia el backend para evitar problemas de CORS         │
└───────────────────────┬─────────────────────────────────┘
                        │ HTTP (puerto 8080, interno)
                        ▼
┌─────────────────────────────────────────────────────────┐
│          BACKEND · C++ con framework Crow               │
│  Contiene toda la lógica de negocio:                    │
│  · Autenticación JWT                                    │
│  · Dijkstra sobre el grafo del campus                   │
│  · Gestión de turnos (cola FIFO)                        │
│  · APIs REST para todos los módulos                     │
└───────────────────────┬─────────────────────────────────┘
                        │ PostgreSQL protocol (puerto 5432)
                        ▼
┌─────────────────────────────────────────────────────────┐
│           BASE DE DATOS · PostgreSQL 15                 │
│  Almacena de forma persistente:                         │
│  · Usuarios, roles y credenciales                       │
│  · Turnos, trámites, documentos                         │
│  · Grafo del campus (nodos y aristas GPS)               │
└─────────────────────────────────────────────────────────┘
```

---

## Tecnologías Usadas

### Backend

| Tecnología | Versión | Rol | Por qué se eligió |
|---|---|---|---|
| **C++** | C++17 | Lenguaje principal | Permite implementar estructuras de datos desde cero y tiene el mejor rendimiento para algoritmos como Dijkstra |
| **Crow** | 1.0 | Framework HTTP | Librería C++ minimalista para crear APIs REST, con soporte nativo para routing y middleware |
| **libpqxx** | 6.4 | Driver PostgreSQL | Librería oficial de C++ para conectarse a PostgreSQL de forma segura |
| **nlohmann/json** | 3.11 | Serialización JSON | La librería JSON más popular para C++, con una sintaxis muy limpia |
| **libjwt / HS256** | — | Autenticación | Estándar de la industria para tokens de sesión sin estado |

### Frontend

| Tecnología | Rol | Por qué se eligió |
|---|---|---|
| **HTML5 + CSS3 + JavaScript** | Interfaz de usuario | Sin frameworks pesados (React/Angular). El sistema debe ser ligero y funcionar en computadoras básicas del campus |
| **Leaflet.js** | Mapa interactivo | Librería open-source de mapas. Gratuita, liviana (~40KB) y compatible con OpenStreetMap |
| **OpenStreetMap** | Tiles del mapa | Mapa colaborativo y gratuito (como Wikipedia pero de mapas). No requiere API Key ni tiene límites de uso |
| **Nominatim** | Búsqueda de lugares | Geocodificador gratuito de OSM. Permite buscar "FISEI UTA Ambato" y centra el mapa en ese lugar |

### Infraestructura

| Tecnología | Rol | Por qué se eligió |
|---|---|---|
| **Docker** | Contenedorización | Garantiza que el sistema funciona igual en cualquier entorno |
| **Docker Compose** | Orquestación | Levanta los 3 servicios (DB, Backend, Frontend) con un solo comando |
| **PostgreSQL 15** | Base de datos | Robusto, open-source, soporta concurrencia real |
| **Nginx** | Servidor web / Proxy | El servidor web más eficiente para archivos estáticos, y actúa como puerta de entrada al sistema |

---

## Módulos del Sistema

### 🎫 Módulo de Turnos

Implementa una **cola FIFO (First In, First Out)** para gestionar la atención en ventanilla.

**¿Cómo funciona?**
1. El estudiante hace clic en "Nuevo Turno" y elige el tipo de trámite
2. El sistema le asigna un número (ej. *Turno #47*)
3. La secretaria ve la cola en tiempo real y llama al "Siguiente"
4. El turno pasa de estado `pendiente` → `en_proceso` → `atendido`

**La estructura de datos real:** La cola está implementada en C++ con una `std::queue` que opera sobre los registros de la base de datos. Esto garantiza que el orden de llegada nunca se altera.

### 📋 Módulo de Trámites

Permite a los estudiantes iniciar solicitudes (certificados de matrícula, récords académicos, etc.) y hacer seguimiento de su estado.

**Estados de un trámite:** `pendiente` → `en_proceso` → `completado` / `rechazado`

### 📜 Módulo de Historial

Registra todas las acciones del usuario en una **pila LIFO (Last In, First Out)**. Esto permite ver las últimas acciones realizadas y tiene una función de "deshacer" (undo) que revierte la última acción.

**La estructura de datos real:** Implementada como una `std::stack` en C++ en memoria, sincronizada con la tabla `historial_acciones` en PostgreSQL.

### 📁 Módulo de Documentos

Organiza documentos en categorías jerárquicas usando un **Árbol N-ario** (un árbol donde cada nodo puede tener múltiples hijos).

```
Documentos
├── Formularios
│   ├── Matrícula
│   └── Retiro de materias
├── Reglamentos
│   ├── Académico
│   └── Disciplinario
└── Certificados
    └── Récord Académico
```

### 🏛️ Módulo de Dependencias

Representa el **organigrama oficial completo de la UTA** en un **Árbol N-ario** con 78 nodos reales. La interfaz muestra el árbol en modo colapsable (sin scroll excesivo), con búsqueda instantánea y un panel lateral de detalle:

```
Universidad Técnica de Ambato
├── Rectorado (11 dependencias)
│   ├── Secretaría General
│   ├── Dirección Financiera
│   ├── Dirección de Gestión de la Calidad
│   └── ...
├── Vicerrectorado Académico (6)
│   ├── Dirección Académica
│   └── Dirección de Investigación y Desarrollo
├── Vicerrectorado Administrativo (3)
├── Vicerrectorado de Investigación e Innovación (4)
├── FISEI — Sistemas, Electrónica e Industrial (3 carreras)
├── FCA — Ciencias Administrativas (2 carreras)
├── FCS — Ciencias de la Salud · Ingahurco (4 carreras)
├── FCAG — Agropecuarias · Querochaca (2 carreras)
└── Centros Universitarios (11 centros/direcciones)
```

**Funcionalidades de la vista:**
- 🔍 Búsqueda instantánea que abre automáticamente los nodos padres del resultado
- ▶/▼ Colapsar/expandir ramas individualmente
- ⊞ Botones "Expandir todo" y "Colapsar todo"
- Panel lateral sticky con descripción, nivel jerárquico e ícono por nivel
- Solo el Admin puede agregar nuevas dependencias

---

## Estructuras de Datos Implementadas

Este es uno de los aspectos académicamente más importantes del proyecto. Todas las estructuras están implementadas en C++ **desde cero**, sin usar las librerías estándar de C++ (como `std::map` o `std::list`) para las funciones principales:

### 🕸️ Grafo con Lista de Adyacencia

**Archivo:** `backend/src/domain/Grafo.h`

Representa el campus universitario como una red de puntos (nodos) conectados por caminos (aristas). Cada nodo almacena coordenadas GPS reales (`latitud`, `longitud`), y cada arista almacena la distancia en metros calculada con la fórmula **Haversine**.

```
Nodo: Rectorado (lat: -1.2650, lng: -78.6270)
  └── Arista → Biblioteca (distancia: 85m)
  └── Arista → Cruce Central (distancia: 40m)

Nodo: Biblioteca (lat: -1.2648, lng: -78.6265)
  └── Arista → Rectorado (distancia: 85m)
  └── Arista → FISI (distancia: 120m)
```

**¿Por qué Lista de Adyacencia y no Matriz?** Porque el campus tiene pocos caminos entre los muchos edificios posibles. Una matriz desperdiciaría memoria con ceros; la lista de adyacencia solo almacena conexiones que realmente existen.

### 🔍 Algoritmo de Dijkstra

**Archivo:** `backend/src/domain/Grafo.h` (método `dijkstra()`)

El algoritmo de la ruta más corta. Funciona así:

1. Parte del nodo origen, asignándole distancia `0` y a todos los demás distancia `∞`
2. Visita el nodo no visitado con menor distancia acumulada
3. Para cada vecino, calcula: `distancia_actual + peso_arista`
4. Si ese valor es menor que la distancia registrada del vecino, lo actualiza
5. Repite hasta llegar al destino o visitar todos los nodos

**Resultado:** El camino de menor distancia total en metros, que sigue únicamente los senderos peatonales definidos por el administrador (nunca atraviesa edificios).

### 🌳 Árbol N-ario

**Archivo:** `backend/src/domain/Arbol.h`

Usado para representar jerarquías (documentos y dependencias). Cada nodo puede tener cualquier número de hijos.

### 📋 Lista Secuencial

**Archivo:** `backend/src/domain/ListaSecuencial.h`

Lista enlazada simple usada para el catálogo de tipos de trámite. Permite recorrer todos los tipos en orden.

---

## El Mapa del Campus

### ¿Cómo funciona el mapa interactivo?

1. **Los tiles del mapa** (las imágenes de las calles) vienen de los servidores de OpenStreetMap, igual que en Google Maps pero con datos abiertos.

2. **Los edificios y senderos** son datos propios de la UTA almacenados en PostgreSQL, con coordenadas GPS reales obtenidas visitando o verificando la ubicación en el mapa.

3. **El administrador** puede agregar edificios haciendo clic directamente sobre el mapa en la posición correcta, sin necesidad de conocer coordenadas numéricas.

4. **El algoritmo Dijkstra** corre en el servidor C++ y devuelve la lista de nodos que forman el camino más corto. El frontend dibuja esa ruta como una línea verde sobre el mapa real.

### Por qué las rutas no atraviesan edificios

La clave está en que Dijkstra **no dibuja líneas rectas**: sigue el grafo que el administrador define. Si el administrador crea senderos rodeando los edificios (como hace Google Maps con sus calles), la ruta también los rodeará. Si un camino directo no existe en el grafo, Dijkstra lo ignorará.

### Fórmula Haversine

Para calcular la distancia real en metros entre dos puntos GPS, el sistema usa la fórmula Haversine, que considera la curvatura de la Tierra:

```
a = sin²(Δlat/2) + cos(lat1) · cos(lat2) · sin²(Δlng/2)
distancia = 2R · atan2(√a, √(1−a))
donde R = 6,371,000 metros (radio de la Tierra)
```

---

## Seguridad

### Autenticación con JWT

**JWT (JSON Web Token)** es un estándar de la industria para sesiones sin estado. Cuando el usuario inicia sesión:

1. El servidor verifica email + contraseña (hasheada en la BD)
2. Genera un **token firmado** con una clave secreta, que contiene el ID del usuario, su rol y la fecha de expiración
3. El navegador guarda ese token y lo envía en cada petición futura
4. El servidor verifica la firma del token para saber que es auténtico, sin consultar la base de datos en cada petición

**¿Por qué esto es seguro?** El token está firmado con una clave que solo conoce el servidor. Nadie puede modificar el rol dentro del token sin que el servidor lo detecte.

### Control de Acceso por Rol

Cada endpoint de la API verifica el rol del usuario. El backend filtra los datos en la consulta SQL — el estudiante nunca recibe información de otros usuarios:

| Acción | Estudiante | Secretaria | Admin |
|---|---|---|---|
| Portal personalizado (Mi Portal) | ✅ propio | ❌ | ❌ |
| Dashboard administrativo | ❌ | ✅ | ✅ |
| Ver mapa del campus | ✅ | ✅ | ✅ |
| Calcular ruta | ✅ | ✅ | ✅ |
| **Editar mapa del campus** | ❌ (DOM eliminado) | ❌ | ✅ |
| Sacar turno | ✅ | ✅ | ✅ |
| Ver **sus propios** turnos | ✅ | — | — |
| Ver **todos** los turnos | ❌ | ✅ | ✅ |
| Atender turno siguiente | ❌ | ✅ | ✅ |
| Ver **sus propios** trámites | ✅ | — | — |
| Ver **todos** los trámites | ❌ | ✅ | ✅ |
| Ver historial global | ❌ | ✅ | ✅ |
| Ver dependencias (lectura) | ✅ | ✅ | ✅ |
| Gestionar dependencias | ❌ | ❌ | ✅ |
| Ver reportes | ❌ | ❌ | ✅ |
| Gestionar usuarios | ❌ | ❌ | ✅ |
| Editar carrera/facultad de estudiante | ❌ | ❌ | ✅ |

### Auto-registro de Estudiantes

Cualquier persona puede crear su cuenta desde la pantalla de login sin necesidad de que un administrador la cree:

1. Clic en el tab **"Registrarse"** en la pantalla de inicio
2. Completa: nombre, apellido, cédula, email, **carrera** (lista completa UTA), contraseña
3. El sistema crea el usuario con rol `estudiante` y genera su perfil académico
4. Acceso inmediato: puede iniciar sesión de forma instantánea
5. El Admin puede corregir la carrera/facultad desde el panel de administración si el estudiante se equivocó

**Endpoint:** `POST /api/auth/register-student` (público, sin JWT)

---

## Cómo Funciona Internamente

### Un flujo completo de ejemplo: Calcular una ruta

```
1. NAVEGADOR
   └─ El estudiante selecciona "Rectorado" → "FISI" y hace clic en "Calcular"
   └─ El JS envía: POST /api/campus/ruta  { origen_id: 1, destino_id: 5 }
      con el token JWT en el header Authorization

2. NGINX (proxy)
   └─ Redirige la petición al puerto 8080 del backend C++

3. BACKEND C++ (Crow)
   └─ Verifica el token JWT → extrae el usuario y su rol
   └─ Llama a CampusService::calcularRuta(1, 5)
   └─ CampusService tiene el grafo cargado en memoria RAM
   └─ Ejecuta Grafo::dijkstra(1, 5)
   └─ Retorna: [Rectorado, Cruce A, Biblioteca, Cruce B, FISI]
   └─ Calcula la distancia total: 250 metros
   └─ Devuelve el JSON con la ruta y los coordinadas GPS de cada punto

4. NAVEGADOR
   └─ Recibe el JSON con la ruta
   └─ Leaflet.js dibuja una línea verde sobre el mapa siguiendo las coordenadas
   └─ El panel muestra "Ruta óptima: 250 m"
```

### Un flujo completo de ejemplo: Agregar un edificio (Admin)

```
1. NAVEGADOR (Admin)
   └─ Admin hace clic en "📍 Agregar punto" y luego en el mapa
   └─ Un pin naranja aparece en el mapa con un formulario flotante
   └─ Admin escribe "Rectorado", tipo "Edificio", marca "Aparece en Dijkstra"
   └─ Clic en "Guardar"
   └─ El JS envía: POST /api/campus/puntos  { nombre: "Rectorado", x: -78.627, y: -1.265, ... }

2. BACKEND C++
   └─ Verifica que el rol sea "admin" (si no, rechaza con error 403)
   └─ CampusRepo::crearPunto() inserta el punto en PostgreSQL
   └─ Llama a CampusService::recargar() para actualizar el grafo en RAM
   └─ Devuelve { ok: true, id: 7 }

3. NAVEGADOR
   └─ El pin naranja temporal se reemplaza por un marcador azul permanente
   └─ El edificio ahora aparece en los dropdowns de Dijkstra
```

---

## Despliegue con Docker

El sistema completo se levanta con un solo comando:

```bash
docker-compose up -d
```

Esto inicia tres contenedores:

```yaml
# docker-compose.yml (simplificado)

db:        # PostgreSQL 16
  image: postgres:16-alpine
  volumen: ./db/init.sql  # Crea tablas e inserta datos iniciales + organigrama UTA

backend:   # C++ compilado en imagen Alpine
  build: ./backend
  depends_on: db
  port: 8080 (interno)

frontend:  # Nginx con HTTPS self-signed
  build: ./frontend
  depends_on: backend
  ports:
    - 8090 → 80  (redirige automáticamente a HTTPS)
    - 8443 → 443 (HTTPS principal)
```

### HTTPS y acceso en red local

El sistema usa **certificados SSL auto-firmados** generados en tiempo de build. Nginx redirige automáticamente todo el tráfico HTTP a HTTPS:

| URL | Descripción |
|---|---|
| `https://localhost:8443` | Acceso local (tu máquina) |
| `http://localhost:8090` | HTTP → redirige a HTTPS |
| `https://192.168.2.15:8443` | **Acceso desde red WiFi local** |

> El navegador mostrará "Tu conexión no es privada" la primera vez (es normal con cert self-signed). Clic en **Avanzado → Continuar**.

Nginx usa `server_name _` (comodín) para aceptar conexiones desde **cualquier IP**, lo que permite que todos los dispositivos en el mismo WiFi del salón accedan al sistema sin configuración adicional.

### Proceso de compilación del backend

Al hacer `docker-compose build backend`:
1. Docker descarga la imagen base Alpine Linux (muy pequeña, ~5MB)
2. Instala las dependencias: `libpqxx`, `openssl`, `nlohmann-json`
3. Compila el código C++ con `cmake` y `make -j$(nproc)` (paralelo)
4. El ejecutable final (`smartcampus`) pesa ~3MB

---

## Guía Rápida de Uso

### Acceso al sistema
| URL | Descripción |
|---|---|
| `https://localhost:8443` | Acceso local |
| `https://192.168.2.15:8443` | Acceso desde WiFi del salón |

### Cuentas de demostración
| Rol | Email | Contraseña |
|---|---|---|
| Administrador | `admin@uta.edu.ec` | `admin123` |
| Secretaria | `secretaria@uta.edu.ec` | `admin123` |
| Estudiante | `juan@uta.edu.ec` | `admin123` |

### Registro de nuevo estudiante
1. Ir a la pantalla de login → tab **"Registrarse"**
2. Completar todos los campos (nombre, cédula, carrera de la lista UTA, contraseña)
3. El sistema crea la cuenta instantáneamente con rol `estudiante`

### Como Administrador

**Mapa del campus:**
1. Ir a **Mapa Campus** → tab **⚙️ Editar Campus**
2. Clic **"📍 Agregar punto"** → clic en mapa → llenar formulario → Guardar
3. Clic **"🔗 Conectar"** → clic Nodo A → clic Nodo B para crear sendero
4. Para eliminar: clic en marcador/línea → botón 🗑

**Gestionar estudiantes:**
- Ir a **Administración** → lista de estudiantes → botón ✏️ para editar carrera/facultad

**Dependencias:**
- Ir a **Dependencias** → ver el organigrama completo de la UTA
- Usar la búsqueda para encontrar facultades y oficinas
- Botón **"+ Dependencia"** para agregar nodos al árbol

### Como Estudiante

Al iniciar sesión, el estudiante es redirigido a **Mi Portal** (no al dashboard general):

1. **Mi Portal:** Turno activo del día + últimos trámites propios + estado de la cola
2. **Turnos:** Sacar turno para atención en ventanilla, ver historial propio
3. **Mis Trámites:** Ver únicamente sus propias solicitudes (no las de otros)
4. **Documentos:** Acceder al repositorio de documentos
5. **Dependencias:** Ver el organigrama para saber a qué oficina dirigirse
6. **Mapa Campus:** Calcular rutas entre edificios

### Como Secretaria

1. **Dashboard:** Ver estadísticas generales del sistema
2. **Turnos:** Atender el siguiente en la cola con el botón "✅ Atender Siguiente"
3. **Trámites:** Ver y gestionar todos los trámites, cambiar estados

---

## Resumen Técnico Final

| Aspecto | Decisión | Razón |
|---|---|---|
| Lenguaje backend | C++17 | Implementación real de estructuras de datos académicas |
| Framework backend | Crow | Minimalista, headers-only, ideal para C++ |
| Base de datos | PostgreSQL 16 | Concurrencia real, ACID, robusto |
| Mapas | Leaflet + OSM | Gratuito, open-source, sin API key |
| Ruta más corta | Dijkstra propio | Algoritmo implementado en C++ para el proyecto |
| Autenticación | JWT + HS256 | Estándar de la industria, sin estado en el servidor |
| Seguridad transporte | HTTPS + SSL self-signed | Cifrado en tránsito, acceso local y LAN |
| Despliegue | Docker Compose | Reproducible en cualquier máquina |
| Frontend | HTML/CSS/JS puro | Sin frameworks pesados, accesible desde cualquier PC |
| Control de acceso | RBAC + filtrado BD | Estudiante solo ve sus propios datos |
| Registro | Auto-registro público | Estudiantes crean cuenta sin intervención del admin |
| Organigrama | Árbol N-ario — 78 nodos | Estructura oficial completa de la UTA |

---

*Documentación generada para el proyecto SmartCampus UTA.*  
*Universidad Técnica de Ambato — Facultad de Ingeniería en Sistemas.*  
*Última actualización: Junio 2026*
