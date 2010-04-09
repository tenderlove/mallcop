require 'socket'

module MallCop
  class Session

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

      raise ConnectionError, native_last_errmsg
    end

    def authenticate username, options
      userauth_password username, options[:password]
    end

    def open_channel
      if channel = native_open_channel
        return channel
      end

      raise ChannelError, native_last_errmsg
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
