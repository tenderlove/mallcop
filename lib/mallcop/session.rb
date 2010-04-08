module MallCop
  class Session
    ###
    # Create a new MallCop::Session bound to +socket+
    def initialize host, port
      @host = host
      @port = port
    end

    def start
      native_start socket
      self
    end

    def authlist_for user
      userauth_list(user).split ','
    end

  private

    def socket
      @socket ||= Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0).tap do |s|
        s.connect Socket.sockaddr_in(@port, @host)
      end
    end

  end
end
