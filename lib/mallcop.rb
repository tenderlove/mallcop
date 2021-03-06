require 'mallcop/mallcop'
require 'mallcop/channel'
require 'mallcop/session'
require 'mallcop/shell'

module MallCop
  VERSION = '1.0.0'

  # Raised when something happens with connecting to the server
  class RuntimeError    < ::RuntimeError; end
  class ConnectionError < StandardError ; end
  class SessionError    < StandardError ; end
  class ChannelError    < StandardError ; end

  # Establishes a connection to the SSH server.
  #
  # Arguments
  # =====
  # +host+      The host to connect to
  # +username+  The username to connect under.
  #
  # Optional:
  # +options+
  #   :port     The port to connect on (defaults to 22)
  #   :password Use a password based authentication strategy.
  def self.connect(host, username, options = {})
    session = Session.start(host, options[:port] || 22)

    unless session.authenticate username, options
      raise ChannelError, "Could not authenticate user '#{username}'"
    end

    if block_given?
      yield session
      session.disconnect("Closing session")
    end

    session
  end

  def self.shell(host, username, options = {})
    connect(host, username, options) do |session|
      shell = Shell.new(session)
      yield shell
      shell.cleanup
    end
  end
end
