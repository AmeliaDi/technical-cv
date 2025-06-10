#!/bin/bash

echo "🔍 CREDENTIAL LEAK SCANNER v2.0"
echo "==============================="
echo "Escaneando el repositorio en busca de credenciales filtradas..."
echo ""

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

FOUND_ISSUES=0

# Función para reportar encontrados
report_finding() {
    local file="$1"
    local line="$2"
    local pattern="$3"
    local content="$4"
    
    echo -e "${RED}🚨 CREDENCIAL DETECTADA${NC}"
    echo -e "   📁 Archivo: ${YELLOW}$file${NC}"
    echo -e "   📍 Línea: $line"
    echo -e "   🔍 Patrón: $pattern"
    echo -e "   📝 Contenido: $content"
    echo ""
    ((FOUND_ISSUES++))
}

echo "🔍 Escaneando patrones de credenciales comunes..."

# Patrones de credenciales a buscar
patterns=(
    "password\s*[:=]\s*['\"]?[^'\"[:space:]]+['\"]?"
    "passwd\s*[:=]\s*['\"]?[^'\"[:space:]]+['\"]?"
    "secret\s*[:=]\s*['\"]?[^'\"[:space:]]+['\"]?"
    "api[_-]?key\s*[:=]\s*['\"]?[^'\"[:space:]]+['\"]?"
    "access[_-]?token\s*[:=]\s*['\"]?[^'\"[:space:]]+['\"]?"
    "auth[_-]?token\s*[:=]\s*['\"]?[^'\"[:space:]]+['\"]?"
    "bearer\s+[a-zA-Z0-9._-]+"
    "ftp://[^[:space:]]+"
    "://[^/:]+:[^@/]+@"
    "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}:[^[:space:]]+"
    "BEGIN.*PRIVATE.*KEY"
    "ssh-rsa\s+[A-Za-z0-9+/]+"
    "ssh-ed25519\s+[A-Za-z0-9+/]+"
    "['\"][A-Za-z0-9]{20,}['\"]"
    "mongodb://[^[:space:]]+"
    "mysql://[^[:space:]]+"
    "postgres://[^[:space:]]+"
    "redis://[^[:space:]]+"
)

# Credenciales específicas conocidas
specific_patterns=(
    "ftpupload\.net"
    "if0_39194298"
    "3JCqt7IUk4ED"
    "tttng_39201066"
    "5772cbdea57d017"
)

# Archivos a excluir del escaneo
exclude_patterns="\.git/|node_modules/|\.png$|\.jpg$|\.jpeg$|\.gif$|\.ico$|\.woff|\.woff2|\.ttf|\.eot|\.otf|\.mp4|\.webm|\.pdf|\.zip|\.tar\.gz|\.dmg|\.exe|cleanup_git_history\.sh|secure_cleanup\.sh|credential_scanner\.sh"

echo "📁 Escaneando archivos..."

# Escanear patrones generales
for pattern in "${patterns[@]}"; do
    echo "   🔍 Buscando: $pattern"
    while IFS=: read -r file line content; do
        if [[ -n "$file" && -n "$content" ]]; then
            report_finding "$file" "$line" "$pattern" "$content"
        fi
    done < <(grep -rn -E -i "$pattern" . --exclude-dir=.git --exclude-dir=node_modules 2>/dev/null | grep -v -E "$exclude_patterns")
done

# Escanear patrones específicos conocidos
echo ""
echo "🎯 Escaneando credenciales específicas conocidas..."
for pattern in "${specific_patterns[@]}"; do
    echo "   🔍 Buscando: $pattern"
    while IFS=: read -r file line content; do
        if [[ -n "$file" && -n "$content" ]]; then
            report_finding "$file" "$line" "$pattern" "$content"
        fi
    done < <(grep -rn -E "$pattern" . --exclude-dir=.git --exclude-dir=node_modules 2>/dev/null | grep -v -E "$exclude_patterns")
done

echo ""
echo "🕵️ Escaneando historial de Git..."

# Escanear historial de git
git_patterns=(
    "password"
    "secret"
    "key"
    "token"
    "credential"
    "ftp"
    "ftpupload"
    "if0_39194298"
    "3JCqt7IUk4ED"
    "tttng_39201066"
    "5772cbdea57d017"
)

for pattern in "${git_patterns[@]}"; do
    echo "   🔍 Git historial: $pattern"
    result=$(git log --all --grep="$pattern" --oneline 2>/dev/null)
    if [[ -n "$result" ]]; then
        echo -e "${RED}🚨 CREDENCIAL EN HISTORIAL GIT${NC}"
        echo -e "   🔍 Patrón: $pattern"
        echo -e "   📝 Commits:"
        echo "$result" | while read line; do
            echo "     $line"
        done
        echo ""
        ((FOUND_ISSUES++))
    fi
done

echo ""
echo "📊 RESUMEN DEL ESCANEO"
echo "====================="

if [[ $FOUND_ISSUES -eq 0 ]]; then
    echo -e "${GREEN}✅ ¡REPOSITORIO LIMPIO!${NC}"
    echo "   No se encontraron credenciales filtradas."
    echo ""
    echo "🛡️  RECOMENDACIONES DE SEGURIDAD:"
    echo "   ✓ Usar variables de entorno para credenciales"
    echo "   ✓ Mantener .gitignore actualizado"
    echo "   ✓ Usar git-secrets o pre-commit hooks"
    echo "   ✓ Revisar regularmente con este script"
else
    echo -e "${RED}⚠️  CREDENCIALES ENCONTRADAS: $FOUND_ISSUES${NC}"
    echo ""
    echo "🚨 ACCIONES REQUERIDAS:"
    echo "   1. Eliminar credenciales de archivos"
    echo "   2. Limpiar historial con git-filter-repo"
    echo "   3. Cambiar todas las credenciales comprometidas"
    echo "   4. Revisar logs de acceso"
    echo "   5. Push forzado después de limpieza"
fi

echo ""
echo "🔧 Para limpiar historial, ejecuta:"
echo "   ./secure_cleanup.sh"

exit $FOUND_ISSUES 