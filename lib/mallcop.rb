require 'socket'
require 'mallcop/mallcop'
require 'mallcop/session'

module MallCop
  VERSION = '1.0.0'

  def self.interactive(host, username, options = {})
    socket = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    socket.connect(Socket.sockaddr_in(22, host))
    session = Session.new(socket)
    session.start
    session.userauth_password username, options[:password]
    chan = session.open_channel
    chan.channel_exec("bash -l")
    yield chan if block_given?
    chan
  end
end
