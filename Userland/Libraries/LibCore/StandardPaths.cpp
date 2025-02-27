/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2023, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteString.h>
#include <AK/LexicalPath.h>
#include <AK/Platform.h>
#include <AK/String.h>
#include <AK/StringBuilder.h>
#include <LibCore/SessionManagement.h>
#include <LibCore/StandardPaths.h>
#include <LibCore/System.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(AK_OS_HAIKU)
#    include <FindDirectory.h>
#endif

namespace Core {

ByteString StandardPaths::home_directory()
{
    if (auto* home_env = getenv("HOME"))
        return LexicalPath::canonicalized_path(home_env);

    auto* pwd = getpwuid(getuid());
    ByteString path = pwd ? pwd->pw_dir : "/";
    endpwent();
    return LexicalPath::canonicalized_path(path);
}

ByteString StandardPaths::desktop_directory()
{
    StringBuilder builder;
    builder.append(home_directory());
    builder.append("/Desktop"sv);
    return LexicalPath::canonicalized_path(builder.to_byte_string());
}

ByteString StandardPaths::documents_directory()
{
    StringBuilder builder;
    builder.append(home_directory());
    builder.append("/Documents"sv);
    return LexicalPath::canonicalized_path(builder.to_byte_string());
}

ByteString StandardPaths::downloads_directory()
{
    StringBuilder builder;
    builder.append(home_directory());
    builder.append("/Downloads"sv);
    return LexicalPath::canonicalized_path(builder.to_byte_string());
}

ByteString StandardPaths::config_directory()
{
    if (auto* config_directory = getenv("XDG_CONFIG_HOME"))
        return LexicalPath::canonicalized_path(config_directory);

    StringBuilder builder;
    builder.append(home_directory());
#if defined(AK_OS_MACOS)
    builder.append("/Library/Preferences"sv);
#elif defined(AK_OS_HAIKU)
    builder.append("/config/settings"sv);
#else
    builder.append("/.config"sv);
#endif
    return LexicalPath::canonicalized_path(builder.to_byte_string());
}

ByteString StandardPaths::data_directory()
{
    if (auto* data_directory = getenv("XDG_DATA_HOME"))
        return LexicalPath::canonicalized_path(data_directory);

    StringBuilder builder;
    builder.append(home_directory());
#if defined(AK_OS_SERENITY)
    builder.append("/.data"sv);
#elif defined(AK_OS_MACOS)
    builder.append("/Library/Application Support"sv);
#elif defined(AK_OS_HAIKU)
    builder.append("/config/non-packaged/data"sv);
#else
    builder.append("/.local/share"sv);
#endif

    return LexicalPath::canonicalized_path(builder.to_byte_string());
}

ErrorOr<ByteString> StandardPaths::runtime_directory()
{
    if (auto* data_directory = getenv("XDG_RUNTIME_DIR"))
        return LexicalPath::canonicalized_path(data_directory);

    StringBuilder builder;

#if defined(AK_OS_SERENITY)
    auto sid = TRY(Core::SessionManagement::root_session_id());
    builder.appendff("/tmp/session/{}", sid);
#elif defined(AK_OS_MACOS)
    builder.append(home_directory());
    builder.append("/Library/Application Support"sv);
#elif defined(AK_OS_HAIKU)
    builder.append("/boot/system/var/shared_memory"sv);
#elif defined(AK_OS_LINUX)
    auto uid = getuid();
    builder.appendff("/run/user/{}", uid);
#else
    // Just create a directory in /tmp that's owned by us with 0700
    auto uid = getuid();
    builder.appendff("/tmp/runtime_{}", uid);
    auto error_or_stat = System::stat(builder.string_view());
    if (error_or_stat.is_error()) {
        MUST(System::mkdir(builder.string_view(), 0700));
    } else {
        auto stat = error_or_stat.release_value();
        VERIFY(S_ISDIR(stat.st_mode));
        if ((stat.st_mode & 0777) != 0700)
            warnln("{} has unexpected mode flags {}", builder.string_view(), stat.st_mode);
    }
#endif

    return LexicalPath::canonicalized_path(builder.to_byte_string());
}

ByteString StandardPaths::tempfile_directory()
{
    return "/tmp";
}

ErrorOr<Vector<String>> StandardPaths::font_directories()
{
#if defined(AK_OS_HAIKU)
    Vector<String> paths_vector;
    char** paths;
    size_t paths_count;
    if (find_paths(B_FIND_PATH_FONTS_DIRECTORY, NULL, &paths, &paths_count) == B_OK) {
        for (size_t i = 0; i < paths_count; ++i) {
            StringBuilder builder;
            builder.append(paths[i], strlen(paths[i]));
            paths_vector.append(TRY(builder.to_string()));
        }
    }
    return paths_vector;
#else
    return Vector { {
#    if defined(AK_OS_SERENITY)
        "/res/fonts"_string,
#    elif defined(AK_OS_MACOS)
        "/System/Library/Fonts"_string,
        "/Library/Fonts"_string,
        TRY(String::formatted("{}/Library/Fonts"sv, home_directory())),
#    else
        "/usr/share/fonts"_string,
        "/usr/local/share/fonts"_string,
        TRY(String::formatted("{}/.local/share/fonts"sv, home_directory())),
#    endif
    } };
#endif
}

}
