#pragma once
#include <nlohmann/json.hpp>
#include "../persistence/UsuarioRepo.h"
#include "../utils/JwtHelper.h"

using json = nlohmann::json;

class AuthService {
public:
    // Retorna {ok, token, usuario} o {ok:false, error}
    static json login(const std::string& email, const std::string& password) {
        auto u = UsuarioRepo::findByEmail(email);
        if (u.is_null()) return {{"ok", false}, {"error", "Usuario no encontrado"}};

        std::string expectedHash = JwtHelper::hashPassword(password);
        if (u["password_hash"].get<std::string>() != expectedHash)
            return {{"ok", false}, {"error", "Contraseña incorrecta"}};

        std::string token = JwtHelper::createToken(
            u["id"].get<int>(),
            u["rol"].get<std::string>(),
            u["nombre"].get<std::string>()
        );

        return {{"ok",     true},
                {"token",  token},
                {"id",     u["id"]},
                {"nombre", u["nombre"]},
                {"rol",    u["rol"]}};
    }

    static json registrar(const std::string& nombre, const std::string& email,
                          const std::string& password, int rol_id) {
        try {
            std::string hash = JwtHelper::hashPassword(password);
            int id = UsuarioRepo::crear(nombre, email, hash, rol_id);
            return {{"ok", true}, {"id", id}};
        } catch (const std::exception& e) {
            return {{"ok", false}, {"error", e.what()}};
        }
    }
};
