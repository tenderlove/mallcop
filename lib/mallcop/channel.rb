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
      retval = ERROR_EAGAIN

      until retval != ERROR_EAGAIN
        @session.io_select
        retval = read_nonblock
      end

      retval
    end

    def read_nonblock
      raise ChannelError, "The channel has been closed" unless @open
      native_channel_read
    end

    def write(cmd)
      retval = ERROR_EAGAIN

      until retval != ERROR_EAGAIN
        @session.io_select(true)
        retval = write_nonblock(cmd)
      end

      retval
    end

    def write_nonblock(cmd)
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
      retval = native_channel_exec(cmd)

      until retval != ERROR_EAGAIN
        @session.io_select
        retval = native_channel_exec(cmd)
      end

      retval
    end

    def send_eof
      native_send_eof
    end

    def flush
      native_flush
    end
  end
end