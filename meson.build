project('steam', 'cpp')

steam_inc = include_directories('steam/sdk/public')

quickjs = dependency('quickjs', static:true)

steam_lib_path = ''
steam_ext = ''
if host_machine.system() == 'linux'
    if host_machine.cpu_family() == 'x86'
        steam_lib_path = 'linux32'
    elif host_machine.cpu_family() == 'x86_64'
        steam_lib_path = 'linux64'
    endif
elif host_machine.system() == 'darwin'
    steam_lib_path = 'osx'
elif host_machine.system() == 'windows'
    if host_machine.cpu_family() == 'x86_64'
        steam_lib_path = 'win64'
        steam_ext = '64'
    endif
endif

cxx = meson.get_compiler('cpp')
steam_dep = declare_dependency(
  dependencies: cxx.find_library('steam_api'+ steam_ext, dirs: meson.current_source_dir() / 'steam' / 'sdk' / 'redistributable_bin' / steam_lib_path))

steam_lib = library('steam',
  'qjs_steam.cpp',
  include_directories: steam_inc,
  cpp_args: '-std=c++11',
  dependencies: [quickjs, steam_dep],
  c_shared_args: '-DJS_SHARED_LIBRARY',
)

qjs_steam_dep = declare_dependency(
  link_with: steam_lib,
  include_directories : steam_inc
)
