# 🎓 SmartCampus UTA — Guía de Defensa Grupal

> **Defensa mañana · 6 integrantes · Cada quien defiende su parte**

---

## 👤 PERSONA 1 — Arquitectura General + Docker

**Tu tema:** Cómo está organizado el sistema por dentro y cómo se levanta.

**Qué decir:**
- Sistema tiene **3 capas en contenedores Docker**: Base de datos (PostgreSQL), Servidor (C++), Interfaz web (Nginx)
- Un solo comando lo levanta todo: `docker-compose up -d`
- **Por qué Docker:** garantiza que funciona igual en cualquier computadora, sin instalar dependencias manualmente
- El frontend actúa como **proxy inverso**: el usuario nunca habla directamente con el backend, todo pasa por Nginx

**Diagrama a mostrar:**
```
Navegador → Nginx (puerto 8090) → Backend C++ (8080) → PostgreSQL (5432)
```

**Pregunta trampa probable:** *¿Por qué no pusieron todo en un solo servidor?*
> Separar en capas permite escalar cada parte independientemente. Si hay muchos usuarios, solo se replica el backend.

---

## 👤 PERSONA 2 — Backend C++ + API REST

**Tu tema:** El servidor que procesa todas las peticiones.

**Qué decir:**
- Backend escrito en **C++17** usando el framework **Crow** (similar a Express de Node.js pero en C++)
- Expone una **API REST** con más de 25 endpoints: `/api/auth/login`, `/api/campus/ruta`, `/api/turnos`, etc.
- **Por qué C++:** permite implementar estructuras de datos reales (no las de Python que ya vienen hechas)
- Cada petición pasa por validación JWT antes de ejecutarse

**Ejemplo real a mostrar:**
```
POST /api/campus/ruta
{ "origen_id": 1, "destino_id": 5 }
→ Backend corre Dijkstra en C++
→ Retorna: { ruta: [...], distancia_total: 250 }
```

**Pregunta trampa probable:** *¿Por qué no usaron Python/Node que es más fácil?*
> Requisito académico: implementar estructuras de datos propias. En C++ hay que construirlas desde cero, demostrando que se entiende cómo funcionan.

---

## 👤 PERSONA 3 — Estructuras de Datos (Grafo + Dijkstra)

**Tu tema:** El corazón académico del proyecto — los algoritmos implementados en C++.

**Qué decir:**
- **Grafo con Lista de Adyacencia** (`Grafo.h`): representa el campus. Cada nodo = edificio con coordenadas GPS. Cada arista = sendero peatonal con distancia en metros
- **Algoritmo Dijkstra** implementado desde cero: encuentra la ruta más corta entre dos edificios
- La distancia entre dos puntos GPS se calcula con la **fórmula Haversine** (considera curvatura de la Tierra)
- **Por qué Lista de Adyacencia y no Matriz:** el campus tiene pocos caminos entre muchos nodos posibles → la lista usa menos memoria

**Otras estructuras usadas:**
| Estructura | Archivo | Uso |
|---|---|---|
| Cola FIFO | `Cola.h` | Turnos de atención |
| Pila LIFO | `Pila.h` | Historial / deshacer |
| Árbol N-ario | `Arbol.h` | Documentos y dependencias |
| Lista Secuencial | `ListaSecuencial.h` | Catálogo de trámites |

**Pregunta trampa probable:** *¿Dijkstra siempre da la ruta óptima?*
> Sí, siempre que el grafo esté bien definido con aristas de peso no negativo. En nuestro caso los pesos son distancias en metros → siempre positivos → Dijkstra es óptimo.

---

## 👤 PERSONA 4 — Base de Datos + Seguridad

**Tu tema:** Cómo se almacena la información y cómo se protege.

**Qué decir:**
- **PostgreSQL 15**: base de datos relacional. Elegida sobre SQLite porque soporta múltiples usuarios simultáneos sin bloquearse
- Tablas principales: `usuarios`, `turnos`, `tramites`, `puntos_campus`, `conexiones_campus`, `historial_acciones`
- **Autenticación JWT**: al hacer login el servidor devuelve un token firmado. Cada petición lleva ese token en el header. El servidor lo verifica sin consultar la BD → rápido
- **Contraseñas hasheadas**: nunca se guarda la contraseña en texto plano, solo el hash BCrypt
- **Control de roles**: 3 niveles → Admin (todo), Secretaria (turnos/trámites), Estudiante (consulta)

**Pregunta trampa probable:** *¿Qué pasa si alguien roba el token JWT?*
> Los tokens expiran. Además, el servidor usa HTTPS (SSL) en producción para que nadie intercepte el token en tránsito.

---

## 👤 PERSONA 5 — Frontend + Mapa Interactivo

**Tu tema:** La interfaz que ve el usuario y el mapa del campus.

**Qué decir:**
- Frontend en **HTML + CSS + JavaScript puro** (sin React/Angular) → funciona en cualquier computadora, incluso lentas
- Mapa con **Leaflet.js + OpenStreetMap**: mapa real de Ambato, gratuito, sin API Key
- Admin puede **agregar edificios haciendo clic** directamente en el mapa → formulario flotante aparece junto al pin
- Admin puede **conectar edificios** haciendo clic en nodo A → nodo B → línea peatonal creada con distancia automática
- Al calcular ruta: frontend envía origen+destino al backend, backend corre Dijkstra, frontend dibuja la ruta en verde sobre el mapa real
- **Eliminar**: clic en cualquier marcador o línea → popup con botón 🗑 Eliminar

**Demo recomendada:**
1. Mostrar mapa vacío
2. Agregar 2 edificios clicando en el mapa
3. Conectarlos
4. Calcular ruta → línea verde aparece

**Pregunta trampa probable:** *¿Por qué no usaron Google Maps?*
> Google Maps requiere API Key de pago. OpenStreetMap es gratuito y open-source. Leaflet.js permite integrar nuestra lógica Dijkstra propia.

---

## 👤 PERSONA 6 — Módulos del Sistema + Conclusiones

**Tu tema:** Las funcionalidades administrativas y el resumen del proyecto.

**Qué decir:**

**Módulos implementados:**
- 🎫 **Turnos**: cola digital FIFO. Estudiante saca turno → secretaria atiende en orden → sin filas físicas
- 📋 **Trámites**: el estudiante inicia una solicitud (certificado, récord académico) y puede ver su estado en línea
- 📜 **Historial**: pila LIFO que registra todas las acciones. Tiene función "deshacer" (undo)
- 📁 **Documentos**: árbol N-ario de categorías. Admin sube documentos, estudiantes los descargan
- 🏛️ **Dependencias**: árbol jerárquico de facultades, carreras y departamentos de la UTA

**Conclusiones para defender:**
> "SmartCampus UTA digitaliza los procesos administrativos de la UTA integrando 5 módulos funcionales con estructuras de datos implementadas en C++ y un sistema de navegación georreferenciada usando coordenadas GPS reales del campus. El resultado es un sistema full-stack completo, desplegado en Docker, con mapa interactivo, algoritmo Dijkstra propio y autenticación JWT."

**Pregunta trampa probable:** *¿Qué mejorarían si tuvieran más tiempo?*
> Integrar notificaciones en tiempo real (WebSockets) cuando el turno está próximo, y usar OSRM para que las rutas sigan automáticamente los senderos de OpenStreetMap sin que el admin los dibuje manualmente.

---

## 📌 Datos Técnicos Clave (todos deben saber)

| Dato | Valor |
|---|---|
| Lenguaje backend | C++17 |
| Framework backend | Crow |
| Base de datos | PostgreSQL 15 |
| Mapa | Leaflet.js + OpenStreetMap |
| Algoritmo de rutas | Dijkstra propio en C++ |
| Autenticación | JWT (HS256) |
| Despliegue | Docker + Docker Compose |
| URL local | http://localhost:8090 |
| Admin demo | admin@uta.edu.ec / admin123 |

---

*SmartCampus UTA · Universidad Técnica de Ambato · Defensa Académica*
