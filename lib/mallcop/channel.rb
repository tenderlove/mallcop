module MallCop
  class Channel
    include Errors

    def eof?
      native_eof?
    end
  end
end