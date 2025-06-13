#!/bin/bash

echo "🔒 LIMPIEZA SEGURA DE CREDENCIALES - git-filter-repo"
echo "===================================================="

# Backup del repositorio
echo "📦 Creando backup completo..."
cd ..
cp -r caloa caloa_backup_$(date +%Y%m%d_%H%M%S)
cd caloa

echo "🔥 Eliminando archivos con credenciales del historial..."

# Lista de archivos a eliminar del historial
CREDENTIAL_FILES=(
    "sync_website.sh"
    "filezilla_commands.sh" 
    "lftp_segunda_cuenta.txt"
    "upload_segunda_cuenta.sh"
    "filezilla_upload.sh"
    "ftp_upload.sh"
    "upload_to_ftp.sh"
    "upload_ambas_cuentas.sh"
)

# Eliminar archivos del historial
for file in "${CREDENTIAL_FILES[@]}"; do
    echo "   🗑️  Eliminando del historial: $file"
    git filter-repo --path "$file" --invert-paths --force
done

echo "🔍 Eliminando patrones de credenciales..."

# Eliminar líneas específicas con credenciales
git filter-repo --replace-text <(cat <<EOF
ftpupload.net==>***REMOVED***
if0_39194298==>***REMOVED***
3JCqt7IUk4ED==>***REMOVED***
tttng_39201066==>***REMOVED***
5772cbdea57d017==>***REMOVED***
EOF
) --force

echo "✅ LIMPIEZA COMPLETADA"
echo ""
echo "⚠️  ACCIONES CRÍTICAS REQUERIDAS:"
echo "   1. Verificar limpieza:"
echo "      git log --oneline --all | grep -i ftp"
echo "      git log --oneline --all | grep -i cred"
echo ""
echo "   2. Push forzado al repositorio:"
echo "      git push origin --force --all"
echo "      git push origin --force --tags"
echo ""
echo "🔑 CAMBIAR CREDENCIALES INMEDIATAMENTE:"
echo "   - Cambiar password de if0_39194298"
echo "   - Cambiar password de tttng_39201066" 
echo "   - Revisar logs de acceso FTP"
echo "   - Considerar regenerar API keys"
echo ""
echo "🚨 VERIFICAR TAMBIÉN:"
echo "   - GitHub Security tab"
echo "   - git log --all --grep='password'"
echo "   - git log --all --grep='credential'" 