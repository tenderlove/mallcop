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
      block { native_channel_close }
      true
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
      block { native_send_eof }
      true
    end

    def flush
      block { native_flush }
      true
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