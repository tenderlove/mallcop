require 'socket'

module MallCop
  class Session

    def self.start host, port
      new(host, port).start
    end

    def initialize host, port
      @host = host
      @port = port
    end

    def start
      case native_start(socket)
      when ERROR_SOCKET_NONE       then raise "The socket is invalid"
      when ERROR_BANNER_SEND       then raise "Unable to send banner to remote host"
      when ERROR_KEX_FAILURE       then raise "Encryption key exchange with the remote host failed"
      when ERROR_SOCKET_SEND       then raise "Unable to send data on socket"
      when ERROR_SOCKET_DISCONNECT then raise "The socket was disconnected"
      when ERROR_PROTO             then raise "An invalid SSH protocol response was received on the socket"
      when ERROR_EAGAIN            then raise "Marked for non-blocking I/O but the call would block"
      end

      self
    end

    def authenticate username, options
      userauth_password username, options[:password]
    end

    def open_channel
      case channel_or_error = native_open_channel
      when ERROR_ALLOC           then raise "An internal memory allocation call failed"
      when ERROR_SOCKET_SEND     then raise "Unable to send data on socket"
      when ERROR_CHANNEL_FAILURE then raise "The channel failed"
      when ERROR_EAGAIN          then raise "Marked for non-blocking I/O but the call would block"
      end
      channel_or_error
    end

  private

    def socket
      @socket ||= Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0).tap do |s|
        s.connect Socket.sockaddr_in(@port, @host)
      end
    rescue Errno::ECONNREFUSED => e
      raise ConnectionError, "Cannot establish a connection to #{@host} on port #{@port}"
    end

    def authlist_for user
      userauth_list(user).split ','
    end

  end
end
