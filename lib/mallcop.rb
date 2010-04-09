require 'mallcop/mallcop'
require 'mallcop/channel'
require 'mallcop/session'
require 'mallcop/shell'

module MallCop
  VERSION = '1.0.0'

  def self.interactive(host, username, options = {})
    session = Session.start(host, options[:port] || 22)
    session.userauth_password username, options[:password]

    yield Shell.new(session)
  end
end
