# Proyecto Integrador de la Asignatura Estructura de Datos
## SmartCampus UTA Web

* **Carrera:** Ingeniería de Software
* **Asignatura:** Estructura de Datos
* **Tipo de trabajo:** Proyecto integrador avanzado
* **Modalidad:** Aplicativo web con base de datos y arquitectura por capas
* **Contexto:** Propuesta alineada con la planificación académica de la asignatura

---

## 1. Introducción
En la formación de Ingeniería de Software, el estudio de estructuras de datos adquiere mayor valor cuando se aplica dentro de soluciones completas y no únicamente como ejercicios aislados de programación. Por ello, este proyecto integrador propone que los estudiantes diseñen y construyan un aplicativo web funcional, partiendo de requerimientos iniciales, diseño de base de datos, arquitectura de software por capas e integración de estructuras de datos lineales y no lineales.

La propuesta responde a la secuencia de contenidos de la asignatura, que aborda TDA, listas, pilas, colas, árboles, recursividad y grafos, y busca que cada equipo resuelva un problema realista de gestión universitaria con criterios técnicos, de calidad y de documentación profesional.

## 2. Justificación
El proyecto permite evaluar de forma articulada la capacidad del estudiante para analizar problemas, levantar requerimientos, modelar datos, diseñar arquitectura de software y seleccionar estructuras de datos apropiadas. Además, fortalece la visión de ingeniería al exigir que cada estructura implementada tenga un uso real dentro del sistema y no sea incorporada únicamente como demostración académica.

* Integra teoría y práctica en un solo producto de software.
* Promueve decisiones técnicas justificadas y documentadas.
* Refuerza competencias de desarrollo web, base de datos y modelado.
* Facilita una evaluación más completa del desempeño del estudiante.

## 3. Problema a Resolver
En el contexto universitario, procesos como la atención estudiantil, el registro de trámites, la organización documental y la localización de espacios suelen estar distribuidos en diferentes medios, lo que genera retrasos, poca trazabilidad y dificultad para visualizar relaciones entre actores, dependencias y recursos. Se requiere un sistema web que centralice estos procesos y represente la información mediante estructuras de datos adecuadas, con persistencia en base de datos y una arquitectura clara.

## 4. Tema del Proyecto
Desarrollar el sistema **"SmartCampus UTA Web"**, un aplicativo web orientado a la gestión de atención, trámites, organización documental y rutas internas del campus universitario, aplicando estructuras de datos avanzadas, base de datos relacional y arquitectura de software por capas.

## 5. Objetivo General
Desarrollar un aplicativo web integral para la gestión de atención, trámites, documentación y rutas internas de un campus universitario, aplicando estructuras de datos lineales y no lineales, diseño de base de datos a partir de requerimientos iniciales y una arquitectura de software por capas.

## 6. Objetivos Específicos
* Levantar y documentar los requerimientos funcionales y no funcionales del sistema.
* Analizar el problema y modelar los procesos principales del aplicativo.
* Diseñar la base de datos desde los requerimientos iniciales, incluyendo modelo conceptual, lógico y físico.
* Implementar estructuras de datos propias y asociarlas a funcionalidades reales del sistema.
* Diseñar una arquitectura de software web organizada por capas.
* Desarrollar el aplicativo web con interfaz, lógica de negocio, persistencia y seguridad básica.
* Validar el funcionamiento del sistema mediante pruebas funcionales y técnicas.
* Documentar y defender las decisiones tomadas en el proyecto.

## 7. Alcance del Proyecto
El sistema deberá incluir, como mínimo: autenticación de usuarios, administración de roles, registro de estudiantes o usuarios institucionales, gestión de turnos de atención, gestión de solicitudes o trámites, historial de acciones, organización jerárquica de documentos o dependencias, representación de un mapa del campus, búsqueda de rutas entre ubicaciones, almacenamiento persistente en base de datos e interfaz web funcional.

---

## 8. Requerimientos Iniciales

### 8.1 Requerimientos Funcionales
| Código | Descripción |
| :--- | :--- |
| **RF1** | Registrar usuarios y gestionar autenticación. |
| **RF2** | Administrar roles y permisos básicos. |
| **RF3** | Registrar estudiantes, personal o usuarios institucionales. |
| **RF4** | Registrar solicitudes o trámites. |
| **RF5** | Gestionar turnos de atención y atenderlos en orden de cola. |
| **RF6** | Consultar historial de trámites por usuario. |
| **RF7** | Organizar documentos por categorías jerárquicas. |
| **RF8** | Visualizar estructura de dependencias institucionales. |
| **RF9** | Registrar puntos del campus y sus conexiones. |
| **RF10** | Calcular rutas entre dos ubicaciones. |
| **RF11** | Generar reportes básicos y bitácora de acciones. |

### 8.2 Requerimientos No Funcionales
* El sistema debe implementarse como aplicativo web.
* Debe usar una base de datos relacional con integridad referencial.
* La solución debe organizarse con arquitectura por capas.
* Debe incluir validación de formularios y manejo básico de errores.
* La interfaz debe ser usable, consistente y claramente navegable.
* Debe existir documentación técnica y justificación del uso de estructuras de datos.

---

## 9. Diseño y Construcción de la Base de Datos
La base de datos deberá construirse a partir de los requerimientos levantados por el equipo. No se aceptará un diseño creado únicamente al final del desarrollo; por tanto, cada grupo debe demostrar la trazabilidad entre requerimientos, entidades, relaciones y módulos del sistema.

### 9.1 Actividades Mínimas
* Identificación de entidades, atributos y relaciones.
* Definición de cardinalidades y reglas de negocio.
* Elaboración del modelo conceptual, modelo lógico y modelo físico.
* Normalización y validación de integridad.
* Generación de script SQL y carga de datos de prueba.

### 9.2 Entidades Sugeridas
| Grupo | Entidades | Propósito |
| :--- | :--- | :--- |
| **Acceso** | Usuario, Rol | Autenticación y control de acceso. |
| **Personas** | Estudiante, Empleado | Gestión de actores del sistema. |
| **Procesos** | Trámite, Solicitud, Turno | Atención y seguimiento. |
| **Organización** | Dependencia, Categoría Documento, Documento | Estructura documental y administrativa. |
| **Rutas** | PuntoRuta, Conexión Ruta, Edificio | Mapa del campus y cálculo de recorridos. |
| **Trazabilidad** | HistorialAcción | Registro de eventos relevantes. |

---

## 10. Arquitectura de Software Propuesta
El sistema deberá implementarse con una arquitectura de software por capas, de modo que exista separación de responsabilidades entre presentación, lógica de aplicación, dominio y persistencia. Esta organización permitirá que el uso de estructuras de datos se ubique en la capa adecuada y que el aplicativo web sea mantenible y escalable.

| Capa | Responsabilidad Principal | Ejemplos |
| :--- | :--- | :--- |
| **Presentación** | Interacción con el usuario mediante navegador. | Formularios, vistas, dashboards, validaciones de interfaz. |
| **Aplicación** | Orquestación de casos de uso. | Registrar solicitud, asignar turno, consultar historial, buscar rutas. |
| **Dominio** | Reglas del negocio y estructuras de datos. | `ListaSimple`, `ListaDoble`, `ListaCircular`, `Pila`, `Cola`, `Árbol` y `Grafo`. |
| **Persistencia**| Acceso a datos y repositorios. | Consultas, inserciones, actualizaciones, borrado lógico. |
| **Datos** | Base de datos relacional y scripts. | Tablas, restricciones, vistas y datos semilla. |

---

## 11. Integración de Estructuras de Datos en el Sistema

| Estructura | Aplicación Sugerida | Justificación |
| :--- | :--- | :--- |
| **Lista secuencial** | Catálogos de tipos de trámite o edificios. | Acceso indexado y manejo compacto de datos de referencia. |
| **Lista simplemente enlazada** | Historial dinámico de solicitudes. | Inserciones frecuentes y crecimiento flexible. |
| **Lista doblemente enlazada** | Navegación hacia adelante/atrás en expedientes. | Permite recorrido bidireccional. |
| **Lista circular** | Rotación de ventanillas o responsables. | Representa ciclos y asignación Round-Robin. |
| **Pila** | Deshacer acciones o historial LIFO. | Adecuada para reversión y seguimiento del último evento. |
| **Cola** | Fila de atención estudiantil. | Modela el orden FIFO de servicio. |
| **Árbol** | Clasificación jerárquica de documentos o dependencias. | Representa niveles y relaciones padre-hijo. |
| **Grafo** | Mapa del campus y búsqueda de rutas. | Permite modelar conexiones complejas entre nodos. |

---

## 12. Implementación del Aplicativo Web
La solución final deberá entregarse como un aplicativo web funcional. Se recomienda el uso de una arquitectura tecnológica coherente con la carrera y con el nivel del curso, por ejemplo **ASP.NET Core** para backend y **SQL Server** o **PostgreSQL** como base de datos. Sin embargo, se podrá emplear otra tecnología web siempre que el grupo la justifique y documente adecuadamente.

### 12.1 Tecnologías Sugeridas
* **Frontend:** HTML, CSS, JavaScript y Bootstrap.
* **Backend:** ASP.NET Core MVC o ASP.NET Core Web API.
* **Persistencia:** Entity Framework Core o ADO.NET.
* **Base de datos:** SQL Server o PostgreSQL.
* **Control de versiones:** Git y GitHub.

---

## 13. Metodología de Trabajo y Fases
| Fase | Actividades | Producto Esperado |
| :--- | :--- | :--- |
| **Inicio y análisis** | Problema, alcance, actores, requerimientos y casos de uso. | Documento de análisis. |
| **Diseño** | Modelo de datos, arquitectura, diagramas y prototipo de interfaz. | Diseño técnico y base de datos. |
| **Construcción** | Implementación de estructuras, backend, frontend e integración. | Aplicativo web operativo. |
| **Pruebas** | Validation funcional, técnica y revisión de consistencia. | Casos de prueba y evidencias. |
| **Cierre** | Documentación final y defensa del proyecto. | Informe final, manuales y exposición. |

---

## 14. Entregables del Proyecto
| Entrega | Contenido Mínimo | Evidencias | Ponderación Sugerida |
| :--- | :--- | :--- | :--- |
| **1. Análisis** | Problema, justificación, objetivos, requerimientos y casos de uso. | Documento base y prototipo inicial. | 15% |
| **2. Base de datos** | MER, modelo lógico, normalización y script SQL. | Diagramas y script ejecutable. | 20% |
| **3. Diseño de software** | Arquitectura por capas, diagrama de clases y componentes. | Documento técnico. | 15% |
| **4. Avance funcional** | Aplicación web parcial con al menos 3 módulos operativos. | Demo y repositorio actualizado. | 20% |
| **5. Producto final** | Sistema completo, pruebas, documentación y defensa. | Aplicativo, manuales y exposición. | 30% |

---

## 15. Restricciones Académicas
* No se permitirá limitar la solución al uso exclusivo de estructuras provistas por librerías del lenguaje (deben ser de **implementación propia**).
* Cada estructura de datos implementada debe corresponder a una funcionalidad real del sistema.
* La base de datos debe derivarse del análisis inicial y mantener trazabilidad con los requerimientos.
* La entrega final debe ser web; no se aceptarán únicamente prototipos de consola.
* La documentación debe justificar las decisiones técnicas adoptadas por el equipo.

---

## 16. Rúbrica de Evaluación
| Criterio | Descripción | Valor | Indicador de Logro |
| :--- | :--- | :--- | :--- |
| **Análisis de requerimientos** | Claridad, completitud y coherencia del levantamiento. | 1.5 | Define necesidades y alcance de forma sólida. |
| **Diseño de base de datos** | Modelo correcto, normalizado y documentado. | 1.5 | Construye tablas y relaciones consistentes. |
| **Arquitectura de software** | Separación de capas y coherencia técnica. | 1.5 | Explica responsabilidades y flujos del sistema. |
| **Estructuras de datos** | Implementación propia e integración real. | 2.0 | Aplica estructuras adecuadas a módulos concretos. |
| **Aplicativo web** | Usabilidad, navegación y funcionalidad integrada. | 1.5 | Entrega un sistema web operativo. |
| **Persistencia** | Conexión estable con base de datos. | 1.0 | Guarda y recupera información correctamente. |
| **Pruebas y validación** | Casos de prueba y evidencia de funcionamiento. | 0.5 | Demuestra verificación básica del sistema. |
| **Documentación y defensa** | Presentación técnica y dominio del tema. | 0.5 | Justifica decisiones con claridad. |

---

## 17. Preguntas Sugeridas para la Defensa
1. ¿Por qué eligieron una cola para el módulo de atención y no otra estructura?
2. ¿Qué ventaja ofrece la lista circular en la rotación de ventanillas o responsables?
3. ¿Cómo justifican el uso de árbol en la clasificación documental o estructura organizacional?
4. ¿Qué representación del grafo usaron y por qué resultó apropiada para el campus?
5. ¿De qué manera la base de datos fue construida a partir de los requerimientos iniciales?
6. ¿Qué responsabilidades asumió cada capa de la arquitectura propuesta?

---

## 18. Enunciado Final para Estudiantes
Desarrollar un aplicativo web denominado **"SmartCampus UTA Web"**, orientado a la gestión de atención, trámites, documentación y rutas internas de un campus universitario. El sistema deberá construirse a partir del levantamiento de requerimientos iniciales, el diseño y construcción de una base de datos relacional, la definición de una arquitectura de software por capas y la implementación de estructuras de datos avanzadas como listas, pilas, colas, árboles y grafos.

La solución deberá evidenciar el uso real de dichas estructuras dentro de módulos funcionales del sistema, incluyendo atención por turnos, historial de operaciones, organización jerárquica de documentos y búsqueda de rutas entre ubicaciones del campus. La entrega final deberá incluir código fuente, repositorio, base de datos, pruebas, diagramas, manuales y defensa oral.

