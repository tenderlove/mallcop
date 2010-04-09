require 'socket'

module MallCop
  class Session
    include Errors

    def self.start host, port
      session = new(host, port)
      session.start
      session
    end

    def initialize host, port
      @host = host
      @port = port
    end

    def start
      if native_start(socket) == 0
        return true
      end

      case native_last_errno
      when ERROR_SOCKET_NONE       then raise "The socket is invalid"
      when ERROR_BANNER_SEND       then raise "Unable to send banner to remote host"
      when ERROR_KEX_FAILURE       then raise "Encryption key exchange with the remote host failed"
      when ERROR_SOCKET_SEND       then raise "Unable to send data on socket"
      when ERROR_SOCKET_DISCONNECT then raise "The socket was disconnected"
      when ERROR_PROTO             then raise "An invalid SSH protocol response was received on the socket"
      when ERROR_EAGAIN            then raise "Marked for non-blocking I/O but the call would block"
      else raise "Something went wrong"
      end
    end

    def authenticate username, options
      userauth_password username, options[:password]
    end

    def open_channel
      if channel = native_open_channel
        return channel
      end

      case native_last_errno
      when ERROR_ALLOC           then raise "An internal memory allocation call failed"
      when ERROR_SOCKET_SEND     then raise "Unable to send data on socket"
      when ERROR_CHANNEL_FAILURE then raise "The channel failed"
      when ERROR_EAGAIN          then raise "Marked for non-blocking I/O but the call would block"
      else raise "Something went wrong"
      end
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
