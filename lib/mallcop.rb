require 'socket'
require 'mallcop/mallcop'
require 'mallcop/session'

module MallCop
  VERSION = '1.0.0'

  def self.interactive(host, username, options = {})
    session = Session.new(host, options[:port] || 22)
    session.start
    session.userauth_password username, options[:password]
    chan = session.open_channel
    chan.channel_exec("bash -l")
    yield chan if block_given?
    chan
  end
end
