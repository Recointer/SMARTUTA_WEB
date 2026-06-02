-- ============================================================
-- ORGANIGRAMA REAL UTA - SmartCampus
-- Limpia datos demo e inserta estructura oficial
-- ============================================================

TRUNCATE TABLE dependencias RESTART IDENTITY CASCADE;

-- ── NIVEL 0: Raíz ────────────────────────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Universidad Técnica de Ambato', 'UTA — Alma Mater de todos los ambateños', NULL, 0);
-- id = 1

-- ── NIVEL 1: Órganos de Gobierno y Dirección ─────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Honorable Consejo Universitario',          'Máximo órgano de gobierno de la UTA',                    1, 1), -- 2
('Tribunal Electoral Universitario',          'Órgano electoral de la universidad',                     1, 1), -- 3
('Consejo Electoral Universitario Estudiantil','Representación electoral estudiantil',                  1, 1), -- 4
('Rectorado',                                 'Máxima autoridad ejecutiva de la UTA',                  1, 1), -- 5
('Vicerrectorado Académico',                  'Gestión académica y curricular',                        1, 1), -- 6
('Vicerrectorado Administrativo',             'Gestión administrativa y financiera',                   1, 1), -- 7
('Vicerrectorado de Investigación e Innovación y Vinculación', 'I+D+i y vinculación con la sociedad', 1, 1), -- 8

-- Facultades Campus Huachi
('Facultad de Ciencia e Ingeniería en Alimentos y Biotecnología', 'FIAB — Campus Huachi',             1, 1), -- 9
('Facultad de Jurisprudencia y Ciencias Sociales',               'FJCS — Campus Huachi',              1, 1), -- 10
('Facultad de Ingeniería en Sistemas, Electrónica e Industrial', 'FISEI — Campus Huachi',             1, 1), -- 11
('Facultad de Ciencias Humanas y de la Educación',               'FCHE — Campus Huachi',              1, 1), -- 12
('Facultad de Diseño y Arquitectura',                            'FDA — Campus Huachi',               1, 1), -- 13
('Facultad de Ingeniería Civil y Mecánica',                      'FICM — Campus Huachi',              1, 1), -- 14
('Facultad de Ciencias Administrativas',                         'FCA — Campus Huachi',               1, 1), -- 15
('Facultad de Contabilidad y Auditoría',                         'FCAUD — Campus Huachi',             1, 1), -- 16

-- Facultad Campus Ingahurco
('Facultad de Ciencias de la Salud',         'FCS — Campus Ingahurco',                                1, 1), -- 17

-- Facultad Campus Querochaca
('Facultad de Ciencias Agropecuarias',       'FCAG — Campus Querochaca',                             1, 1), -- 18

-- Centros universitarios
('Centros y Direcciones Universitarias',     'Servicios de apoyo académico y estudiantil',            1, 1); -- 19

-- ── NIVEL 2: Dependencias del Rectorado (padre=5) ─────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Secretaría General',                        'Gestión documental y actas oficiales',           5, 2), -- 20
('Procuraduría',                              'Asesoría jurídica institucional',               5, 2), -- 21
('Dirección de Planificación y Evaluación',   'POA, planificación estratégica',                5, 2), -- 22
('Dirección de Cultura',                      'Actividades culturales universitarias',         5, 2), -- 23
('Dirección de Comunicación y RRPP',          'Comunicación institucional y medios',          5, 2), -- 24
('Dirección de Relaciones Nacionales e Internacionales', 'Cooperación y convenios',           5, 2), -- 25
('Dirección Financiera',                      'Presupuesto y finanzas institucionales',        5, 2), -- 26
('Dirección de Auditoría Interna',            'Control interno y cumplimiento',               5, 2), -- 27
('Dirección de Gestión de la Calidad',        'Acreditación y aseguramiento de calidad',      5, 2), -- 28
('Dirección de Gestión de Riesgos',           'Gestión de riesgos institucionales',           5, 2), -- 29
('Coordinación General del Rectorado',        'Coordinación ejecutiva del Rectorado',         5, 2); -- 30

-- ── NIVEL 2: Dependencias del Vicerrectorado Académico (padre=6) ──
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Consejo Académico Universitario',           'Órgano asesor académico universitario',        6, 2), -- 31
('Dirección Académica',                       'Coordinación curricular y docente',            6, 2), -- 32
('Consejo de Posgrados',                      'Gestión de programas de posgrado',             6, 2), -- 33
('Dirección de Educación Continua, a Distancia y Virtual', 'Modalidades no presenciales',    6, 2), -- 34
('Dirección de Investigación y Desarrollo',   'Proyectos de investigación institucional',     6, 2), -- 35
('Dirección de Innovación y Emprendimiento',  'Ecosistema de innovación y startups',          6, 2); -- 36

-- ── NIVEL 2: Dependencias del Vicerrectorado Administrativo (padre=7) ──
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Dirección de Talento Humano',               'Gestión del personal universitario',           7, 2), -- 37
('Dirección de Infraestructura Física',       'Obras, mantenimiento y campus',                7, 2), -- 38
('Dirección Administrativa',                  'Logística, compras y servicios',               7, 2); -- 39

-- ── NIVEL 2: Dependencias Vicerrectorado Investigación (padre=8) ──
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Consejo de Investigación e Innovación',     'Órgano rector de I+D+i',                      8, 2), -- 40
('Consejo de Vinculación con la Sociedad',    'Prácticas preprofesionales y servicio',        8, 2), -- 41
('Comité Editorial',                          'Publicaciones y revistas científicas',         8, 2), -- 42
('Centros de Investigación e Innovación',     'Laboratorios y grupos de investigación',       8, 2); -- 43

-- ── NIVEL 2: Carreras FIAB (padre=9) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Ingeniería en Alimentos',  'Tecnología y procesamiento de alimentos',                      9, 2), -- 44
('Biotecnología',            'Biotecnología aplicada y biociencias',                         9, 2); -- 45

-- ── NIVEL 2: Carreras FJCS (padre=10) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Derecho',                  'Ciencias jurídicas y legislación',                            10, 2), -- 46
('Comunicación Social',      'Periodismo, medios y comunicación',                           10, 2); -- 47

-- ── NIVEL 2: Carreras FISEI (padre=11) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Ingeniería en Sistemas',   'Desarrollo de software y sistemas informáticos',             11, 2), -- 48
('Ingeniería Electrónica',   'Electrónica, automatización y control',                      11, 2), -- 49
('Ingeniería Industrial',    'Procesos industriales y productividad',                       11, 2); -- 50

-- ── NIVEL 2: Carreras FCHE (padre=12) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Pedagogía',                'Formación docente y ciencias de la educación',               12, 2), -- 51
('Psicología Educativa',     'Psicología aplicada al entorno educativo',                   12, 2); -- 52

-- ── NIVEL 2: Carreras FDA (padre=13) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Arquitectura',             'Diseño y construcción de espacios habitables',               13, 2), -- 53
('Diseño Gráfico',           'Comunicación visual y diseño digital',                       13, 2), -- 54
('Diseño Industrial',        'Productos industriales y ergonomía',                         13, 2), -- 55
('Diseño Textil e Indumentaria', 'Moda, textiles y confección',                            13, 2); -- 56

-- ── NIVEL 2: Carreras FICM (padre=14) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Ingeniería Civil',         'Obras civiles, vialidad e infraestructura',                  14, 2), -- 57
('Ingeniería Mecánica',      'Mecánica, termodinámica y manufactura',                      14, 2); -- 58

-- ── NIVEL 2: Carreras FCA (padre=15) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Administración de Empresas','Gestión empresarial y organizacional',                       15, 2), -- 59
('Mercadotecnia',             'Marketing, ventas y comportamiento del consumidor',          15, 2); -- 60

-- ── NIVEL 2: Carreras FCAUD (padre=16) ─────────────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Contabilidad y Auditoría', 'Contabilidad, tributación y control financiero',             16, 2); -- 61

-- ── NIVEL 2: Carreras FCS — Campus Ingahurco (padre=17) ───────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Medicina',                 'Formación médica y ciencias de la salud',                    17, 2), -- 62
('Enfermería',               'Cuidado integral del paciente',                              17, 2), -- 63
('Odontología',              'Salud bucal y estomatología',                                17, 2), -- 64
('Terapia Física',           'Rehabilitación y fisioterapia',                              17, 2); -- 65

-- ── NIVEL 2: Carreras FCAG — Campus Querochaca (padre=18) ─────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Agronomía',                'Producción agrícola y sostenibilidad',                       18, 2), -- 66
('Medicina Veterinaria',     'Salud y producción animal',                                 18, 2); -- 67

-- ── NIVEL 2: Centros universitarios (padre=19) ────────────────
INSERT INTO dependencias (nombre, descripcion, padre_id, nivel) VALUES
('Centro de Posgrados',               'Maestrías y doctorados',                           19, 2), -- 68
('Centro de Idiomas',                 'Inglés, francés, alemán y más',                    19, 2), -- 69
('Centro de Ciencias Básicas',        'Nivelación en matemáticas, física, química',      19, 2), -- 70
('Centro de Cultura Física',          'Deportes, educación física y bienestar',          19, 2), -- 71
('Coordinación General de Bibliotecas','Red de bibliotecas universitarias',              19, 2), -- 72
('Dirección de Bienestar Universitario','Servicios estudiantiles, becas y apoyo',        19, 2), -- 73
('DTIC',                              'Tecnologías de Información y Comunicación',       19, 2), -- 74
('Centro de Educación Continua',      'Cursos de actualización y capacitación',          19, 2), -- 75
('Programas a Distancia y Virtual',   'Educación en línea y semipresencial',             19, 2), -- 76
('Idiomas Modalidad Abierta',         'Cursos libres de idiomas',                        19, 2), -- 77
('Centro Cultural Universitario',     'Artes, cultura y talleres creativos',             19, 2); -- 78

-- Actualizar también la tabla facultades con IDs reales para que el registro funcione
INSERT INTO facultades (nombre) VALUES
('FISEI — Ingeniería en Sistemas, Electrónica e Industrial'),
('FCA — Ciencias Administrativas'),
('FJCS — Jurisprudencia y Ciencias Sociales'),
('FCHE — Ciencias Humanas y de la Educación'),
('FICM — Ingeniería Civil y Mecánica'),
('FDA — Diseño y Arquitectura'),
('FIAB — Ciencia e Ingeniería en Alimentos'),
('FCAUD — Contabilidad y Auditoría'),
('FCS — Ciencias de la Salud'),
('FCAG — Ciencias Agropecuarias')
ON CONFLICT DO NOTHING;
