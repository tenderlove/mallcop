require 'mallcop/mallcop'
require 'mallcop/session'

module MallCop
  VERSION = '1.0.0'

  def self.interactive(host, username, options = {})
    session = Session.start(host, options[:port] || 22)
    session.userauth_password username, options[:password]
    chan = session.open_channel
    chan.shell
    yield chan if block_given?
    chan
  end
end
