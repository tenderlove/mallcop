module MallCop
  class Channel

    def initialize(session)
      @session, @open = session, true

      block { native_initialize }
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
      block { native_request_pty(term) }
      true
    end

    def shell
      block { native_shell }
      true
    end

    def channel_exec(cmd)
      block { native_channel_exec(cmd) }
      true
    end

    def send_eof
      native_send_eof
    end

    def flush
      native_flush
    end

  private

    def block
      retval = yield

      until retval != ERROR_EAGAIN
        @session.io_select
        retval = yield
      end

      retval
    end

  end
end