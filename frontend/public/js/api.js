// ── api.js — fetch wrapper + JWT management ──────────────────

const API_BASE = '/api';

const api = {
    getToken() { return localStorage.getItem('sc_token'); },
    getUser()  { return JSON.parse(localStorage.getItem('sc_user') || 'null'); },
    setAuth(token, user) {
        localStorage.setItem('sc_token', token);
        localStorage.setItem('sc_user', JSON.stringify(user));
    },
    clearAuth() {
        localStorage.removeItem('sc_token');
        localStorage.removeItem('sc_user');
    },
    isLoggedIn() { return !!this.getToken(); },

    async request(method, path, body = null) {
        const headers = { 'Content-Type': 'application/json' };
        const token = this.getToken();
        if (token) headers['Authorization'] = 'Bearer ' + token;

        const opts = { method, headers };
        if (body) opts.body = JSON.stringify(body);

        const res = await fetch(API_BASE + path, opts);

        if (res.status === 401) {
            this.clearAuth();
            window.location.href = '/index.html';
            return null;
        }
        const data = await res.json().catch(() => ({}));
        return { status: res.status, data, ok: res.ok };
    },

    get(path)         { return this.request('GET',    path); },
    post(path, body)  { return this.request('POST',   path, body); },
    put(path, body)   { return this.request('PUT',    path, body); },
    delete(path)      { return this.request('DELETE', path); },
};

// ── Toast notifications ──────────────────────────────────────
function toast(msg, type = 'info') {
    let container = document.getElementById('toast-container');
    if (!container) {
        container = document.createElement('div');
        container.id = 'toast-container';
        document.body.appendChild(container);
    }
    const icons = { success: '✅', error: '❌', info: 'ℹ️' };
    const el = document.createElement('div');
    el.className = `toast ${type}`;
    el.innerHTML = `<span>${icons[type] || 'ℹ️'}</span><span>${msg}</span>`;
    container.appendChild(el);
    setTimeout(() => el.remove(), 3500);
}

// ── Guard: redirect to login if not authenticated ────────────
function requireAuth() {
    if (!api.isLoggedIn()) window.location.href = '/index.html';
}

// ── Guard: require specific role ─────────────────────────────
function requireRole(...roles) {
    const user = api.getUser();
    if (!user || !roles.includes(user.rol)) {
        toast('No tienes permisos para esta sección', 'error');
        window.location.href = '/dashboard.html';
    }
}

// ── Render sidebar based on user role ───────────────────────
function renderSidebar(activePage = '') {
    const user = api.getUser();
    if (!user) return;

    const nav = [
        { href: 'dashboard.html',        icon: '🏠', label: 'Dashboard',      roles: ['admin','secretaria'] },
        { href: 'portal-estudiante.html', icon: '🎓', label: 'Mi Portal',       roles: ['estudiante'] },
        { href: 'turnos.html',            icon: '🎫', label: 'Turnos',          roles: ['admin','secretaria','estudiante'] },
        { href: 'tramites.html',          icon: '📋', label: 'Mis Trámites',    roles: ['estudiante'] },
        { href: 'tramites.html',          icon: '📋', label: 'Trámites',        roles: ['admin','secretaria'] },
        { href: 'historial.html',         icon: '📜', label: 'Historial',       roles: ['admin','secretaria'] },
        { href: 'documentos.html',        icon: '📁', label: 'Documentos',      roles: ['admin','secretaria','estudiante'] },
        { href: 'dependencias.html',      icon: '🏛️', label: 'Dependencias',    roles: ['admin','secretaria','estudiante'] },
        { href: 'campus.html',            icon: '🗺️', label: 'Mapa Campus',     roles: ['admin','secretaria','estudiante'] },
        { href: 'reportes.html',          icon: '📊', label: 'Reportes',        roles: ['admin'] },
        { href: 'admin.html',             icon: '⚙️', label: 'Administración',  roles: ['admin'] },

    ];

    const sidebarEl = document.getElementById('sidebar');
    if (!sidebarEl) return;

    const links = nav
        .filter(n => n.roles.includes(user.rol))
        .map(n => `
            <a href="${n.href}" class="nav-link${activePage === n.href ? ' active' : ''}">
                <span class="icon">${n.icon}</span>${n.label}
            </a>`)
        .join('');

    const initial = (user.nombre || 'U')[0].toUpperCase();

    sidebarEl.innerHTML = `
        <div class="sidebar-logo">
            <h1>SmartCampus</h1>
            <span>Universidad Técnica de Ambato</span>
        </div>
        <nav class="sidebar-nav">
            <div class="nav-section">
                <div class="nav-label">Navegación</div>
                ${links}
            </div>
        </nav>
        <div class="sidebar-footer">
            <div class="user-badge">
                <div class="user-avatar">${initial}</div>
                <div class="user-info">
                    <div class="name">${user.nombre}</div>
                    <div class="role">${user.rol}</div>
                </div>
                <button onclick="logout()" class="btn btn-ghost btn-sm btn-icon" title="Salir" style="margin-left:auto">🚪</button>
            </div>
        </div>`;
}

function logout() {
    api.clearAuth();
    window.location.href = '/index.html';
}

// ── Badge helpers ─────────────────────────────────────────────
function estadoBadge(estado) {
    const map = {
        pendiente: 'badge-yellow',
        atendido:  'badge-green',
        en_proceso:'badge-blue',
        completado:'badge-green',
        rechazado: 'badge-red',
        nueva:     'badge-blue',
    };
    return `<span class="badge ${map[estado] || 'badge-gray'}">${estado}</span>`;
}

function formatDate(iso) {
    if (!iso) return '—';
    return new Date(iso).toLocaleString('es-EC', {
        day:'2-digit', month:'2-digit', year:'numeric',
        hour:'2-digit', minute:'2-digit'
    });
}
