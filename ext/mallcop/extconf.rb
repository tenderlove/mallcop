require 'mkmf'

# :stopdoc:

dir_config 'libssh2', '/opt/local/include', '/opt/local/lib'

def asplode missing
  abort "#{missing} is missing. Please install libssh2."
end

asplode('libssh2.h')  unless find_header 'libssh2.h'
asplode('libssh2') unless find_library 'ssh2', 'libssh2_session_init_ex'

create_makefile 'mallcop'

# :startdoc:
