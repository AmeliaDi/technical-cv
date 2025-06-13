#!/bin/bash

echo "🔒 LIMPIEZA DE CREDENCIALES DEL HISTORIAL GIT"
echo "=============================================="

# Backup del repositorio antes de limpiar
echo "📦 Creando backup..."
cp -r .git .git_backup_$(date +%Y%m%d_%H%M%S)

# Lista de archivos con credenciales a eliminar del historial
FILES_TO_REMOVE=(
    "sync_website.sh"
    "filezilla_commands.sh" 
    "lftp_segunda_cuenta.txt"
    "upload_segunda_cuenta.sh"
    "filezilla_upload.sh"
    "ftp_upload.sh"
    "upload_to_ftp.sh"
    "upload_ambas_cuentas.sh"
)

echo "🔥 Eliminando archivos del historial Git..."

# Eliminar archivos del historial usando git filter-branch
for file in "${FILES_TO_REMOVE[@]}"; do
    echo "   🗑️  Eliminando: $file"
    git filter-branch --force --index-filter \
        "git rm --cached --ignore-unmatch '$file'" \
        --prune-empty --tag-name-filter cat -- --all
done

echo "🧹 Limpiando referencias..."
rm -rf .git/refs/original/
git reflog expire --expire=now --all
git gc --prune=now --aggressive

echo "✅ LIMPIEZA COMPLETADA"
echo ""
echo "⚠️  SIGUIENTE PASO CRÍTICO:"
echo "   Debes hacer force push al repositorio:"
echo "   git push origin --force --all"
echo ""
echo "🔑 CAMBIAR CREDENCIALES INMEDIATAMENTE:"
echo "   - Cambiar password de ***REMOVED***"
echo "   - Cambiar password de ***REMOVED***"
echo "   - Revisar logs de acceso FTP" 