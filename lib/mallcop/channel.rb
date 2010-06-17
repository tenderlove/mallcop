module MallCop
  class Channel

    def initialize(session)
      @session, @open = session, true

      res = native_initialize
      until res != ERROR_EAGAIN
        @session.io_select
        res = native_initialize
      end
    end

    def read
      raise ChannelError, "The channel has been closed" unless @open

      retval = ERROR_EAGAIN

      until retval != ERROR_EAGAIN
        @session.io_select
        retval = read_nonblock
      end

      retval
    end

    def read_nonblock
      native_channel_read
    end

    def write(cmd)
      raise ChannelError, "The channel has been closed" unless @open
      native_channel_write(cmd)
    end

    def eof?
      native_eof?
    end

    def close
      return true unless @open
      case native_channel_close
      when 0
        true
      when ERROR_EAGAIN
        Errno::EAGAIN
      else
        raise RuntimeError, "Something went wrong"
      end
    ensure
      @open = false
    end

    def request_pty(term)
      native_request_pty(term)
    end

    def shell
      native_shell
    end

    def channel_exec(cmd)
      native_channel_exec(cmd)
    end

    def send_eof
      native_send_eof
    end

    def flush
      native_flush
    end
  end
end