module MallCop
  class Channel

    def initialize(session)
      @session, @open = session, true
      native_initialize
    end

    def read
      raise ChannelError, "The channel has been closed" unless @open
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
  end
end