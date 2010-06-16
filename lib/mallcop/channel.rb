module MallCop
  class Channel

    def initialize(session)
      @session = session
      native_initialize
    end

    def eof?
      native_eof?
    end
  end
end