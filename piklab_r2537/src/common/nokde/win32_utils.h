/*
   This file is part of the KDE libraries
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KDE_WIN32_UTILS_H
#define KDE_WIN32_UTILS_H

#include <windows.h>

//#include <kdecore/kdelibs_export.h>
#define KDEWIN32_EXPORT

#ifdef  __cplusplus
#include <qstring.h>

extern "C" {
#endif

#define fcopy_src_err -1
#define fcopy_dest_err -2

/**
 Copies @p src file to @p dest file.
 @return 0 on success, fcopy_src_err on source file error,
 fcopy_dest_err on destination file error.
*/
KDEWIN32_EXPORT int fcopy(const char *src, const char *dest);

/**
 Converts all backslashes to slashes in @p path.
 Converting is stopped on a null character or at @p maxlen character.
*/
KDEWIN32_EXPORT void win32_slashify(char *path, int maxlen);

#ifdef  __cplusplus
}

/**
 \return a value from MS Windows native registry.
 @param key is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h. 
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 @param item is an item inside subKey or "" if default folder's value should be returned
 @param ok if not null, will be set to true on success and false on failure
*/
KDEWIN32_EXPORT QString getWin32RegistryValue(HKEY key, const QString& subKey, 
	const QString& item, bool *ok = 0);

/**
 \return a value from MS Windows native registry for shell folder \a folder.
*/
inline QString getWin32ShellFoldersPath(const QString& folder) {
	return getWin32RegistryValue(HKEY_CURRENT_USER, 
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", folder);
}

/**
 Shows native MS Windows file property dialog for a file \a fileName.
 Return true on success. Only works for local absolute paths.
 Used by KPropertiesDialog, if possible.
*/
KDEWIN32_EXPORT
bool showWin32FilePropertyDialog(const QString& fileName);

/**
 \return two-letter locale name (like "en" or "pl") taken from MS Windows native registry.
 Useful when we don't want to rely on KSyCoCa.
 Used e.g. by kbuildsycoca application.
*/
KDEWIN32_EXPORT
QCString getWin32LocaleName();

/*! Temporary solutiuon
 \return a KFileDialog-compatible filter string converted to QFileDialog compatible one. 
 This is temporary solution for kdelibs/win32... */
KDEWIN32_EXPORT QString convertKFileDialogFilterToQFileDialogFilter(const QString& filter);

#endif //__cplusplus

#endif
