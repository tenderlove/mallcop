require 'socket'

module MallCop
  class Session

    def self.start host, port
      session = new(host, port)
      session.start
      session
    end

    def initialize host, port
      native_initialize
      @host = host
      @port = port
    end

    def start
      if native_start(socket.fileno) == 0
        return true
      end

      raise ConnectionError, native_last_errmsg
    end

    def authenticate username, options
      allowed = authlist_for username

      attempted = []

      auth_methods.each do |method|
        next unless allowed.include?(method)

        attempted << method

        if send "authenticate_via_#{method.downcase}", username, options
          return true
        end
      end

      false
    end

    def open_channel
      Channel.new(self)
    end

    def hostkey_hash(hashtype)
      native_hostkey_hash(hashtype)
    end

    def userauth_publickey_fromfile(user, public_key, private_key, password)
      native_userauth_publickey_fromfile(user, public_key, private_key, password)
    end

    def disconnect(description)
      native_disconnect(description)
    end

  private

    def socket
      @socket ||= Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0).tap do |s|
        s.connect Socket.sockaddr_in(@port, @host)
      end
    rescue Errno::ECONNREFUSED => e
      raise ConnectionError, "Cannot establish a connection to #{@host} on port #{@port}"
    end

    def auth_methods
      %w(publickey hostbased password)
    end

    def authlist_for username
      native_userauth_list(username).split ','
    end

    def authenticate_via_password username, options
      res = native_userauth_password(username, options[:password])
      res == 0
    end

    def authenticate_via_publickey username, options
      # Not implemented yet
    end
    
    def authenticate_via_hostbased username, options
      # Not implemented yet
    end
  end
end
