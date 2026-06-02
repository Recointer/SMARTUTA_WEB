#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

// ============================================================
// JwtHelper — JWT HS256 usando OpenSSL (sin dependencias extra)
// ============================================================
class JwtHelper {
public:
    static inline const std::string SECRET = []() -> std::string {
        const char* env = std::getenv("JWT_SECRET");
        return env ? env : "SmartCampusUTA_SecretKey_2024";
    }();

private:
    // Base64URL encode
    static std::string b64url_encode(const unsigned char* data, size_t len) {
        static const char tbl[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string out;
        for (size_t i = 0; i < len; i += 3) {
            unsigned int b  = (data[i] << 16)
                            | ((i+1 < len ? data[i+1] : 0) << 8)
                            | (i+2 < len ? data[i+2] : 0);
            out += tbl[(b >> 18) & 0x3F];
            out += tbl[(b >> 12) & 0x3F];
            out += (i+1 < len) ? tbl[(b >> 6) & 0x3F] : '=';
            out += (i+2 < len) ? tbl[b & 0x3F]        : '=';
        }
        // url-safe: +→- /→_ remove =
        for (auto& c : out) { if (c=='+') c='-'; if (c=='/') c='_'; }
        out.erase(std::remove(out.begin(), out.end(), '='), out.end());
        return out;
    }

    static std::string b64url_encode_str(const std::string& s) {
        return b64url_encode(
            reinterpret_cast<const unsigned char*>(s.c_str()), s.size());
    }

    static std::string b64url_decode(const std::string& in) {
        std::string s = in;
        for (auto& c : s) { if (c=='-') c='+'; if (c=='_') c='/'; }
        while (s.size() % 4) s += '=';
        std::string out;
        for (size_t i = 0; i < s.size(); i += 4) {
            auto v = [](char c) -> int {
                if (c>='A'&&c<='Z') return c-'A';
                if (c>='a'&&c<='z') return c-'a'+26;
                if (c>='0'&&c<='9') return c-'0'+52;
                if (c=='+') return 62; if (c=='/') return 63; return 0;
            };
            int b = (v(s[i])<<18)|(v(s[i+1])<<12)
                   |(v(s[i+2])<<6)|v(s[i+3]);
            out += char(b>>16); if (s[i+2]!='=') out += char((b>>8)&0xFF);
            if (s[i+3]!='=') out += char(b&0xFF);
        }
        return out;
    }

    static std::string hmac_sha256(const std::string& data, const std::string& key) {
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int  dlen = 0;
        HMAC(EVP_sha256(),
             key.c_str(), key.size(),
             reinterpret_cast<const unsigned char*>(data.c_str()), data.size(),
             digest, &dlen);
        return b64url_encode(digest, dlen);
    }

public:
    // Hash de contraseña con SHA-256 + salt
    static std::string hashPassword(const std::string& password) {
        std::string salted = "SMARTUTA_" + password;
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(salted.c_str()),
               salted.size(), digest);
        std::ostringstream oss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        return oss.str();
    }

    static std::string createToken(int userId, const std::string& rol,
                                   const std::string& nombre, int expHours = 8) {
        // Header
        std::string header  = b64url_encode_str(R"({"alg":"HS256","typ":"JWT"})");
        // Payload
        long now = (long)std::time(nullptr);
        std::ostringstream pl;
        pl << "{\"sub\":" << userId
           << ",\"rol\":\"" << rol << "\""
           << ",\"nombre\":\"" << nombre << "\""
           << ",\"iat\":" << now
           << ",\"exp\":" << (now + expHours * 3600) << "}";
        std::string payload = b64url_encode_str(pl.str());
        // Signature
        std::string sig     = hmac_sha256(header + "." + payload, SECRET);
        return header + "." + payload + "." + sig;
    }

    static bool verifyToken(const std::string& token,
                            int& userId, std::string& rol, std::string& nombre) {
        auto dot1 = token.find('.');
        if (dot1 == std::string::npos) return false;
        auto dot2 = token.find('.', dot1 + 1);
        if (dot2 == std::string::npos) return false;

        std::string header  = token.substr(0, dot1);
        std::string payload = token.substr(dot1 + 1, dot2 - dot1 - 1);
        std::string sig     = token.substr(dot2 + 1);

        // Verify signature
        if (hmac_sha256(header + "." + payload, SECRET) != sig) return false;

        // Decode payload (minimal JSON parse)
        std::string json = b64url_decode(payload);
        auto extract = [&](const std::string& key) -> std::string {
            auto p = json.find("\"" + key + "\":");
            if (p == std::string::npos) return "";
            p += key.size() + 3;
            if (json[p] == '"') {
                auto e = json.find('"', p + 1);
                return json.substr(p + 1, e - p - 1);
            }
            auto e = json.find_first_of(",}", p);
            return json.substr(p, e - p);
        };

        // Check expiry
        long exp = std::stol(extract("exp"));
        if (std::time(nullptr) > exp) return false;

        userId = std::stoi(extract("sub"));
        rol    = extract("rol");
        nombre = extract("nombre");
        return true;
    }

    static std::string extractToken(const std::string& authHeader) {
        if (authHeader.rfind("Bearer ", 0) == 0)
            return authHeader.substr(7);
        return "";
    }
};
