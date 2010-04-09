require 'mallcop/mallcop'
require 'mallcop/channel'
require 'mallcop/session'
require 'mallcop/shell'

module MallCop
  VERSION = '1.0.0'

  # Raised when something happens with connecting to the server
  class ConnectionError < StandardError ; end

  def self.connect(host, username, options = {})
    session = Session.start(host, options[:port] || 22)
    session.userauth_password username, options[:password]

    yield session if block_given?
    session
  end

  def self.shell(host, username, options = {})
    session = connect(host, username, options)
    yield Shell.new(session)
  end
end
