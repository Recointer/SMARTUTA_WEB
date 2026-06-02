-- ============================================================
-- SmartCampus UTA Web — Esquema de Base de Datos
-- PostgreSQL 16
-- ============================================================

-- ACCESO
CREATE TABLE roles (
    id   SERIAL PRIMARY KEY,
    nombre      VARCHAR(50)  NOT NULL UNIQUE,
    descripcion TEXT
);

CREATE TABLE usuarios (
    id            SERIAL PRIMARY KEY,
    nombre        VARCHAR(100) NOT NULL,
    email         VARCHAR(150) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    rol_id        INTEGER REFERENCES roles(id),
    activo        BOOLEAN DEFAULT TRUE,
    created_at    TIMESTAMP DEFAULT NOW()
);

-- PERSONAS
CREATE TABLE facultades (
    id     SERIAL PRIMARY KEY,
    nombre VARCHAR(150) NOT NULL,
    codigo VARCHAR(20) UNIQUE
);

CREATE TABLE estudiantes (
    id          SERIAL PRIMARY KEY,
    cedula      VARCHAR(20) UNIQUE NOT NULL,
    nombre      VARCHAR(100) NOT NULL,
    apellido    VARCHAR(100) NOT NULL,
    carrera     VARCHAR(150),
    facultad_id INTEGER REFERENCES facultades(id),
    email       VARCHAR(150),
    usuario_id  INTEGER REFERENCES usuarios(id)
);

CREATE TABLE empleados (
    id         SERIAL PRIMARY KEY,
    cedula     VARCHAR(20) UNIQUE NOT NULL,
    nombre     VARCHAR(100) NOT NULL,
    apellido   VARCHAR(100) NOT NULL,
    cargo      VARCHAR(100),
    email      VARCHAR(150),
    usuario_id INTEGER REFERENCES usuarios(id)
);

-- VENTANILLAS (Lista Circular)
CREATE TABLE ventanillas (
    id        SERIAL PRIMARY KEY,
    nombre    VARCHAR(100) NOT NULL,
    activo    BOOLEAN DEFAULT TRUE
);

-- TURNOS (Cola FIFO)
CREATE TABLE turnos (
    id           SERIAL PRIMARY KEY,
    numero_turno INTEGER NOT NULL,
    usuario_id   INTEGER REFERENCES usuarios(id),
    tipo_tramite VARCHAR(100),
    estado       VARCHAR(50) DEFAULT 'pendiente',
    ventanilla_id INTEGER REFERENCES ventanillas(id),
    created_at   TIMESTAMP DEFAULT NOW(),
    atendido_at  TIMESTAMP
);

-- TIPOS DE TRÁMITE (Lista Secuencial)
CREATE TABLE tipos_tramite (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(100) NOT NULL,
    descripcion TEXT,
    activo      BOOLEAN DEFAULT TRUE
);

-- TRÁMITES (Lista Simple + Lista Doble)
CREATE TABLE tramites (
    id              SERIAL PRIMARY KEY,
    estudiante_id   INTEGER REFERENCES estudiantes(id),
    tipo_tramite_id INTEGER REFERENCES tipos_tramite(id),
    descripcion     TEXT,
    estado          VARCHAR(50) DEFAULT 'pendiente',
    created_at      TIMESTAMP DEFAULT NOW(),
    updated_at      TIMESTAMP DEFAULT NOW()
);

CREATE TABLE solicitudes (
    id         SERIAL PRIMARY KEY,
    tramite_id INTEGER REFERENCES tramites(id),
    descripcion TEXT,
    estado      VARCHAR(50) DEFAULT 'nueva',
    created_at  TIMESTAMP DEFAULT NOW()
);

-- DEPENDENCIAS INSTITUCIONALES (Árbol N-ario)
CREATE TABLE dependencias (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(150) NOT NULL,
    descripcion TEXT,
    padre_id    INTEGER REFERENCES dependencias(id),
    nivel       INTEGER DEFAULT 0
);

-- DOCUMENTOS (Árbol N-ario)
CREATE TABLE categorias_documento (
    id       SERIAL PRIMARY KEY,
    nombre   VARCHAR(150) NOT NULL,
    padre_id INTEGER REFERENCES categorias_documento(id)
);

CREATE TABLE documentos (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(200) NOT NULL,
    descripcion TEXT,
    categoria_id INTEGER REFERENCES categorias_documento(id),
    archivo_url VARCHAR(500),
    subido_por  INTEGER REFERENCES usuarios(id),
    created_at  TIMESTAMP DEFAULT NOW()
);

-- CAMPUS — GRAFO (Lista de Adyacencia + Dijkstra)
-- x = longitud (lng), y = latitud (lat) — coordenadas GPS WGS84
CREATE TABLE puntos_campus (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(150) NOT NULL,
    tipo        VARCHAR(50) DEFAULT 'waypoint',
    x           DOUBLE PRECISION NOT NULL,  -- longitud
    y           DOUBLE PRECISION NOT NULL,  -- latitud
    es_edificio BOOLEAN DEFAULT FALSE,
    descripcion TEXT
);

CREATE TABLE conexiones_campus (
    id         SERIAL PRIMARY KEY,
    origen_id  INTEGER REFERENCES puntos_campus(id),
    destino_id INTEGER REFERENCES puntos_campus(id),
    distancia  FLOAT NOT NULL,
    tipo_camino VARCHAR(50) DEFAULT 'peatonal'
);

-- HISTORIAL (Pila LIFO)
CREATE TABLE historial_acciones (
    id          SERIAL PRIMARY KEY,
    usuario_id  INTEGER REFERENCES usuarios(id),
    accion      VARCHAR(100) NOT NULL,
    descripcion TEXT,
    datos_json  TEXT,
    timestamp   TIMESTAMP DEFAULT NOW()
);

-- ============================================================
-- DATOS SEMILLA
-- ============================================================

INSERT INTO roles (nombre, descripcion) VALUES
    ('admin', 'Administrador del sistema'),
    ('secretaria', 'Personal de secretaría y atención'),
    ('estudiante', 'Estudiante universitario');

-- Contraseña: "admin123" → SHA256 con salt "SMART"
-- hash = sha256("SMARTadmin123") — se valida en el backend
INSERT INTO usuarios (nombre, email, password_hash, rol_id) VALUES
    -- Contraseña: "admin123" → hash = SHA256("SMARTUTA_admin123")
    ('Administrador UTA', 'admin@uta.edu.ec',   '7bc2bcf4b59479af264acbe946fbb52662b246f09d5a687f4a63d6e24693eafd', 1),
    ('María Secretaría',  'secretaria@uta.edu.ec','7bc2bcf4b59479af264acbe946fbb52662b246f09d5a687f4a63d6e24693eafd', 2),
    ('Juan Estudiante',   'juan@uta.edu.ec',     '7bc2bcf4b59479af264acbe946fbb52662b246f09d5a687f4a63d6e24693eafd', 3);

INSERT INTO facultades (nombre, codigo) VALUES
    ('Facultad de Ingeniería en Sistemas', 'FISI'),
    ('Facultad de Ciencias Administrativas', 'FCA'),
    ('Facultad de Ingeniería Civil', 'FIC'),
    ('Facultad de Ciencias de la Salud', 'FCS');

INSERT INTO estudiantes (cedula, nombre, apellido, carrera, facultad_id, email, usuario_id) VALUES
    ('1804001001', 'Juan', 'Pérez', 'Ingeniería de Software', 1, 'juan@uta.edu.ec', 3),
    ('1804001002', 'Ana', 'Torres', 'Ingeniería de Software', 1, 'ana@uta.edu.ec', NULL),
    ('1804001003', 'Carlos', 'Ruiz', 'Administración de Empresas', 2, 'carlos@uta.edu.ec', NULL);

INSERT INTO ventanillas (nombre) VALUES
    ('Ventanilla 1'), ('Ventanilla 2'), ('Ventanilla 3');

INSERT INTO tipos_tramite (nombre, descripcion) VALUES
    ('Certificado de matrícula', 'Certificado que acredita matrícula vigente'),
    ('Récord académico', 'Historial de notas del estudiante'),
    ('Solicitud de beca', 'Trámite para acceder a becas universitarias'),
    ('Cambio de carrera', 'Solicitud de transferencia de carrera'),
    ('Homologación de materias', 'Reconocimiento de materias cursadas'),
    ('Título universitario', 'Solicitud para obtención de título');

-- Dependencias institucionales (Árbol N-ario)
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
    ('Universidad Técnica de Ambato', 'Institución de educación superior', NULL, 0);
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
    ('Rectorado', 'Máxima autoridad ejecutiva', 1, 1),
    ('Vicerrectorado Académico', 'Gestión académica institucional', 1, 1),
    ('Vicerrectorado Administrativo', 'Gestión administrativa', 1, 1);
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
    ('FISI', 'Facultad de Ingeniería en Sistemas', 3, 2),
    ('FCA', 'Facultad de Ciencias Administrativas', 3, 2),
    ('FIC', 'Facultad de Ingeniería Civil', 3, 2),
    ('DTIC', 'Dirección de Tecnologías', 4, 2);
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
    ('Ingeniería de Software', 'Carrera de Software', 5, 3),
    ('Ingeniería en Sistemas', 'Carrera de Sistemas', 5, 3);

-- Categorías de documento (Árbol N-ario)
INSERT INTO categorias_documento (nombre, padre_id) VALUES ('Documentos Académicos', NULL);
INSERT INTO categorias_documento (nombre, padre_id) VALUES ('Documentos Administrativos', NULL);
INSERT INTO categorias_documento (nombre, padre_id) VALUES ('Certificados', 1);
INSERT INTO categorias_documento (nombre, padre_id) VALUES ('Reglamentos', 2);
INSERT INTO categorias_documento (nombre, padre_id) VALUES ('Syllabus', 1);
INSERT INTO categorias_documento (nombre, padre_id) VALUES ('Formularios', 2);

INSERT INTO documentos (nombre, descripcion, categoria_id, subido_por) VALUES
    ('Reglamento Académico 2024', 'Reglamento de régimen académico vigente', 4, 1),
    ('Formulario de Beca', 'Formulario oficial para solicitud de beca', 6, 1),
    ('Certificado Modelo', 'Modelo de certificado de matrícula', 3, 1);

-- ============================================================
-- Campus UTA Huachi — Coordenadas GPS reales (WGS84)
-- x = longitud (lng), y = latitud (lat)
-- Campus: Universidad Técnica de Ambato, Huachi Chico, Ambato, Ecuador
-- Centro aprox: -1.2422, -78.6194
-- ============================================================
INSERT INTO puntos_campus (nombre, tipo, x, y, es_edificio, descripcion) VALUES
    -- EDIFICIOS PRINCIPALES
    ('Entrada Principal Av. Los Chasquis', 'entrada',    -78.6218, -1.2449, FALSE, 'Ingreso principal por Av. Los Chasquis'),
    ('Rectorado',           'edificio',  -78.6198, -1.2430, TRUE,  'Edificio de Rectorado y administración central'),
    ('Biblioteca General',  'edificio',  -78.6207, -1.2422, TRUE,  'Biblioteca universitaria central'),
    ('FISI — Ingeniería en Sistemas', 'edificio', -78.6183, -1.2418, TRUE, 'Facultad de Ingeniería en Sistemas e Informática'),
    ('FCA — Ciencias Administrativas', 'edificio', -78.6210, -1.2408, TRUE, 'Facultad de Ciencias Administrativas'),
    ('FISEI — Ingeniería Industrial', 'edificio', -78.6188, -1.2438, TRUE, 'Facultad de Ingeniería en Sistemas, Electrónica e Industrial'),
    ('Coliseo Universitario', 'edificio', -78.6196, -1.2443, TRUE, 'Coliseo cubierto universitario'),
    ('Cafetería Central',   'edificio',  -78.6202, -1.2435, TRUE,  'Cafetería y restaurante universitario'),
    ('DTIC',                'edificio',  -78.6186, -1.2426, TRUE,  'Dirección de Tecnologías de Información'),
    ('Canchas Deportivas',  'edificio',  -78.6174, -1.2432, TRUE,  'Instalaciones deportivas'),
    ('Bienestar Universitario', 'edificio', -78.6205, -1.2414, TRUE, 'Centro de bienestar y salud estudiantil'),
    ('Auditorio Central',   'edificio',  -78.6193, -1.2411, TRUE,  'Auditorio para eventos institucionales'),
    -- WAYPOINTS — INTERSECCIONES PEATONALES REALES (no cruzan edificios)
    ('WP — Av. Los Chasquis Norte',  'waypoint', -78.6207, -1.2445, FALSE, 'Avenida Los Chasquis, tramo norte'),
    ('WP — Caminera Rectorado-Bib',  'waypoint', -78.6203, -1.2426, FALSE, 'Sendero entre Rectorado y Biblioteca'),
    ('WP — Cruce Central',           'waypoint', -78.6198, -1.2422, FALSE, 'Cruce central del campus'),
    ('WP — Sendero Norte',           'waypoint', -78.6197, -1.2412, FALSE, 'Sendero norte hacia FCA y Auditorio'),
    ('WP — Caminera Este',           'waypoint', -78.6187, -1.2422, FALSE, 'Sendero este entre FISI y DTIC'),
    ('WP — Caminera Cafetería',      'waypoint', -78.6200, -1.2432, FALSE, 'Sendero hacia cafetería y coliseo'),
    ('WP — Av. Chasquis Sur',        'waypoint', -78.6207, -1.2438, FALSE, 'Avenida Los Chasquis, tramo sur'),
    ('WP — Cruce Deportivo',         'waypoint', -78.6183, -1.2430, FALSE, 'Cruce zona deportiva');

-- ============================================================
-- Conexiones peatonales — distancias en metros
-- IMPORTANTE: Solo caminos reales, NO cruzan el interior de edificios
-- ============================================================
INSERT INTO conexiones_campus (origen_id, destino_id, distancia, tipo_camino) VALUES
    -- Desde entrada principal
    (1,  13, 30,  'peatonal'),   -- Entrada → WP Av.Chasquis Norte
    (13,  2, 55,  'peatonal'),   -- WP Av.Chasquis Norte → Rectorado
    (13, 19, 40,  'peatonal'),   -- WP Av.Chasquis Norte → WP Chasquis Sur
    (19,  7, 35,  'peatonal'),   -- WP Chasquis Sur → Coliseo
    (19,  8, 30,  'peatonal'),   -- WP Chasquis Sur → Cafetería
    -- Rectorado - zona central
    (2,  14, 45,  'peatonal'),   -- Rectorado → WP Rectorado-Bib
    (2,  18, 40,  'peatonal'),   -- Rectorado → WP Cafetería
    (14,  3, 35,  'peatonal'),   -- WP Rectorado-Bib → Biblioteca
    (14, 15, 30,  'peatonal'),   -- WP Rectorado-Bib → Cruce Central
    -- Zona central
    (15,  9, 45,  'peatonal'),   -- Cruce Central → DTIC
    (15, 17, 35,  'peatonal'),   -- Cruce Central → Caminera Este
    (15, 16, 50,  'peatonal'),   -- Cruce Central → Sendero Norte
    (17,  4, 40,  'peatonal'),   -- Caminera Este → FISI
    (17,  9, 30,  'peatonal'),   -- Caminera Este → DTIC
    (17, 20, 40,  'peatonal'),   -- Caminera Este → Cruce Deportivo
    (20, 10, 35,  'peatonal'),   -- Cruce Deportivo → Canchas
    (20,  6, 45,  'peatonal'),   -- Cruce Deportivo → FISEI
    -- Zona norte (FCA, Auditorio, Bienestar)
    (16,  5, 55,  'peatonal'),   -- Sendero Norte → FCA
    (16, 11, 50,  'peatonal'),   -- Sendero Norte → Bienestar
    (16, 12, 40,  'peatonal'),   -- Sendero Norte → Auditorio
    (12,  4, 55,  'peatonal'),   -- Auditorio → FISI
    -- Cafetería - Coliseo
    (18,  8, 25,  'peatonal'),   -- WP Cafetería → Cafetería
    (8,   7, 60,  'peatonal'),   -- Cafetería → Coliseo
    -- Biblioteca zona
    (3,  16, 65,  'peatonal');   -- Biblioteca → Sendero Norte

